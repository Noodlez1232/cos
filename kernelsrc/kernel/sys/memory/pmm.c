/*
 * The MIT License
 *
 * Copyright 2017 nathaniel.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//The phyiscal memory manager (Basically, makes sure we don't put anything where we don't want it)

#include <sys/memory/pmm.h>
#include <display/term.h>
#include <input/term.h>

#include "sys/memory/kmalloc.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"


//This holds the length of the actual tables we have so that we can make sure we don't ever try to iterate through entries that don't even exist
uint8_t mmap_tables_length;
//This holds the base addresses for all the memory map. We create a ridiculous size because we want to make sure there is sufficient space
phys_addr_t mmap_base_addresses[128];
//This holds all the lengths of the memory map.
phys_addr_t mmap_lengths[128];
//This holds all the types of the memory map
uint8_t mmap_types[128];
//This puts the first block of the pmm mapper that that memory location is
uint32_t mmap_first_block[128];


//Here we have the bitmap for the PMM
//The bitmap will itself have its own blocks allocated to prevent programs from
//overwriting it
uint8_t* pmm_bitmap;
//This stores the pmm bitmap's size in bytes
size_t pmm_bitmap_size;

void pmm_init(multiboot_info_t* mbt)
{
    //First we set all those pointer stuffs
    kernel_end = &_kernel_end;
    kernel_start = &_kernel_start;

    //This holds the length of the GRUB memory map as it's passed
    uint32_t mmap_length;
    //Holds the size of one entry in the memory map
    uint32_t mmap_size;
    //This pointer will actually hold the GRUB's memory map 
    multiboot_memory_map_t* mmap;
    //We tell the user what we are doing
#if DEBUG == 1
    terminal_debug_writeline("Setting up physical memory manager");
#endif
    //Now we check to make sure the memory table was passed (6th bit is the flag for if it was passed or not)
    if ((mbt->flags & 0x20) != 0x20)
    {
        //For some reason, it wasn't sent
        terminal_info("Loading of PMM failed! Memory table was not passed to the OS. Halting.", 1);
        halt();
        //Execution should not reach further than this....
    }
    //It was, so that's good.
    //Let's actually get the memory table now (The -4 is to align the table to the size field not the base_addr_low field)
    mmap = (multiboot_memory_map_t*) mbt->mmap_addr;
#if DEBUG==1
    terminal_debug_writestring("mmap address: ");
    terminal_debug_writehexdword((uint32_t) mmap);
    terminal_debug_putchar('\n');
#endif
    //And now we get the length
    mmap_length = mbt->mmap_length;
#if DEBUG==1
    terminal_debug_writestring("mmap length: ");
    terminal_debug_writehexdword(mmap_length);
    terminal_debug_putchar('\n');
#endif
    mmap_size = mmap->size;
#if DEBUG==1
    terminal_debug_writestring("mmap entry size: ");
    terminal_debug_writehexdword(mmap_size);
    terminal_debug_putchar('\n');
#endif
    uint16_t i = 0;
    //Actually get the memory map now, and not just the table's address
    //We do this so that we can make sure that this memory map not only doesn't get overwritten but also gets in a format we can easily use
    while (mmap < (multiboot_memory_map_t*) (mbt->mmap_addr + mmap_length))
    {
        //Now we get the base addresses of the memory map
        mmap_base_addresses[i] = (phys_addr_t) mmap->addr;
        //Then we get the lengths of the memory map
        mmap_lengths[i] = (phys_addr_t) mmap->len;
        //Then we get the types from the memory map
        mmap_types[i] = (uint8_t) (mmap->type & 0xFF);
        //Here we get where the first block in the pmm is
        mmap_first_block[i] = ((uint32_t) mmap_base_addresses[i]) / PMM_BLOCK_SIZE;
        //Increment the current record of the memory map
        mmap = (multiboot_memory_map_t*) ((phys_addr_t) mmap + mmap_size + sizeof (mmap_size));
        //This is incremented to keep track of where we are in allocating blocks
        i++;
    }
    //We make sure that we put the table's length in here
    mmap_tables_length = i;
    //Install the print_pmmtable command
    terminal_installcommand("printpmmtable", &print_pmmtable);
    //Then we find out the ammount of memory we can actually use

    for (uint8_t i = 0; i < mmap_tables_length; i++)
    {
        //We go through every single entry in the pmm and try to find an entry that is available
        if (mmap_types[i] == MULTIBOOT_MEMORY_AVAILABLE)
        {
            //That memory is available, so we add the length of that to our memsize variable
            pmm_usable_memsize += mmap_lengths[i];
        }
    }

    //Here we check to see if the machine has less than the minimum required RAM
    if (pmm_usable_memsize < PMM_MIN_RAM)
    {
        //If so, we warn them, but we don't halt their machine, at this point of
        //making the OS. This will be changed later
        //TODO: Change this when appropriate
        terminal_info("Machine has less than the required ammount of RAM\n", 3);
    }

    //Here we set up our pmm bitmap
    pmm_bitmap = (uint8_t*) kernel_end;
    pmm_bitmap_size = (pmm_usable_memsize / PMM_BLOCK_SIZE);
    //And now we allocate all of the kernel and the bitmap
    pmm_allocate_RAM((phys_addr_t) kernel_start, (size_t) (kernel_start - kernel_end));
    pmm_allocate_RAM((phys_addr_t) pmm_bitmap, pmm_bitmap_size / PMM_BLOCKS_PER_BYTE);
#if DEBUG == 1
    terminal_debug_writestring("Bitmap size: ");
    terminal_debug_itoa(pmm_bitmap_size / 1024);
    terminal_debug_writeline("KB");
#endif

}

void print_pmmtable(char *command)
{
    terminal_writestring("Memory available for use: ");
    terminal_itoa((pmm_usable_memsize / 1024) / 1024);
    terminal_writeline("MB");
    terminal_writeline("Physical memory table info:");
    for (uint8_t i = 0; i < mmap_tables_length; i++)
    {
        terminal_writestring("[");
        terminal_writehexword(i);
        terminal_writestring("] Base address:");
        terminal_writehexdword(mmap_base_addresses[i]);

        terminal_writestring(" | Length:");
        terminal_writehexdword(mmap_lengths[i]);

        terminal_writestring(" | Type:");
        terminal_writehexword(mmap_types[i]);
        terminal_putchar('\n');

    }
}

//Returns true if the address passed is fine for use, and returns false if it is not

bool pmm_check_addr(phys_addr_t addr)
{
    phys_addr_t base_addr;
    phys_addr_t length;
    for (uint8_t i = 0; i < mmap_tables_length; i++)
    {
        //We go through every single address space to make sure it is okay with all of them
        base_addr = mmap_base_addresses[i];
        length = mmap_lengths[i];
        if ((addr > base_addr) && addr < length + base_addr)
        {
            //So, we've found the block in which the address is in, so we check to make sure that it's good to go
            if (mmap_types[i] == MULTIBOOT_MEMORY_AVAILABLE)
                //It is indeed available so we report back it's good to go
                return true;
            else
                //It's not good, so we report back not good to go
                return false;
        }
    }
    //The block wasn't even found which is way worse, so we just return false
    return false;
}

phys_addr_t get_extended_RAM()
{
    //This is really only used for malloc, but whatever, it needs to be put in
    
    bool is_first_section_found = false;
    for (uint8_t i = 0; i < mmap_tables_length; i++)
    {
        //so we check through each one of them
        if (mmap_types[i] == MULTIBOOT_MEMORY_AVAILABLE)
        {
            if (is_first_section_found)
            {
                return mmap_base_addresses[i];
            }
            else
            {
                is_first_section_found = true;
            }
        }
    }
}

/*
 * BEGINNING OF THE PMM BLOCK ALLOCATOR
 */

/*
 * The PMM is as described
 * 
 * We basically bitmap the whole RAM space. This means one block can have one of
 * two states, allocated and unallocated.
 */

//Allocates a block in our PMM's bitmap

void pmm_allocate_block(uint32_t block)
{
    pmm_bitmap[block / PMM_BLOCKS_PER_BYTE] |= 1 << (block % PMM_BLOCKS_PER_BYTE);
}

//Deallocates a block in our PMM's bitmap

void pmm_deallocate_block(uint32_t block)
{
    pmm_bitmap[block / PMM_BLOCKS_PER_BYTE] &= ~(1 << (block % PMM_BLOCKS_PER_BYTE));
}

//Translates a physical address to a block number

uint32_t pmm_translate_phys_addr_to_block_num(phys_addr_t addr)
{
    /*
     * Here we translate a physical address to a block number
     * This is actually pretty simple, we just basically divide the address by
     * the block size and voila, we have our block number
     */
    return addr / PMM_BLOCK_SIZE;
}


//Allocates a certain ammount of RAM in the PMM

void pmm_allocate_RAM(phys_addr_t starting_addr, size_t size)
{
    //First we get the first block of the parts we need to allocate
    uint32_t block = pmm_translate_phys_addr_to_block_num(starting_addr);
    //Then we calculate how many blocks we need to allocate
    uint32_t blocks_to_allocate = (size / PMM_BLOCK_SIZE) + 1;
    //Now we do a loop and allocate all those parts.

#if DEBUG == 1
    terminal_debug_writestring("Allocating ");
    terminal_debug_itoa(size);
    terminal_debug_writestring(" bytes starting at ");
    terminal_debug_writehexdword(starting_addr);
    terminal_debug_writestring(" taking ");
    terminal_debug_itoa(blocks_to_allocate);
    terminal_debug_writeline(" blocks.");
#endif
    for (uint32_t i = 0; i < blocks_to_allocate; i++)
    {
        pmm_allocate_block(block + i);
    }
}

bool pmm_is_block_open(uint32_t block)
{
    return (pmm_bitmap[block / PMM_BLOCKS_PER_BYTE] & (1 << (block % PMM_BLOCKS_PER_BYTE))) == true;
}

#if 0

phys_addr_t kmalloc(size_t size, kheap_t *kheap)
{
    //Had to rewrite this way too many times... As of right now, this is my 3rd time

    /*
     * The first thing we do is figure out mow many blocks we need to allocate.
     * We do this by basically taking the size, dividing it by the pmm's block
     * size and adding 1 (if only one block is needed, the division will return
     * 0, and if we need two blocks, the division will return 1, due to how
     * integer division works on computers)
     */
    //uint32_t blocks_needed = (size / PMM_BLOCK_SIZE) + 1;
    //Now we create an array of blocks that we need to allocate


    //I need this for later
    //phys_addr_t address = (((uint32_t) & _kernel_end & ~(PMM_BLOCK_SIZE - 1)) + PMM_BLOCK_SIZE) + (blocksize * PMM_BLOCK_SIZE);
#if DEBUG == 1
    terminal_debug_itoa(size);
    terminal_debug_writestring(" bytes allocated at ");
    //terminal_debug_writehexdword(address);
    terminal_debug_putchar('\n');
#endif
    //return address;
}
#endif
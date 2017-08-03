//The phyiscal memory manager (Basically, makes sure we don't put anything where we don't want it)

#include <sys/memory/pmm.h>
#include <display/term.h>
#include <input/term.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

//This is the size of the memory we are using
uint32_t pmm_usable_memsize;
//This holds the length of the actual tables we have so that we can make sure we don't ever try to iterate through entries that don't even exist
uint8_t mmap_tables_length;
//This holds the base addresses for all the memory map. We create a ridiculous size because we want to make sure there is sufficient space
phys_addr_t mmap_base_addresses[128];
//This holds all the lengths of the memory map.
phys_addr_t mmap_lengths[128];
//This holds all the types of the memory map
uint8_t mmap_types[128];

void pmm_init(multiboot_info_t* mbt)
{
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
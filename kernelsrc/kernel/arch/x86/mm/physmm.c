#include <arch/x86/mm/physmm.h>
#include <arch/x86/x86.h>

//We first set up our physical memory table
//We set this up to cover all 4GB of the address space
//This bitmap takes up 32KB. There are two states of this bitmap. 1 = used, 0 = free

uint32_t bitmap[32767];

//Allocates a certain ammount of memory.
uint32_t* phys_allocatemem(uint32_t size)
{

}

//Sets a certain block of memory used
void phys_setmemused(uint32_t block)
{
    //Expanded version
    // //First we figure out what index this is
    // uint32_t index = block / 32;
    // //Then we figure out which bit it is
    // uint8_t bit = block % 32;
    //
    // //And now we set that bit
    // bitmap[index] |= 1 << bit;

    bitmap[block / 32] |= 1 << (block % 32);
}

//And now we set a certain block of memory free
void phys_setmemfree(uint32_t block)
{
    //The expanded version is virtually the same as the phys_setmemused() function
    bitmap[block / 32] &= ~(1 << (block % 32));
}

void phys_findfreemem(uint32_t size)
{
    
}

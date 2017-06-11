#include <sys/memory/pmm.h>
#include <display/term.h>
#include <input/term.h>


void pmm_init(multiboot_info_t* mbt)
{
	terminal_installcommand("pmminfo", (command_t)&print_pmmtable);
	//mmap is defined in pmm.h
	mmap = mbt->mmap_addr;
	while(mmap < mbt->mmap_addr + mbt->mmap_length)
	{
		mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(mmap->size) );
	}
}

void print_pmmtable(char *command)
{
	
}
	

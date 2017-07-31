#ifndef _PMM_H
#define _PMM_H 1

#include <common.h>
#include <sys/boot/multiboot.h>

void pmm_init(multiboot_info_t* mbt);

void print_pmmtable(char *command);

#endif
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

#ifndef _PMM_H
#define _PMM_H 1

#include <common.h>
#include <sys/boot/multiboot.h>

extern uint32_t _kernel_end;
extern uint32_t _kernel_start;
uint32_t* kernel_end;
uint32_t* kernel_start;

uint32_t pmm_usable_memsize;

/*
 * We make the block size large enough for it to be usable without there being
 * too many 
 */
#define PMM_BLOCK_SIZE 4096
/*
 * Increasing this and decreasing this doesn't really condense data, so keeping
 * it small makes it easier to manage
 */
#define PMM_BLOCKS_PER_BYTE 8
//It's best to align things to the size of blocks just for ease of organization
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE


// Here we set the minimum ammount of memory we need in bytes (64 MB as of now)
#define PMM_MIN_RAM 67108864



void pmm_init(multiboot_info_t* mbt);
void print_pmmtable(char *command);

bool pmm_check_addr(uint32_t addr);

void pmm_allocate_RAM(phys_addr_t starting_addr, size_t size);

phys_addr_t pmm_allocate_free_RAM(size_t size);

bool pmm_is_block_open(uint32_t block);

#endif

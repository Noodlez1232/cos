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

#include <sys/memory/kmalloc.h>
#include <sys/memory/pmm.h>

//This is a pointer to where the kernel heap will be stored.
uint32_t* kheap;

/*
 * INFORMATION ON HOW THE HEAP WORKS
 * 
 * A heap is basically a way to allocate RAM in an organized way that makes RAM
 * able to be freed and made usable again, if needed.
 * 
 * Design:
 * The heap is dynamically allocated and consists of a structure of
 * typedef struct heap_entry
 * {
 *	heap_header_t header;
 *	data_t data;
 *	heap_footer_t footer;
 * } heap_t;
 * Heap header:
 * typedef struct HeapHdr
 * {
 *	uint32_t magic_num,
 *	uint16_t size
 * } heap_header_t;
 * Heap footer:
 * typedef struct HeapFtr
 * {
 *	uint32_t magic_num,
 *	uint16_t size
 * } heap_footer_t;
 * 
 * In between the heap header and footer is the data that the program can use.
 * If the footer and/or header have invalid magic numbers, and/or their sizes are
 * not matching, they will be marked as bad and reconstruction will be attempted.
 * This is done by looking at the header and footer and look for a correct footer.
 * If both magic numbers are valid, but both size values are mismatched, the block
 * is marked bad and a panic is thrown (for now). If only one of the magic numbers
 * are verified, that structure is used to rebuild the other.
 * 
 * If the magic number for both the header and footer is 0, then the block is
 * unallocated.
 */

void kheap_init()
{
    /*
     * To initialize our kernel's heap, we first find out a size for our heap.
     * For now, this will be a constant defined in kmalloc.h, but can be
     * redesigned to be dynamically growing
     * 
     * We put the heap at the end of the PMM table (or at the beginning of extended
     * memory)
     */
    kheap = (uint32_t)* get_extended_RAM();
}

void * kmalloc(size_t sz)
{
    //I've had to rewrite this like four times now... I'm not good at design...

    //First we test for a blank block in the heap
    

}
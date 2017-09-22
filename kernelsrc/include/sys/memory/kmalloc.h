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

#ifndef KMALLOC_H
#define KMALLOC_H 1
#include <common.h>

/*
 * And here, we set a size for the heap. This is not the size of the heap itself,
 * but instead the ammount of memory the heap can allocate
 * 
 * As of right now, this is 64MB (64 * 1024 * 1024)
 */
#define HEAP_MAX_SIZE 67108864

uint32_t kheap_size;

/*
 * Here we define our heap structures
 */

typedef struct HeapHdr //Header
{
    uint32_t magic_number;
    size_t size;
} heap_header_t;

typedef struct HeapFtr //Footer
{
    uint32_t magic_number;
    size_t size;
} heap_footer_t;

//We define our magic number for the heap here
#define HEAP_MAGIC_NUMBER 0x0B00B135

//We make a type for the kernel heap just to prevent people from giving random
//addresses
typedef uint32_t kheap_t;

#endif
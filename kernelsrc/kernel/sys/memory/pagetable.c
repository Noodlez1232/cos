/*
 * The MIT License
 *
 * Copyright 2017 Nathaniel Barragan.
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

#include <sys/memory/pagetable.h>

//Sets a flag on a pagetable's entry
void pagetable_set_flag(pagetable_entry_t *entry, pagetable_entry_flag_t flag)
{
	*entry |= flag;
}

//Unsets a flag on a pagetable's entry
void pagetable_unset_flag(pagetable_entry_t *entry, pagetable_entry_flag_t flag)
{
	*entry &= ~flag;
}

//Sets the frame address of a pagetable's entry
void pagetable_set_frame_address(pagetable_entry_t *entry, phys_addr_t addr)
{
	*entry = (*entry & ~PTE_FRAME) | addr;
}

//Sets the free bits that are available in the entry
void pagetable_set_free_bits(pagetable_entry_t *entry, uint8_t value)
{
	*entry = (*entry & ~PTE_FREE_BITS) | (value<<10);
}

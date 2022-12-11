/*
 * Copyright (c) 2022 Clément Bœsch <u pkh me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef OKLAB_INT_H
#define OKLAB_INT_H

#include <stdint.h>

/*
 * Lab values are in the [-K;K] range
 * Linear values are in the [0;K] range
 */
#define K ((1 << 16) - 1)

struct LabInt {
    int32_t L, a, b;
};

int32_t srgb_u8_to_linear_int(uint8_t x);
uint8_t linear_int_to_srgb_u8(int32_t x);
int32_t cbrt01_int(int32_t x);
struct LabInt srgb_u8_to_oklab_int(uint32_t srgb);
uint32_t oklab_int_to_srgb_u8(struct LabInt c);

#endif

/*
 * Copyright (c) 2022 Clément Bœsch <u pkh me>
 * Copyright (c) 2020 Björn Ottosson
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

#include <math.h>

#include "oklab_f32_fast.h"

#define P ((1 << 9) - 1)

static const float srgb2linear[256] = {
/* TPL_SRGB2LINEAR_TABLE */
};

static const uint8_t linear2srgb[P + 1] = {
/* TPL_LINEAR2SRGB_TABLE */
};

uint8_t linear_f32_to_srgb_u8_fast(float x)
{
    if (x <= 0.f) {
        return 0;
    } else if (x >= 1.f) {
        return 0xff;
    } else {
        const float i = x * P;
        const int32_t idx = (int32_t)floorf(i);
        const float y0 = linear2srgb[idx];
        const float y1 = linear2srgb[idx + 1];
        const float r = i - idx;
        return lrintf(y0*(1-r) + y1*r);
    }
}

float srgb_u8_to_linear_f32_fast(uint8_t x)
{
    return srgb2linear[x];
}

struct LabFast srgb_u8_to_oklab_f32_fast(uint32_t srgb)
{
    const float r = srgb2linear[srgb >> 16 & 0xff];
    const float g = srgb2linear[srgb >>  8 & 0xff];
    const float b = srgb2linear[srgb       & 0xff];

    const float l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
    const float m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
    const float s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

    const float l_ = cbrtf(l);
    const float m_ = cbrtf(m);
    const float s_ = cbrtf(s);

    const struct LabFast ret = {
        .L = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_,
        .a = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_,
        .b = 0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_,
    };

    return ret;
}

uint32_t oklab_f32_to_srgb_u8_fast(struct LabFast c)
{
    const float l_ = c.L + 0.3963377774f * c.a + 0.2158037573f * c.b;
    const float m_ = c.L - 0.1055613458f * c.a - 0.0638541728f * c.b;
    const float s_ = c.L - 0.0894841775f * c.a - 1.2914855480f * c.b;

    const float l = l_*l_*l_;
    const float m = m_*m_*m_;
    const float s = s_*s_*s_;

    const uint8_t r = linear_f32_to_srgb_u8_fast(+4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s);
    const uint8_t g = linear_f32_to_srgb_u8_fast(-1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s);
    const uint8_t b = linear_f32_to_srgb_u8_fast(-0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s);

    return r<<16 | g<<8 | b;
}

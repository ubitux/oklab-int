/*
 * Copyright (c) 2020 Björn Ottosson
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

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "oklab_int.h"

#define K2 ((int64_t)K*K)
#define P ((1 << 9) - 1)

/**
 * Table mapping formula:
 *   f(x) = x < 0.04045 ? x/12.92 : ((x+0.055)/1.055)^2.4  (sRGB EOTF)
 * Where x is the normalized index in the table and f(x) the value in the table.
 * f(x) is remapped to [0;K] and rounded.
 */
static const uint16_t srgb2linear[256] = {
/* TPL_SRGB2LINEAR_TABLE */
};

/**
 * Table mapping formula:
 *   f(x) = x < 0.0031308 ? x*12.92 : (1.055)*x^(1/2.4)-0.055  (sRGB OETF)
 * Where x is the normalized index in the table and f(x) the value in the table.
 * f(x) is remapped to [0;0xff] and rounded.
 *
 * Since a 16-bit table is too large, we reduce its precision to 9-bit.
 */
static const uint8_t linear2srgb[P + 1] = {
/* TPL_LINEAR2SRGB_TABLE */
};

int32_t srgb_u8_to_linear_int(uint8_t x)
{
    return (int32_t)srgb2linear[x];
}

uint8_t linear_int_to_srgb_u8(int32_t x)
{
    if (x <= 0) {
        return 0;
    } else if (x >= K) {
        return 0xff;
    } else {
        const int32_t xP = x * P;
        const int32_t i = xP / K;
        const int32_t m = xP % K;
        const int32_t y0 = linear2srgb[i];
        const int32_t y1 = linear2srgb[i + 1];
        return (m * (y1 - y0) + K/2) / K + y0;
    }
}

#define CBRT_B 0
#define CBRT_C 0
#define CBRT_D 0

/* Integer cube root, working only within [0;1] */
int32_t cbrt01_int(int32_t x)
{
    int64_t u;

    /* Approximation curve is for the [0;1] range */
    if (x <= 0) return 0;
    if (x >= K) return K;

    /*
     * Initial approximation: x³ - 2.19893x² + 2.01593x + 0.219407
     *
     * We are not using any rounding here since the precision is not important
     * at this stage and it would require the more expensive rounding function
     * that deals with negative numbers.
     */
    u = x*(x*(x + CBRT_B) / K + CBRT_C) / K + CBRT_D;

    /*
     * Refine with 2 Halley iterations:
     *   uₙ₊₁ = uₙ-2f(uₙ)f'(uₙ)/(2f'(uₙ)²-f(uₙ)f"(uₙ))
     *        = uₙ(2x+uₙ³)/(x+2uₙ³)
     *
     * Note: u is not expected to be < 0, so we can use the (a+b/2)/b rounding.
     */
    for (int i = 0; i < 2; i++) {
        const int64_t u3 = u*u*u;
        const int64_t den = x + (2*u3 + K2/2) / K2;
        u = (u * (2*x + (u3 + K2/2) / K2) + den/2) / den;
    }

    return u;
}

static int64_t div_round64(int64_t a, int64_t b) { return (a^b)<0 ? (a-b/2)/b : (a+b/2)/b; }

#define S2L_LR 0
#define S2L_LG 0
#define S2L_LB 0
#define S2L_MR 0
#define S2L_MG 0
#define S2L_MB 0
#define S2L_SR 0
#define S2L_SG 0
#define S2L_SB 0

#define S2L_LL 0
#define S2L_LM 0
#define S2L_LS 0
#define S2L_AL 0
#define S2L_AM 0
#define S2L_AS 0
#define S2L_BL 0
#define S2L_BM 0
#define S2L_BS 0

struct LabInt srgb_u8_to_oklab_int(uint32_t srgb)
{
    const int32_t r = (int32_t)srgb2linear[srgb >> 16 & 0xff];
    const int32_t g = (int32_t)srgb2linear[srgb >>  8 & 0xff];
    const int32_t b = (int32_t)srgb2linear[srgb       & 0xff];

    // Note: lms can actually be slightly over K due to rounded coefficients
    const int32_t l = (S2L_LR*r + S2L_LG*g + S2L_LB*b + K/2) / K;
    const int32_t m = (S2L_MR*r + S2L_MG*g + S2L_MB*b + K/2) / K;
    const int32_t s = (S2L_SR*r + S2L_SG*g + S2L_SB*b + K/2) / K;

    const int32_t l_ = cbrt01_int(l);
    const int32_t m_ = cbrt01_int(m);
    const int32_t s_ = cbrt01_int(s);

    const struct LabInt ret = {
        .L = div_round64(S2L_LL*l_ + S2L_LM*m_ + S2L_LS*s_, K),
        .a = div_round64(S2L_AL*l_ + S2L_AM*m_ + S2L_AS*s_, K),
        .b = div_round64(S2L_BL*l_ + S2L_BM*m_ + S2L_BS*s_, K),
    };

    return ret;
}

#define L2S_LA 0
#define L2S_LB 0
#define L2S_MA 0
#define L2S_MB 0
#define L2S_SA 0
#define L2S_SB 0

#define L2S_RL 0
#define L2S_RM 0
#define L2S_RS 0
#define L2S_GL 0
#define L2S_GM 0
#define L2S_GS 0
#define L2S_BL 0
#define L2S_BM 0
#define L2S_BS 0

uint32_t oklab_int_to_srgb_u8(struct LabInt c)
{
    const int64_t l_ = c.L + div_round64(L2S_LA * c.a, K) + div_round64(L2S_LB * c.b, K);
    const int64_t m_ = c.L + div_round64(L2S_MA * c.a, K) + div_round64(L2S_MB * c.b, K);
    const int64_t s_ = c.L + div_round64(L2S_SA * c.a, K) + div_round64(L2S_SB * c.b, K);

    const int32_t l = l_*l_*l_ / K2;
    const int32_t m = m_*m_*m_ / K2;
    const int32_t s = s_*s_*s_ / K2;

    const uint8_t r = linear_int_to_srgb_u8((L2S_RL * l + L2S_RM * m + L2S_RS * s + K/2) / K);
    const uint8_t g = linear_int_to_srgb_u8((L2S_GL * l + L2S_GM * m + L2S_GS * s + K/2) / K);
    const uint8_t b = linear_int_to_srgb_u8((L2S_BL * l + L2S_BM * m + L2S_BS * s + K/2) / K);

    return r<<16 | g<<8 | b;
}

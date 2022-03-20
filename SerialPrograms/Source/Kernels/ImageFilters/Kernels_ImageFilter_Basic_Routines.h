/*  Image Filters Basic Routines
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_Kernels_ImageFilter_Basic_Routines_H
#define PokemonAutomation_Kernels_ImageFilter_Basic_Routines_H

#include <stddef.h>
#include <stdint.h>
#include "Common/Compiler.h"

namespace PokemonAutomation{
namespace Kernels{


template <typename Filter>
PA_FORCE_INLINE void filter_rbg32(
    const uint32_t* image, size_t bytes_per_row, size_t width, size_t height,
    uint32_t* out0, size_t bytes_per_row0, const Filter& filter0
){
    if (width == 0 || height == 0){
        return;
    }
    const size_t VECTOR_SIZE = Filter::VECTOR_SIZE;
    do{
        const uint32_t* in = image;
        uint32_t* o0 = out0;
        size_t lc = width / VECTOR_SIZE;
        do{
            filter0.process_full(o0, in);
            in += VECTOR_SIZE;
            o0 += VECTOR_SIZE;
        }while (--lc);
        size_t left = width % VECTOR_SIZE;
        if (left != 0){
            filter0.process_partial(o0, in, left);
        }
        image = (const uint32_t*)((const char*)image + bytes_per_row);
        out0 = (uint32_t*)((const char*)out0 + bytes_per_row0);
    }while (--height);
}
template <typename Filter>
PA_FORCE_INLINE void filter2_rbg32(
    const uint32_t* image, size_t bytes_per_row, size_t width, size_t height,
    uint32_t* out0, size_t bytes_per_row0, const Filter& filter0,
    uint32_t* out1, size_t bytes_per_row1, const Filter& filter1
){
    if (width == 0 || height == 0){
        return;
    }
    const size_t VECTOR_SIZE = Filter::VECTOR_SIZE;
    do{
        const uint32_t* in = image;
        uint32_t* o0 = out0;
        uint32_t* o1 = out1;
        size_t lc = width / VECTOR_SIZE;
        do{
            filter0.process_full(o0, in);
            filter1.process_full(o1, in);
            in += VECTOR_SIZE;
            o0 += VECTOR_SIZE;
            o1 += VECTOR_SIZE;
        }while (--lc);
        size_t left = width % VECTOR_SIZE;
        if (left != 0){
            filter0.process_partial(o0, in, left);
            filter1.process_partial(o1, in, left);
        }
        image = (const uint32_t*)((const char*)image + bytes_per_row);
        out0 = (uint32_t*)((const char*)out0 + bytes_per_row0);
        out1 = (uint32_t*)((const char*)out1 + bytes_per_row1);
    }while (--height);
}
template <typename Filter>
PA_FORCE_INLINE void filter4_rbg32(
    const uint32_t* image, size_t bytes_per_row, size_t width, size_t height,
    uint32_t* out0, size_t bytes_per_row0, const Filter& filter0,
    uint32_t* out1, size_t bytes_per_row1, const Filter& filter1,
    uint32_t* out2, size_t bytes_per_row2, const Filter& filter2,
    uint32_t* out3, size_t bytes_per_row3, const Filter& filter3
){
    if (width == 0 || height == 0){
        return;
    }
    const size_t VECTOR_SIZE = Filter::VECTOR_SIZE;
    do{
        const uint32_t* in = image;
        uint32_t* o0 = out0;
        uint32_t* o1 = out1;
        uint32_t* o2 = out2;
        uint32_t* o3 = out3;
        size_t lc = width / VECTOR_SIZE;
        do{
            filter0.process_full(o0, in);
            filter1.process_full(o1, in);
            filter2.process_full(o2, in);
            filter3.process_full(o3, in);
            in += VECTOR_SIZE;
            o0 += VECTOR_SIZE;
            o1 += VECTOR_SIZE;
            o2 += VECTOR_SIZE;
            o3 += VECTOR_SIZE;
        }while (--lc);
        size_t left = width % VECTOR_SIZE;
        if (left != 0){
            filter0.process_partial(o0, in, left);
            filter1.process_partial(o1, in, left);
            filter2.process_partial(o2, in, left);
            filter3.process_partial(o3, in, left);
        }
        image = (const uint32_t*)((const char*)image + bytes_per_row);
        out0 = (uint32_t*)((const char*)out0 + bytes_per_row0);
        out1 = (uint32_t*)((const char*)out1 + bytes_per_row1);
        out2 = (uint32_t*)((const char*)out2 + bytes_per_row2);
        out3 = (uint32_t*)((const char*)out3 + bytes_per_row3);
    }while (--height);
}



}
}
#endif
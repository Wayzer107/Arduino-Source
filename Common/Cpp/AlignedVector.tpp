/*  Aligned Vector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_AlignedVector_TPP
#define PokemonAutomation_AlignedVector_TPP

#include <new>
#include <type_traits>
#include <utility>
#include <immintrin.h>
#include "Common/Compiler.h"
#include "AlignedVector.h"

namespace PokemonAutomation{



template <typename Object>
AlignedVector<Object>::~AlignedVector(){
    clear();
    _mm_free(m_ptr);
    m_capacity = 0;
}
template <typename Object>
AlignedVector<Object>::AlignedVector(AlignedVector&& x)
    : m_ptr(x.m_ptr)
    , m_size(x.m_size)
    , m_capacity(x.m_capacity)
{
    x.m_ptr = nullptr;
    x.m_size = 0;
    x.m_capacity = 0;
}
template <typename Object>
void AlignedVector<Object>::operator=(AlignedVector&& x){
    clear();
    _mm_free(m_ptr);
    m_ptr = x.m_ptr;
    m_size = x.m_size;
    m_capacity = x.m_capacity;
    x.m_ptr = nullptr;
    x.m_size = 0;
    x.m_capacity = 0;
}
template <typename Object>
AlignedVector<Object>::AlignedVector(const AlignedVector& x)
    : m_capacity(x.m_capacity)
{
    //  Shrink to fit.
    while (m_capacity > 0){
        size_t half = m_capacity / 2;
        if (half >= x.m_size){
            m_capacity = half;
        } else{
            break;
        }
    }
    m_ptr = (Object*)_mm_malloc(m_capacity * sizeof(Object), 64);
    if (m_ptr == nullptr){
        throw std::bad_alloc();
    }

    m_size = 0;
    try{
        for (size_t c = 0; c < x.m_size; c++){
            new (m_ptr + m_size) Object(x[c]);
            m_size++;
        }
    }catch (...){
        clear();
        _mm_free(m_ptr);
        throw;
    }
}
template <typename Object>
void AlignedVector<Object>::operator=(const AlignedVector& x){
    if (this == &x){
        return;
    }
    AlignedVector tmp(x);
    *this = std::move(tmp);
}



template <typename Object>
AlignedVector<Object>::AlignedVector(size_t items){
    m_ptr = (Object*)_mm_malloc(items * sizeof(Object), 64);
    if (m_ptr == nullptr){
        throw std::bad_alloc();
    }
    m_capacity = items;

    if (std::is_trivially_constructible<Object>::value){
        m_size = items;
        return;
    }

    m_size = 0;
    try{
        for (size_t c = 0; c < items; c++){
            new (m_ptr + m_size) Object;
            m_size++;
        }
    }catch (...){
        clear();
        _mm_free(m_ptr);
        throw;
    }
}

template <typename Object>
template <class... Args>
void AlignedVector<Object>::emplace_back(Args&&... args){
    if (m_size >= m_capacity){
        expand();
    }
    new (m_ptr + m_size) Object(std::forward<Args>(args)...);
    m_size++;
}
template <typename Object>
void AlignedVector<Object>::pop_back(){
    m_ptr[--m_size].~Object();
}
template <typename Object>
void AlignedVector<Object>::clear(){
    while (m_size > 0){
        pop_back();
    }
}

template <typename Object>
PA_NO_INLINE void AlignedVector<Object>::expand(){
    size_t size = m_capacity == 0 ? 1 : m_capacity * 2;
    Object* ptr = (Object*)_mm_malloc(size * sizeof(Object), 64);
    if (ptr == nullptr){
        throw std::bad_alloc();
    }
    for (size_t c = 0; c < m_size; c++){
        new (ptr + c) Object(std::move(m_ptr[c]));
        m_ptr[c].~Object();
    }
    _mm_free(m_ptr);
    m_ptr = ptr;
    m_capacity = size;
}







}
#endif

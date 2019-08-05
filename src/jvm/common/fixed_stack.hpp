////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//              ColdSpot, a Java virtual machine implementation.              //
//                    Copyright (C) 2014, Mario Morgenthum                    //
//                                                                            //
//                                                                            //
//  This program is free software: you can redistribute it and/or modify      //
//  it under the terms of the GNU General Public License as published by      //
//  the Free Software Foundation, either version 3 of the License, or         //
//  (at your option) any later version.                                       //
//                                                                            //
//  This program is distributed in the hope that it will be useful,           //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of            //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             //
//  GNU General Public License for more details.                              //
//                                                                            //
//  You should have received a copy of the GNU General Public License         //
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef COLDSPOT_JVM_COMMON_FIXEDSTACK_HPP_
#define COLDSPOT_JVM_COMMON_FIXEDSTACK_HPP_

#include <cassert>
#include <cstdint>

#include <jvm/common/Memory.hpp>
#include <jvm/Logging.hpp>

namespace coldspot
{

    template<typename T>
    class fixed_stack
    {
    public:

        fixed_stack() : _values(0), _capacity(0), _size(0)
        {
        }

        fixed_stack(const fixed_stack &other) = delete;

        ~fixed_stack()
        {

            DELETE_ARRAY(_values);
        }

        fixed_stack &operator=(const fixed_stack &other) = delete;

        void push(const T &value)
        {
#if IS_LOG_LEVEL_DEBUG
            assert(!full());
#endif
            _values[_size++] = value;
        }

        void clear()
        {
            _size = 0;
        }

        bool empty() const
        {
            return _size == 0;
        }

        bool full() const
        {
            return _size == _capacity;
        }

        T &get(int32_t index) const
        {
            return _values[index];
        }

        void init(uint32_t size)
        {
            _capacity = size;
            _values = new T[_capacity];
        }

        T &peek() const
        {
#if IS_LOG_LEVEL_DEBUG
            assert(!empty());
#endif
            return _values[_size - 1];
        }

        T pop()
        {
#if IS_LOG_LEVEL_DEBUG
            assert(!empty());
#endif
            return _values[--_size];
        }

        uint32_t capacity() const
        {
            return _capacity;
        }

        uint32_t size() const
        {
            return _size;
        }

    private:

        T *_values;
        uint32_t _capacity;
        uint32_t _size;
    };

}

#endif

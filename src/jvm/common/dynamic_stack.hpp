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

#ifndef COLDSPOT_JVM_COMMON_DYNAMICSTACK_HPP_
#define COLDSPOT_JVM_COMMON_DYNAMICSTACK_HPP_

#include <cstdint>
#include <cstring>

#include <jvm/common/Memory.hpp>

namespace coldspot
{

    class dynamic_stack
    {
    public:

        class Iterator
        {
        public:

            Iterator(dynamic_stack *stack, uint32_t position) : _stack(stack),
                                                                _position(
                                                                    position) { }


            uint8_t *operator*()
            {
                return &_stack->_bytes[_position];
            }


            Iterator &operator++()
            {
                if (_position == 0)
                {
                    _position = -1;
                }
                else
                {
                    uint32_t *frameSize = (uint32_t * ) &
                                          _stack->_bytes[_position -
                                                         sizeof(uint32_t)];

                    _position -= sizeof(uint32_t);
                    _position -= *frameSize;
                }

                return *this;
            }


            friend bool operator==(const Iterator &lhs, const Iterator &rhs)
            {
                return lhs._stack == rhs._stack &&
                       lhs._position == rhs._position;
            }


            friend bool operator!=(const Iterator &lhs, const Iterator &rhs)
            {
                return !(lhs == rhs);
            }

        private:

            dynamic_stack *_stack;
            uint32_t _position;
        };

        dynamic_stack() : _position(0), _size(0), _capacity(0), _bytes(0) { }


        ~dynamic_stack()
        {
            DELETE_ARRAY(_bytes)
        }


        void init(uint32_t capacity)
        {
            _capacity = capacity;
            _bytes = new uint8_t[_capacity];
        }


        Iterator begin()
        {
            if (_position == 0)
            {
                return end();
            }

            uint32_t *frameSize =
                (uint32_t * ) & _bytes[_position - sizeof(uint32_t)];

            uint32_t lastPosition = _position;
            lastPosition -= sizeof(uint32_t);
            lastPosition -= *frameSize;

            return Iterator(this, lastPosition);
        }


        Iterator end()
        {
            return Iterator(this, -1);
        }


        uint8_t *push(uint32_t size)
        {
            // Current position
            uint8_t *current = &_bytes[_position];

            // Set reserved memory to 0
            memset((void *) current, 0, size);

            // Reserve size bytes
            _position += size;

            // Set the size on top
            uint32_t *frameSize = (uint32_t * ) & _bytes[_position];
            *frameSize = size;

            // Increment position for size
            _position += sizeof(uint32_t);

            // Increment count of elements
            ++_size;

            return current;
        }


        uint8_t *peek()
        {
            // Get a pointer to the size of the last frame
            uint32_t *frameSize =
                (uint32_t * ) & _bytes[_position - sizeof(uint32_t)];

            // Calculate pointer to the start of the last frame
            uint32_t lastPosition = _position;
            lastPosition -= sizeof(uint32_t);
            lastPosition -= *frameSize;

            // Pointer to last frame
            return &_bytes[lastPosition];
        }


        void pop()
        {
            // Get a pointer to the size of the last frame
            uint32_t *frameSize =
                (uint32_t * ) & _bytes[_position - sizeof(uint32_t)];

            // Invalidate last frame
            _position -= sizeof(uint32_t);
            _position -= *frameSize;

            // Decrement count of elements
            --_size;
        }


        bool empty() const
        {
            return position() == 0;
        }


        uint32_t position() const
        {
            return _position;
        }


        uint32_t size() const
        {
            return _size;
        }


        uint32_t capacity() const
        {
            return _capacity;
        }

    private:

        uint32_t _position;
        uint32_t _size;
        uint32_t _capacity;
        uint8_t *_bytes;
    };

}

#endif

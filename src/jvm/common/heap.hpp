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

#ifndef COLDSPOT_JVM_COMMON_UNORDEREDARRAYLIST_HPP_
#define COLDSPOT_JVM_COMMON_UNORDEREDARRAYLIST_HPP_

#include <cstdint>

#include <jvm/common/Memory.hpp>
#include <jvm/Logging.hpp>
#include <jvm/Object.hpp>
#include <jvm/Options.hpp>

namespace coldspot
{

    template<typename T>
    class heap
    {
    public:

        class iterator
        {
        public:

            iterator(heap<T> *list, uint32_t index) : _list(list), _index(index)
            {
            }


            T operator*()
            {
                return (*_list)[_index];
            }


            iterator &operator++()
            {
                ++_index;
                return *this;
            }


            iterator &operator--()
            {
                --_index;
                return *this;
            }


            friend bool operator==(const heap<T>::iterator &lhs,
                const heap<T>::iterator &rhs)
            {
                return lhs._list == rhs._list && lhs._index == rhs._index;
            }


            friend bool operator!=(const heap<T>::iterator &lhs,
                const heap<T>::iterator &rhs)
            {
                return !(lhs == rhs);
            }

            heap<T> *_list;
            uint32_t _index;
        };


        heap() : _elements(0), _capacity(0), _size(0)
        {
            resize(1000);
        }


        heap(const heap<T> &rhs) = delete;


        ~heap()
        {
            FREE_OBJECT(_elements)
            _capacity = 0;
            _size = 0;
        }


        heap &operator=(const heap<T> &rhs) = delete;


        T operator[](uint32_t index)
        {
            return _elements[index];
        }


        void add(T element)
        {
            if (_size == _capacity)
            {
                resize(_capacity * 1.25);
            }
            _elements[_size++] = element;
        }


        iterator begin()
        {
            return iterator(this, 0);
        }


        iterator end()
        {
            return iterator(this, _size);
        }


        // TODO memleak?
        iterator erase(iterator iterator)
        {
            _elements[iterator._index] = _elements[--_size];
            if (_size < _capacity * 0.5)
            {
                resize(_capacity * 0.75);
            }
            return iterator;
        }


        iterator find(T element)
        {
            for (uint32_t i = 0; i < _size; ++i)
            {
                if (_elements[i] == element)
                {
                    return Iterator(this, i);
                }
            }
            return end();
        }


        uint32_t size() const
        {
            return _size;
        }

    private:

        T *_elements;
        uint32_t _capacity;
        uint32_t _size;

        void resize(uint32_t capacity)
        {
            _elements = (T *) realloc(_elements, sizeof(T) * capacity);
            _capacity = capacity;
        }
    };

}

#endif

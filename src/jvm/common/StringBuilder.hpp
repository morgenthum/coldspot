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

#ifndef COLDSPOT_JVM_COMMON_STRINGBUILDER_HPP_
#define COLDSPOT_JVM_COMMON_STRINGBUILDER_HPP_

#include <sstream>

#include "Memory.hpp"
#include "String.hpp"

namespace coldspot
{

    class StringBuilder
    {
    public:

        StringBuilder() : _buffer(0), _length(0), _capacity(0)
        {
            reserve(1024);
        }

        StringBuilder(const StringBuilder &other) = delete;

        ~StringBuilder()
        {
            clear();
        }

        StringBuilder &operator=(const StringBuilder &other) = delete;

        template<typename T>
        StringBuilder &append(const T &value)
        {
            std::stringstream stream;
            stream << value;

            return append(String(stream.str().c_str()));
        }

        StringBuilder &append(const String &value)
        {
            ensureCapacity(_length + value.length());

            memcpy(_buffer + _length, value.c_str(), value.length());
            _length += value.length();

            return *this;
        }

        void clear()
        {
            DELETE_ARRAY(_buffer);
            _buffer = 0;
            _length = 0;
            _capacity = 0;
        }

        String str()
        {
            return String(_buffer, _length);
        }

        template<typename T>
        friend StringBuilder &operator<<(StringBuilder &lhs, const T &rhs)
        {
            return lhs.append(rhs);
        }

    private:

        char *_buffer;
        uint32_t _length;
        uint32_t _capacity;

        void ensureCapacity(uint32_t capacity)
        {
            uint32_t reserveCapacity = _capacity;
            while (reserveCapacity < capacity)
            {
                reserveCapacity *= 1.5;
            }

            if (reserveCapacity != _capacity)
            {
                reserve(reserveCapacity);
            }
        }

        void reserve(uint32_t capacity)
        {
            char *newBuffer = new char[capacity]();

            if (_buffer)
            {
                memcpy(newBuffer, _buffer, _length);
                delete[] _buffer;
            }

            _buffer = newBuffer;
            _capacity = capacity;
        }
    };

}

#endif

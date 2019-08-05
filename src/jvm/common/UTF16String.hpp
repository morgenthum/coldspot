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

#ifndef COLDSPOT_JVM_COMMON_UTF16STRING_HPP_
#define COLDSPOT_JVM_COMMON_UTF16STRING_HPP_

#include <cstring>

#include "Memory.hpp"

namespace coldspot
{

    class String;

    uint16_t strlen16(const char16_t *cstr);

    class UTF16String
    {
    public:

        UTF16String(const char16_t *cstr)
        {
            copyFrom(cstr, strlen16(cstr));
        }

        UTF16String(const char16_t *cstr, uint32_t length)
        {
            copyFrom(cstr, length);
        }

        UTF16String(const UTF16String &other)
        {
            copyFrom(other._value, other._length);
        }

        ~UTF16String()
        {
            clear();
        }

        UTF16String &operator=(const UTF16String &other) = delete;

        operator String() const;

        char16_t &operator[](uint32_t index) const
        {
            return _value[index];
        }

        void clear()
        {
            DELETE_ARRAY(_value);
            _length = 0;
        }

        bool empty() const
        {
            return length() == 0;
        }

        uint32_t length() const
        {
            return _length;
        }

        const char16_t *toCString() const
        {
            return _value;
        }

    private:

        uint32_t _length;
        char16_t *_value;

        void copyFrom(const char16_t *value, uint32_t length)
        {
            _length = length;
            _value = new char16_t[_length + 1];
            memcpy(_value, value, sizeof(char16_t) * length);
            _value[_length] = 0;
        }

        uint16_t getUTF8Length() const;
    };

}

#endif

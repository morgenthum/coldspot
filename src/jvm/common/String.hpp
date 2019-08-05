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

#ifndef COLDSPOT_JVM_COMMON_STRING_HPP_
#define COLDSPOT_JVM_COMMON_STRING_HPP_

#include <cstring>
#include <sstream>

#include "Hashable.hpp"
#include "Memory.hpp"

namespace coldspot
{

    class UTF16String;

    class String : public Hashable
    {
    public:

        template<typename T>
        static String valueOf(T value)
        {
            std::stringstream builder;
            builder << value;
            return builder.str().c_str();
        }

        String() : _length(0), _value(0)
        {
        }

        String(const String &other)
        {
            copyFrom(other._value, other._length);
        }

        String(const char *cstr)
        {
            copyFrom(cstr, strlen(cstr));
        }

        String(const char *cstr, uint32_t length)
        {
            copyFrom(cstr, length);
        }

        String(char c)
        {
            char chars[2];
            chars[0] = c;
            chars[1] = 0;

            copyFrom(chars, 1);
        }

        ~String()
        {
            clear();
        }

        operator UTF16String() const;

        String &operator=(const String &other)
        {
            clear();
            copyFrom(other._value, other._length);

            return *this;
        }

        char &operator[](uint32_t index) const
        {
            return _value[index];
        }

        bool operator==(const String &other) const
        {
            return strcmp(_value, other._value) == 0;
        }

        bool operator!=(const String &other) const
        {
            return !(*this == other);
        }

        void clear()
        {
            _length = 0;
            DELETE_ARRAY(_value);
        }

        bool empty() const
        {
            return length() == 0;
        }

        uint64_t hashCode() const
        {
            uint64_t hash = 5381;
            char *str = _value;
            int c;

            while ((c = *str++) != 0)
            {
                hash = ((hash << 5) + hash) + c;
            }

            return hash;
        }

        uint32_t length() const
        {
            return _length;
        }

        void replace(char current, char replacement)
        {
            for (uint32_t i = 0; i < length(); ++i)
            {
                if (_value[i] == current)
                {
                    _value[i] = replacement;
                }
            }
        }

        bool startsWith(const String &other) const
        {
            if (length() < other.length())
            {
                return false;
            }

            if (length() == other.length())
            {
                return *this == other;
            }

            for (uint32_t i = 0; i < other.length(); ++i)
            {
                if (_value[i] != other._value[i])
                {
                    return false;
                }
            }

            return true;
        }

        String substring(uint32_t begin, uint32_t length = 0) const
        {
            if (length == 0)
            {
                length = _length - begin;
            }

            char substring[length + 1];
            memcpy(substring, _value + begin, length);
            substring[length] = 0;

            return substring;
        }

        const char *c_str() const
        {
            return _value;
        }

        friend String operator+(const String &lhs, const String &rhs)
        {
            uint32_t lhsLength = lhs.length();
            uint32_t rhsLength = rhs.length();
            uint32_t length = lhsLength + rhsLength;

            char temp[length + 1];
            memcpy(temp, lhs.c_str(), lhsLength);
            memcpy(temp + lhsLength, rhs.c_str(), rhsLength);
            temp[length] = 0;

            return temp;
        }

        friend bool operator<(const String &lhs, const String &rhs)
        {
            return strcmp(lhs.c_str(), rhs.c_str()) < 0;
        }

    private:

        uint32_t _length;
        char *_value;

        void copyFrom(const char *value, uint32_t length)
        {
            _length = length;
            _value = new char[_length + 1];
            memcpy(_value, value, length);
            _value[_length] = 0;
        }
    };

}

#endif
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

#include "utf16_string.hpp"

#include "utf8_string.hpp"

namespace coldspot
{

    utf16_string::operator utf8_string() const
    {

        char chars[length_utf8()];
        size_t size = 0;

        char16_t *current = (char16_t *) c_str();

        for (size_t i = 0; i < length(); ++i)
        {
            char16_t c = *current++;
            if ((c == 0) || (c > 0x7f))
            {
                if (c > 0x7ff)
                {
                    chars[size++] = (c >> 12) | 0xe0;
                    chars[size++] = ((c >> 6) & 0x3f) | 0x80;
                }
                else
                {
                    chars[size++] = (c >> 6) | 0xc0;
                }
                chars[size++] = (c & 0x3f) | 0x80;
            }
            else
            {
                chars[size++] = c;
            }
        }

        return utf8_string(chars, size);
    }

    size_t utf16_string::length_utf8() const
    {

        size_t size = 0;

        for (size_t i = 0; i < length(); ++i)
        {
            char16_t c = at(i);
            size += c == 0 || c > 0x7f ? (c > 0x7ff ? 3 : 2) : 1;
        }

        return size;
    }

}

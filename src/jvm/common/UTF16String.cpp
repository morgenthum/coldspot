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

#include "Global.hpp"

namespace coldspot
{

    uint16_t strlen16(const char16_t *cstr)
    {

        uint16_t length = 0;

        while (cstr[length] != 0)
        {
            ++length;
        }

        return length;
    }


    UTF16String::operator String() const
    {

        char16_t *utf16Ptr = _value;

        char utf8[getUTF8Length()];
        int length = 0;

        for (int i = 0; i < _length; ++i)
        {
            char16_t c = *utf16Ptr++;
            if ((c == 0) || (c > 0x7f))
            {
                if (c > 0x7ff)
                {
                    utf8[length++] = (c >> 12) | 0xe0;
                    utf8[length++] = ((c >> 6) & 0x3f) | 0x80;
                }
                else
                {
                    utf8[length++] = (c >> 6) | 0xc0;
                }
                utf8[length++] = (c & 0x3f) | 0x80;
            }
            else
            {
                utf8[length++] = c;
            }
        }

        return String(utf8, length);
    }


    uint16_t UTF16String::getUTF8Length() const
    {

        int length = 0;

        for (uint16_t i = 0; i < _length; ++i)
        {
            char16_t c = _value[i];
            length += c == 0 || c > 0x7f ? (c > 0x7ff ? 3 : 2) : 1;
        }

        return length;
    }

}

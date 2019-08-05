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

#include "utf8_string.hpp"

#include "utf16_string.hpp"

namespace coldspot
{

    utf8_string::operator utf16_string() const
    {
        char16_t chars[length()];
        size_t size = 0;

        for (size_t i = 0; i < length();)
        {
            int x = at(i++);
            if (x & 0x80)
            {
                int y = at(i++);
                if (x & 0x20)
                {
                    int z = at(i++);
                    chars[size++] =
                        ((x & 0xf) << 12) + ((y & 0x3f) << 6) + (z & 0x3f);
                }
                else
                {
                    chars[size++] = ((x & 0x1f) << 6) + (y & 0x3f);
                }
            }
            else
            {
                chars[size++] = x;
            }
        }
        return utf16_string(chars, size);
    }

}

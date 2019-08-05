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

    String::operator UTF16String() const
    {

        char16_t chars[_length];
        uint16_t charLength = 0;

        for (int i = 0; i < _length;)
        {
            int x = _value[i++];
            if (x & 0x80)
            {
                int y = _value[i++];
                if (x & 0x20)
                {
                    int z = _value[i++];
                    chars[charLength++] =
                        ((x & 0xf) << 12) + ((y & 0x3f) << 6) + (z & 0x3f);
                }
                else
                {
                    chars[charLength++] = ((x & 0x1f) << 6) + (y & 0x3f);
                }
            }
            else
            {
                chars[charLength++] = x;
            }
        }

        return UTF16String(chars, charLength);
    }

}

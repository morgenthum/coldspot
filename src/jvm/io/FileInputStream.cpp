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

#include <jvm/Global.hpp>

namespace coldspot
{

    FileInputStream::~FileInputStream()
    {
        close();
    }


    bool FileInputStream::open(const char *filePath)
    {
        return (_handle = fopen(filePath, "r")) != 0;
    }


    void FileInputStream::close()
    {
        if (_handle != 0)
        {
            fclose(_handle);
            _handle = 0;
        }
    }


    size_t FileInputStream::read(uint8_t *buffer, size_t offset, size_t length)
    {
        return fread(buffer + offset, 1, length, _handle);
    }

}
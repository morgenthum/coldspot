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

#ifndef COLDSPOT_JVM_CLASSFILE_ACCESSFLAGS_HPP_
#define COLDSPOT_JVM_CLASSFILE_ACCESSFLAGS_HPP_

#include <cstdint>

namespace coldspot
{

    enum AccessFlags : uint16_t
    {
        ACCESS_FLAG_PUBLIC = 0x0001,
        ACCESS_FLAG_PRIVATE = 0x0002,
        ACCESS_FLAG_PROTECTED = 0x0004,
        ACCESS_FLAG_STATIC = 0x0008,
        ACCESS_FLAG_FINAL = 0x0010,
        ACCESS_FLAG_SUPER = 0x0020,
        ACCESS_FLAG_SYNCHRONIZED = 0x0020,
        ACCESS_FLAG_BRIDGE = 0x0040,
        ACCESS_FLAG_VOLATILE = 0x0040,
        ACCESS_FLAG_TRANSIENT = 0x0080,
        ACCESS_FLAG_VARARGS = 0x0080,
        ACCESS_FLAG_NATIVE = 0x0100,
        ACCESS_FLAG_INTERFACE = 0x0200,
        ACCESS_FLAG_ABSTRACT = 0x0400,
        ACCESS_FLAG_STRICT = 0x0800,
        ACCESS_FLAG_SYNTHETIC = 0x1000,
        ACCESS_FLAG_ANNOTATION = 0x2000,
        ACCESS_FLAG_ENUM = 0x4000,
    };

}

#endif

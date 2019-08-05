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

#ifndef COLDSPOT_JVM_CLASSFILE_CLASSFILE_HPP_
#define COLDSPOT_JVM_CLASSFILE_CLASSFILE_HPP_

#include <cstdint>

#include <jvm/common/SmartArray.hpp>

namespace coldspot
{

    class AttributeInfo;
    class ConstantPoolEntry;
    class FieldInfo;
    class MethodInfo;

    class ClassFile
    {
    public:

        uint16_t accessFlags;
        uint16_t thisClass;
        uint16_t superClass;
        SmartArray<ConstantPoolEntry *, uint16_t> constantPool;
        SmartArray <uint16_t, uint16_t> interfaces;
        SmartArray<FieldInfo *, uint16_t> fields;
        SmartArray<MethodInfo *, uint16_t> methods;
        SmartArray<AttributeInfo *, uint16_t> attributes;

        ~ClassFile();
    };

}

#endif

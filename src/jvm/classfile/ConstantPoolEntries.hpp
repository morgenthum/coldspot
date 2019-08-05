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

#ifndef COLDSPOT_JVM_CLASSFILE_CONSTANTPOOLENTRIES_HPP_
#define COLDSPOT_JVM_CLASSFILE_CONSTANTPOOLENTRIES_HPP_

#include <cstdint>

#include <jvm/common/Memory.hpp>

namespace coldspot
{

    const uint8_t CP_CLASS = 7;
    const uint8_t CP_FIELDREF = 9;
    const uint8_t CP_METHODREF = 10;
    const uint8_t CP_INTERFACEMETHODREF = 11;
    const uint8_t CP_STRING = 8;
    const uint8_t CP_INTEGER = 3;
    const uint8_t CP_FLOAT = 4;
    const uint8_t CP_LONG = 5;
    const uint8_t CP_DOUBLE = 6;
    const uint8_t CP_NAMEANDTYPE = 12;
    const uint8_t CP_UTF8 = 1;
    const uint8_t CP_METHODHANDLE = 15;
    const uint8_t CP_METHODTYPE = 16;
    const uint8_t CP_INVOKEDYNAMIC = 18;

    class ConstantPoolEntry
    {
    public:
        uint8_t tag;

        virtual ~ConstantPoolEntry()
        {
        }
    };


    class ClassInfoEntry : public ConstantPoolEntry
    {
    public:
        uint16_t nameIndex;
    };


    class FieldrefInfoEntry : public ConstantPoolEntry
    {
    public:
        uint16_t classIndex;
        uint16_t nameAndTypeIndex;
    };


    class MethodrefInfoEntry : public ConstantPoolEntry
    {
    public:
        uint16_t classIndex;
        uint16_t nameAndTypeIndex;
    };


    class InterfaceMethodrefInfoEntry : public ConstantPoolEntry
    {
    public:
        uint16_t classIndex;
        uint16_t nameAndTypeIndex;
    };


    class StringInfoEntry : public ConstantPoolEntry
    {
    public:
        uint16_t stringIndex;
    };


    class IntegerInfoEntry : public ConstantPoolEntry
    {
    public:
        uint32_t bytes;
    };


    class FloatInfoEntry : public ConstantPoolEntry
    {
    public:
        uint32_t bytes;
    };


    class LongInfoEntry : public ConstantPoolEntry
    {
    public:
        uint32_t highBytes;
        uint32_t lowBytes;
    };


    class DoubleInfoEntry : public ConstantPoolEntry
    {
    public:
        uint32_t highBytes;
        uint32_t lowBytes;
    };


    class NameAndTypeInfoEntry : public ConstantPoolEntry
    {
    public:
        uint16_t nameIndex;
        uint16_t descriptorIndex;
    };


    class Utf8InfoEntry : public ConstantPoolEntry
    {
    public:
        uint16_t length;
        uint8_t *bytes;

        Utf8InfoEntry() : length(0), bytes(0)
        {
        }

        ~Utf8InfoEntry()
        {
            DELETE_OBJECT(bytes);
        }
    };


    class MethodHandleInfoEntry : public ConstantPoolEntry
    {
    public:
        uint8_t referenceKind;
        uint16_t referenceIndex;
    };


    class MethodTypeInfoEntry : public ConstantPoolEntry
    {
    public:
        uint16_t descriptorIndex;
    };


    class InvokeDynamicInfoEntry : public ConstantPoolEntry
    {
    public:
        uint16_t bootstrapMethodAttributeIndex;
        uint16_t nameAndTypeIndex;
    };

}

#endif

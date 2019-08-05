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

#ifndef COLDSPOT_JVM_CLASS_RUNTIMECONSTANTPOOLENTRIES_HPP_
#define COLDSPOT_JVM_CLASS_RUNTIMECONSTANTPOOLENTRIES_HPP_

#include <jvm/jdk/Global.hpp>
#include <jvm/common/String.hpp>

namespace coldspot
{

    class Object;
    class Class;
    class Field;
    class Method;

    class RunTimeConstantPoolEntry
    {
    public:

        virtual ~RunTimeConstantPoolEntry()
        {
        }
    };


    class RunTimeClassInfoEntry : public RunTimeConstantPoolEntry
    {
    public:

        Class *clazz;
    };


    class RunTimeFieldrefInfoEntry : public RunTimeConstantPoolEntry
    {
    public:

        Field *field;
    };


    class RunTimeMethodrefInfoEntry : public RunTimeConstantPoolEntry
    {
    public:

        Method *method;
    };


    class RunTimeInterfaceMethodrefInfoEntry : public RunTimeConstantPoolEntry
    {
    public:

        Method *interfaceMethod;
    };


    class RunTimeStringInfoEntry : public RunTimeConstantPoolEntry
    {
    public:

        Object *stringObject;
    };


    class RunTimeIntegerInfoEntry : public RunTimeConstantPoolEntry
    {
    public:

        jint value;
    };


    class RunTimeFloatInfoEntry : public RunTimeConstantPoolEntry
    {
    public:

        jfloat value;
    };


    class RunTimeLongInfoEntry : public RunTimeConstantPoolEntry
    {
    public:

        jlong value;
    };


    class RunTimeDoubleInfoEntry : public RunTimeConstantPoolEntry
    {
    public:

        jdouble value;
    };


    class RunTimeNameAndTypeInfoEntry : public RunTimeConstantPoolEntry
    {
    public:

        String name;
        String descriptor;
    };


    class RunTimeUtf8InfoEntry : public RunTimeConstantPoolEntry
    {
    public:

        String value;
    };


    class RunTimeMethodHandleInfoEntry : public RunTimeConstantPoolEntry
    {
    public:
        // TODO
    };


    class RunTimeMethodTypeInfoEntry : public RunTimeConstantPoolEntry
    {
    public:
        // TODO
    };


    class RunTimeInvokeDynamicInfoEntry : public RunTimeConstantPoolEntry
    {
    public:
        // TODO
    };

}

#endif

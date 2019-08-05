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

#ifndef COLDSPOT_JVM_JNI_TYPES_HPP
#define COLDSPOT_JVM_JNI_TYPES_HPP

#include <cstdint>

namespace coldspot
{
    class Array;
    class Field;
    class Method;
    class Object;
}

using jboolean = uint8_t;
using jbyte = int8_t;
using jchar = char16_t;
using jshort = int16_t;
using jint = int32_t;
using jlong = int64_t;
using jfloat = float;
using jdouble = double;
using jsize = jint;

using jobject = coldspot::Object *;
using jclass = jobject;
using jstring = jobject;
using jthrowable = jobject;
using jweak = jobject;

using jarray = coldspot::Array *;
using jobjectArray = jarray;
using jbyteArray = jarray;
using jshortArray = jarray;
using jintArray = jarray;
using jlongArray = jarray;
using jbooleanArray = jarray;
using jcharArray = jarray;
using jfloatArray = jarray;
using jdoubleArray = jarray;

using jfieldID = coldspot::Field *;
using jmethodID = coldspot::Method *;

typedef union
{
    jboolean z;
    jbyte b;
    jchar c;
    jshort s;
    jint i;
    jlong j;
    jfloat f;
    jdouble d;
    jobject l;
} jvalue;

typedef enum
{
    JNIInvalidRefType = 0,
    JNILocalRefType = 1,
    JNIGlobalRefType = 2,
    JNIWeakGlobalRefType = 3
} jobjectRefType;

#endif

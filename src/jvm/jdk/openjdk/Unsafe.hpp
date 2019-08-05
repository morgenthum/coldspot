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

#ifndef COLDSPOT_JDK_OPENJDK_UNSAFE_HPP_
#define COLDSPOT_JDK_OPENJDK_UNSAFE_HPP_

#include <jvm/jdk/Global.hpp>
#include <jvm/Object.hpp>

void Unsafe_registerNatives(JNIEnv *env, jclass clazz);

jobject Unsafe_getObject_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset);
jboolean Unsafe_getBoolean_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset);
jbyte Unsafe_getByte_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset);
jshort Unsafe_getShort_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset);
jchar Unsafe_getChar_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset);
jint Unsafe_getInt_object(JNIEnv *env, jobject self, jobject obj, jlong offset);
jlong Unsafe_getLong_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset);
jfloat Unsafe_getFloat_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset);
jdouble Unsafe_getDouble_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset);

void Unsafe_putObject_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset, jobject value);
void Unsafe_putBoolean_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset, jboolean value);
void Unsafe_putByte_object(JNIEnv *env, jobject self, jobject obj, jlong offset,
    jbyte value);
void Unsafe_putShort_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset, jshort value);
void Unsafe_putChar_object(JNIEnv *env, jobject self, jobject obj, jlong offset,
    jchar value);
void Unsafe_putInt_object(JNIEnv *env, jobject self, jobject obj, jlong offset,
    jint value);
void Unsafe_putLong_object(JNIEnv *env, jobject self, jobject obj, jlong offset,
    jlong value);
void Unsafe_putFloat_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset, jfloat value);
void Unsafe_putDouble_object(JNIEnv *env, jobject self, jobject obj,
    jlong offset, jdouble value);

jbyte Unsafe_getByte_address(JNIEnv *env, jobject self, jlong address);
jshort Unsafe_getShort_address(JNIEnv *env, jobject self, jlong address);
jchar Unsafe_getChar_address(JNIEnv *env, jobject self, jlong address);
jint Unsafe_getInt_address(JNIEnv *env, jobject self, jlong address);
jlong Unsafe_getLong_address(JNIEnv *env, jobject self, jlong address);
jfloat Unsafe_getFloat_address(JNIEnv *env, jobject self, jlong address);
jdouble Unsafe_getDouble_address(JNIEnv *env, jobject self, jlong address);
jlong Unsafe_getAddress_address(JNIEnv *env, jobject self, jlong address);

void Unsafe_putByte_address(JNIEnv *env, jobject self, jlong address,
    jbyte value);
void Unsafe_putShort_address(JNIEnv *env, jobject self, jlong address,
    jshort value);
void Unsafe_putChar_address(JNIEnv *env, jobject self, jlong address,
    jchar value);
void Unsafe_putInt_address(JNIEnv *env, jobject self, jlong address,
    jint value);
void Unsafe_putLong_address(JNIEnv *env, jobject self, jlong address,
    jlong value);
void Unsafe_putFloat_address(JNIEnv *env, jobject self, jlong address,
    jfloat value);
void Unsafe_putDouble_address(JNIEnv *env, jobject self, jlong address,
    jdouble value);
void Unsafe_putAddress_address(JNIEnv *env, jobject self, jlong address,
    jlong value);

jint Unsafe_addressSize(JNIEnv *env, jobject self);

jlong Unsafe_allocateMemory(JNIEnv *env, jobject self, jlong size);
jlong Unsafe_reallocateMemory(JNIEnv *env, jobject self, jlong address,
    jlong size);
void Unsafe_freeMemory(JNIEnv *env, jobject self, jlong address);
void Unsafe_setMemory(JNIEnv *env, jobject self, jobject obj, jlong offset,
    jlong bytes, jbyte value);

jint Unsafe_arrayIndexScale(JNIEnv *env, jobject self, jclass class_object);
jint Unsafe_arrayBaseOffset(JNIEnv *env, jobject self, jclass classObj);

jobject Unsafe_staticFieldBase(JNIEnv *env, jobject self, jobject fieldObj);
jlong Unsafe_staticFieldOffset(JNIEnv *env, jobject self, jobject fieldObj);

jobject Unsafe_allocateInstance(JNIEnv *env, jobject self, jclass class_obj);
jlong Unsafe_objectFieldOffset(JNIEnv *env, jobject self, jobject fieldObj);

void Unsafe_ensureClassInitialized(JNIEnv *env, jobject self, jclass classObj);

void Unsafe_monitorEnter(JNIEnv *env, jobject self, jobject obj);
void Unsafe_monitorExit(JNIEnv *env, jobject self, jobject obj);
jboolean Unsafe_tryMonitorEnter(JNIEnv *env, jobject self, jobject obj);

void Unsafe_throwException(JNIEnv *env, jobject self, jthrowable exception);

jboolean Unsafe_compareAndSwapObject(JNIEnv *env, jobject self, jobject obj,
    jlong offset, jobject expected, jobject x);
jboolean Unsafe_compareAndSwapInt(JNIEnv *env, jobject self, jobject obj,
    jlong offset, jint expected, jint x);
jboolean Unsafe_compareAndSwapLong(JNIEnv *env, jobject self, jobject obj,
    jlong offset, jlong expected, jlong x);

#endif
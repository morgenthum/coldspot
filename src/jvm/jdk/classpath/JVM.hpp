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

#ifndef COLDSPOT_JDK_CLASSPATH_JVM_HPP_
#define COLDSPOT_JDK_CLASSPATH_JVM_HPP_

// gnu.classpath.VMStackWalker
jobjectArray VMStackWalker_getClassContext(JNIEnv *env, jclass clazz);

jobject VMStackWalker_getClassLoader(JNIEnv *env, jclass clazz,
    jclass checkClass);

// gnu.classpath.VMSystemProperties
void VMSystemProperties_preInit(JNIEnv *env, jclass clazz, jobject properties);

// java.lang.reflect.VMConstructor
jobject VMConstructor_construct(JNIEnv *env, jobject self, jobjectArray args);

jint VMConstructor_getModifiersInternal(JNIEnv *env, jobject self);

jarray VMConstructor_getParameterTypes(JNIEnv *env, jobject self);

// java.lang.VMClass
jobject VMClass_getClassLoader(JNIEnv *env, jclass clazz, jclass callingClazz);

jobject VMClass_getDeclaredConstructors(JNIEnv *env, jclass clazz,
    jclass checkClazz, jboolean publicOnly);

jobject VMClass_forName(JNIEnv *env, jclass clazz, jstring name, jboolean init,
    jobject loader);

jint VMClass_getModifiers(JNIEnv *env, jclass clazz, jclass checkClazz,
    jboolean ignoreInnerClassesAttrib);

jstring VMClass_getName(JNIEnv *env, jclass clazz, jclass checkClazz);

jboolean VMClass_isAssignableFrom(JNIEnv *env, jclass clazz,
    jclass callingClass, jclass checkClass);

jboolean VMClass_isInstance(JNIEnv *env, jclass clazz, jclass callingClass,
    jobject object);

jboolean VMClass_isInterface(JNIEnv *env, jclass clazz, jclass checkClazz);

jboolean VMClass_isPrimitive(JNIEnv *env, jclass clazz, jclass checkClazz);

// java.lang.VMClassLoader
jclass VMClassLoader_defineClass(JNIEnv *env, jclass clazz, jclass classLoader,
    jstring name, jbyteArray data, jint offset, jint len,
    jobject protectionDomain);

jclass VMClassLoader_findLoadedClass(JNIEnv *env, jclass clazz,
    jobject classLoader, jstring name);

jclass VMClassLoader_getPrimitiveClass(JNIEnv *env, jclass clazz, jchar type);

jclass VMClassLoader_loadClass(JNIEnv *env, jclass clazz, jstring name,
    jboolean resolve);

// java.lang.VMObject
jobject VMObject_clone(JNIEnv *env, jclass clazz, jobject cloneable);

jobject VMObject_getClass(JNIEnv *env, jclass clazz, jobject object);

void VMObject_notify(JNIEnv *env, jclass clazz, jobject object);

void VMObject_notifyAll(JNIEnv *env, jclass clazz, jobject object);

void VMObject_wait(JNIEnv *env, jclass clazz, jobject object, jlong ms,
    jint ns);

// java.lang.VMRuntime
jstring VMRuntime_mapLibraryName(JNIEnv *env, jclass clazz, jstring libname);

jint VMRuntime_nativeLoad(JNIEnv *env, jclass clazz, jstring filename,
    jobject classLoader);

// java.lang.VMSystem
void VMSystem_arraycopy(JNIEnv *env, jclass clazz, jobjectArray src,
    jint srcStart, jobjectArray dest, jint destStart, jint length);

jlong VMSystem_currentTimeMillis(JNIEnv *env, jclass clazz);

jint VMSystem_identityHashCode(JNIEnv *env, jclass clazz, jobject object);

jlong VMSystem_nanoTime(JNIEnv *env, jclass clazz);

// java.lang.VMThread
jobject VMThread_currentThread(JNIEnv *env, jclass clazz);

void VMThread_start(JNIEnv *env, jobject self, jlong stacksize);

// java.lang.VMThrowable
jobject VMThrowable_fillInStackTrace(JNIEnv *env, jclass clazz,
    jobject throwable);

jobject VMThrowable_getStackTrace(JNIEnv *env, jobject self, jobject throwable);

#endif

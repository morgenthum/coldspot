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

#include "Templates.hpp"

using namespace coldspot;

jint JNICALL
JNIGetVersion(JNIEnv
*env)
{
return
JNI_VERSION_1_6;
}


jclass JNICALL
DefineClass(JNIEnv
*env,
const char *name, jobject
loader,
const jbyte *buf, jsize
len)
{
jclass clazz = 0;

THREAD_BLOCK
{
Class *definedClass;
ByteClassFileInputStream inputStream((uint8_t *)
buf);
error_t errorValue = _vm->class_loader()->define_class(name, loader,
    &inputStream, &definedClass);
if (errorValue == RETURN_OK)
{
clazz = definedClass->object;
}}
THREAD_UNBLOCK

return
clazz;
}


jclass JNICALL
FindClass(JNIEnv
*env,
const char *name
)
{
jclass javaClass = 0;

THREAD_BLOCK
if (name == 0)
{
_current_executor->
throw_exception(CLASSNAME_NULLPOINTEREXCEPTION,
"FindClass failed => name is 0");
}
else
{
Class *clazz = 0;
error_t errorValue = _vm->class_loader()->load_class(name, &clazz);
if (errorValue == RETURN_OK)
{
javaClass = _vm->add_local_ref(clazz->object);
}}
THREAD_UNBLOCK

return
javaClass;
}


jmethodID JNICALL
FromReflectedMethod(JNIEnv
*env,
jobject method
)
{
EXIT_FATAL("unimplemented JNI-method 'FromReflectedMethod'");
return 0;
}


jfieldID JNICALL
FromReflectedField(JNIEnv
*env,
jobject field
)
{

EXIT_FATAL("unimplemented JNI-method 'FromReflectedField'");
return 0;
}


jobject JNICALL
ToReflectedMethod(JNIEnv
*env,
jclass cls, jmethodID
methodID,
jboolean isStatic
)
{
EXIT_FATAL("unimplemented JNI-method 'ToReflectedMethod'");
return 0;
}


jclass JNICALL
GetSuperclass(JNIEnv
*env,
jclass object
)
{
Class *clazz = Class::from_class_object(object);
if (clazz != 0)
{
Class *super_class = clazz->super_class;
if (super_class != 0)
{
return env->
NewLocalRef(super_class
->object);
}}

return 0;
}


jboolean JNICALL
IsAssignableFrom(JNIEnv
*env,
jclass subObject, jclass
superObject)
{
Class *sub_class = Class::from_class_object(subObject);
if (sub_class != 0)
{
Class *super_class = Class::from_class_object(superObject);
if (super_class != 0)
{
return sub_class->
is_castable_to(super_class);
}}

return false;
}


jobject JNICALL
ToReflectedField(JNIEnv
*env,
jclass cls, jfieldID
fieldID,
jboolean isStatic
)
{
EXIT_FATAL("unimplemented jni-method 'ToReflectedField'");
return 0;
}


jint JNICALL
Throw(JNIEnv
*env,
jthrowable exception
)
{
if (exception == 0)
{
env->
ThrowNew(_vm
->builtin.nullPointerExceptionClass->object,
"'Throw' failed");
return
JNI_ERR;
}

auto &frames = _current_executor->frames();
if (!frames.
empty()
)
{
Frame *topFrame = (Frame *) frames.peek();
topFrame->
exception = exception;
return
JNI_OK;
}
else
{
return
JNI_ERR;
}}


jint JNICALL
ThrowNew(JNIEnv
*env,
jclass object,
const char *msg
)
{
Class *clazz = Class::from_class_object(object);
if (clazz == 0)
{
return -1;
}

Object *throwable;
error_t errorValue = java_lang_Throwable::new_(clazz, String(msg), &throwable);
if (errorValue != RETURN_OK)
{
return -1;
}

return
Throw(env, throwable
);
}


jthrowable JNICALL
ExceptionOccurred(JNIEnv
*env)
{
return env->
NewLocalRef(_current_executor
->
occured_exception()
);
}


void JNICALL
ExceptionDescribe(JNIEnv
*env)
{
jthrowable exception = env->ExceptionOccurred();
if (exception != 0)
{
THREAD_BLOCK _current_executor
->
describe_exception(exception);
THREAD_UNBLOCK
}}


void JNICALL
ExceptionClear(JNIEnv
*env)
{
THREAD_BLOCK _current_executor
->
clear_exception();
THREAD_UNBLOCK
}


void JNICALL
FatalError(JNIEnv
*env,
const char *msg
)
{
EXIT_FATAL(msg);
}


jint JNICALL
PushLocalFrame(JNIEnv
*env,
jint capacity
)
{
EXIT_FATAL("unimplemented jni-method 'PushLocalFrame'");
return 0;  // TODO implement
}


jobject JNICALL
PopLocalFrame(JNIEnv
*env,
jobject result
)
{
EXIT_FATAL("unimplemented jni-method 'PopLocalFrame'");
return 0;  // TODO implement
}


jobject JNICALL
NewGlobalRef(JNIEnv
*env,
jobject obj
)
{
THREAD_BLOCK jobject
ref = _vm->add_global_ref(obj);
THREAD_UNBLOCK
return
ref;
}


void JNICALL
DeleteGlobalRef(JNIEnv
*env,
jobject obj
)
{
THREAD_BLOCK _vm
->
remove_global_ref(obj);
THREAD_UNBLOCK
}


void JNICALL
DeleteLocalRef(JNIEnv
*env,
jobject obj
)
{
THREAD_BLOCK _vm
->
remove_local_ref(obj);
THREAD_UNBLOCK
}


jboolean JNICALL
IsSameObject(JNIEnv
*env,
jobject obj1, jobject
obj2)
{
return obj1 ==
obj2;
}


jobject JNICALL
NewLocalRef(JNIEnv
*env,
jobject obj
)
{
THREAD_BLOCK jobject
ref = _vm->add_local_ref(obj);
THREAD_UNBLOCK
return
ref;
}


jint JNICALL
EnsureLocalCapacity(JNIEnv
*env,
jint capacity
)
{
return 0;
}


jobject JNICALL
AllocObject(JNIEnv
*env,
jclass classObj
)
{
jobject obj = 0;

THREAD_BLOCK Class
*
clazz = Class::from_class_object(classObj);
if (clazz != 0)
{
Object *object;
error_t errorValue = _vm->memory_manager()->allocate_object(clazz, &object);
if (errorValue == RETURN_OK)
{
obj = env->NewLocalRef(object);
}}
THREAD_UNBLOCK

return
obj;
}


jobject JNICALL
NewObject(JNIEnv
*env,
jclass clazz, jmethodID
methodID, ...)
{
jobject object = env->AllocObject(clazz);

va_list args;
va_start(args, methodID);
call_virtual(object, methodID, args
);
va_end(args);

return
object;
}


jobject JNICALL
NewObjectV(JNIEnv
*env,
jclass clazz, jmethodID
methodID,
va_list args
)
{
jobject object = env->AllocObject(clazz);
call_virtual(object, methodID, args
);

return
object;
}


jobject JNICALL
NewObjectA(JNIEnv
*env,
jclass clazz, jmethodID
methodID,
const jvalue *args
)
{
jobject object = env->AllocObject(clazz);
call_virtual(object, methodID, args
);

return
object;
}


jclass JNICALL
GetObjectClass(JNIEnv
*env,
jobject obj
)
{
if (obj != 0)
{
return env->
NewLocalRef(obj
->
type() -> object
);
}

return 0;
}


jboolean JNICALL
IsInstanceOf(JNIEnv
*env,
jobject obj, jclass
classObj)
{
if (obj != 0)
{
Class *clazz = Class::from_class_object(classObj);
if (clazz != 0)
{
return obj->
type() ->
is_castable_to(clazz);
}}

return
JNI_FALSE;
}


jmethodID JNICALL
GetMethodID(JNIEnv
*env,
jclass classObj,
const char *name,
const char *sig
)
{
jmethodID methodID = 0;

Class *clazz = Class::from_class_object(classObj);
if (clazz != 0)
{
THREAD_BLOCK
{
Method *method;
if (clazz->
get_method(Signature(sig, name), &method
) == RETURN_OK)
{
methodID = method;
}}
THREAD_UNBLOCK
}

return
methodID;
}


jobject JNICALL
CallObjectMethod(JNIEnv
*env,
jobject obj, jmethodID
methodID,
...)
{
va_list args;
va_start(args, methodID);
Value value = call_virtual(obj, methodID, args);
va_end(args);

return env->
NewLocalRef(value
.
as_object()
);
}


jobject JNICALL
CallObjectMethodV(JNIEnv
*env,
jobject obj, jmethodID
methodID,
va_list args
)
{
Object *object = call_virtual(obj, methodID, args).as_object();
return env->
NewLocalRef(object);
}


jobject JNICALL
CallObjectMethodA(JNIEnv
*env,
jobject obj, jmethodID
methodID,
const jvalue *args
)
{
Object *object = call_virtual(obj, methodID, args).as_object();
return env->
NewLocalRef(object);
}


CALL_TYPE_METHOD(Boolean, jboolean, as_boolean)
CALL_TYPE_METHOD(Byte, jbyte, as_byte)
CALL_TYPE_METHOD(Char, jchar, as_char)
CALL_TYPE_METHOD(Short, jshort, as_short)
CALL_TYPE_METHOD(Int, jint, as_int)
CALL_TYPE_METHOD(Long, jlong, as_long)
CALL_TYPE_METHOD(Float, jfloat, as_float)
CALL_TYPE_METHOD(Double, jdouble, as_double)


void JNICALL
CallVoidMethod(JNIEnv
*env,
jobject obj, jmethodID
methodID, ...)
{
va_list args;
va_start(args, methodID);
call_virtual(obj, methodID, args
);
va_end(args);
}


void JNICALL
CallVoidMethodV(JNIEnv
*env,
jobject obj, jmethodID
methodID,
va_list args
)
{
call_virtual(obj, methodID, args
);
}


void JNICALL
CallVoidMethodA(JNIEnv
*env,
jobject obj, jmethodID
methodID,
const jvalue *args
)
{
call_virtual(obj, methodID, args
);
}


jobject JNICALL
CallNonvirtualObjectMethod(JNIEnv
*env,
jobject obj, jclass
clazz,
jmethodID methodID, ...
)
{
va_list args;
va_start(args, methodID);
Value value = call_nonvirtual(obj, methodID, args);
va_end(args);

return env->
NewLocalRef(value
.
as_object()
);
}


jobject JNICALL
CallNonvirtualObjectMethodV(JNIEnv
*env,
jobject obj, jclass
clazz,
jmethodID methodID, va_list
args)
{
Object *object = call_nonvirtual(obj, methodID, args).as_object();
return env->
NewLocalRef(object);
}


jobject JNICALL
CallNonvirtualObjectMethodA(JNIEnv
*env,
jobject obj, jclass
clazz,
jmethodID methodID,
const jvalue *args
)
{
Object *object = call_nonvirtual(obj, methodID, args).as_object();
return env->
NewLocalRef(object);
}


CALL_NONVIRTUAL_TYPE_METHOD(Boolean, jboolean, as_boolean)
CALL_NONVIRTUAL_TYPE_METHOD(Byte, jbyte, as_byte)
CALL_NONVIRTUAL_TYPE_METHOD(Char, jchar, as_char)
CALL_NONVIRTUAL_TYPE_METHOD(Short, jshort, as_short)
CALL_NONVIRTUAL_TYPE_METHOD(Int, jint, as_int)
CALL_NONVIRTUAL_TYPE_METHOD(Long, jlong, as_long)
CALL_NONVIRTUAL_TYPE_METHOD(Float, jfloat, as_float)
CALL_NONVIRTUAL_TYPE_METHOD(Double, jdouble, as_double)


void JNICALL
CallNonvirtualVoidMethod(JNIEnv
*env,
jobject obj, jclass
clazz,
jmethodID methodID, ...
)
{
va_list args;
va_start(args, methodID);
call_nonvirtual(obj, methodID, args
);
va_end(args);
}


void JNICALL
CallNonvirtualVoidMethodV(JNIEnv
*env,
jobject obj, jclass
clazz,
jmethodID methodID, va_list
args)
{
call_nonvirtual(obj, methodID, args
);
}


void JNICALL
CallNonvirtualVoidMethodA(JNIEnv
*env,
jobject obj, jclass
clazz,
jmethodID methodID,
const jvalue *args
)
{
call_nonvirtual(obj, methodID, args
);
}


jfieldID JNICALL
GetFieldID(JNIEnv
*env,
jclass classObj,
const char *name,
const char *sig
)
{
jfieldID field;

Class *clazz = Class::from_class_object(classObj);
if (clazz != 0)
{
THREAD_BLOCK
{
clazz->
get_field(Signature(sig, name), &field
);
}
THREAD_UNBLOCK
}

return
field;
}


jobject JNICALL
GetObjectField(JNIEnv
*env,
jobject obj, jfieldID
fieldID)
{
return env->
NewLocalRef(fieldID
->
get<Object *>(obj)
);
}


GET_TYPE_FIELD(Boolean, jboolean)
GET_TYPE_FIELD(Byte, jbyte)
GET_TYPE_FIELD(Char, jchar)
GET_TYPE_FIELD(Short, jshort)
GET_TYPE_FIELD(Int, jint)
GET_TYPE_FIELD(Long, jlong)
GET_TYPE_FIELD(Float, jfloat)
GET_TYPE_FIELD(Double, jdouble)


SET_TYPE_FIELD(Object, jobject)
SET_TYPE_FIELD(Boolean, jboolean)
SET_TYPE_FIELD(Byte, jbyte)
SET_TYPE_FIELD(Char, jchar)
SET_TYPE_FIELD(Short, jshort)
SET_TYPE_FIELD(Int, jint)
SET_TYPE_FIELD(Long, jlong)
SET_TYPE_FIELD(Float, jfloat)
SET_TYPE_FIELD(Double, jdouble)


jmethodID JNICALL
GetStaticMethodID(JNIEnv
*env,
jclass classObj,
const char *name,
const char *sig
)
{
jmethodID method = 0;

Class *clazz = Class::from_class_object(classObj);
if (clazz != 0)
{
THREAD_BLOCK clazz
->
get_method(Signature(sig, name), &method
);
THREAD_UNBLOCK
}

return
method;
}


jobject JNICALL
CallStaticObjectMethod(JNIEnv
*env,
jclass clazz, jmethodID
methodID, ...)
{
va_list args;
va_start(args, methodID);
Value value = call_static(methodID, args);
va_end(args);

return env->
NewLocalRef(value
.
as_object()
);
}


jobject JNICALL
CallStaticObjectMethodV(JNIEnv
*env,
jclass clazz, jmethodID
methodID,
va_list args
)
{
Object *internObject = call_static(methodID, args).as_object();
return env->
NewLocalRef(internObject);
}


jobject JNICALL
CallStaticObjectMethodA(JNIEnv
*env,
jclass clazz, jmethodID
methodID,
const jvalue *args
)
{
Object *internObject = call_static(methodID, args).as_object();
return env->
NewLocalRef(internObject);
}


CALL_STATIC_TYPE_METHOD(Boolean, jboolean, as_boolean)
CALL_STATIC_TYPE_METHOD(Byte, jbyte, as_byte)
CALL_STATIC_TYPE_METHOD(Char, jchar, as_char)
CALL_STATIC_TYPE_METHOD(Short, jshort, as_short)
CALL_STATIC_TYPE_METHOD(Int, jint, as_int)
CALL_STATIC_TYPE_METHOD(Long, jlong, as_long)
CALL_STATIC_TYPE_METHOD(Float, jfloat, as_float)
CALL_STATIC_TYPE_METHOD(Double, jdouble, as_double)


void JNICALL
CallStaticVoidMethod(JNIEnv
*env,
jclass cls, jmethodID
methodID,
...)
{
va_list args;
va_start(args, methodID);
call_static(methodID, args
);
va_end(args);
}


void JNICALL
CallStaticVoidMethodV(JNIEnv
*env,
jclass cls, jmethodID
methodID,
va_list args
)
{
call_static(methodID, args
);
}


void JNICALL
CallStaticVoidMethodA(JNIEnv
*env,
jclass cls, jmethodID
methodID,
const jvalue *args
)
{
call_static(methodID, args
);
}


jfieldID JNICALL
GetStaticFieldID(JNIEnv
*env,
jclass classObj,
const char *name,
const char *sig
)
{
jfieldID field = 0;

Class *clazz = Class::from_class_object(classObj);
if (clazz != 0)
{
THREAD_BLOCK clazz
->
get_field(Signature(sig, name), &field
);
THREAD_UNBLOCK
}

return
field;
}


jobject JNICALL
GetStaticObjectField(JNIEnv
*env,
jclass clazz, jfieldID
fieldID)
{
return env->
NewLocalRef(fieldID
->
get_static<jobject>()
);
}


GET_STATIC_TYPE_FIELD(Boolean, jboolean, as_boolean)
GET_STATIC_TYPE_FIELD(Byte, jbyte, as_boolean)
GET_STATIC_TYPE_FIELD(Char, jchar, as_char)
GET_STATIC_TYPE_FIELD(Short, jshort, as_short)
GET_STATIC_TYPE_FIELD(Int, jint, as_int)
GET_STATIC_TYPE_FIELD(Long, jlong, as_long)
GET_STATIC_TYPE_FIELD(Float, jfloat, as_float)
GET_STATIC_TYPE_FIELD(Double, jdouble, as_double)


SET_STATIC_TYPE_FIELD(Object, jobject)
SET_STATIC_TYPE_FIELD(Boolean, jboolean)
SET_STATIC_TYPE_FIELD(Byte, jbyte)
SET_STATIC_TYPE_FIELD(Char, jchar)
SET_STATIC_TYPE_FIELD(Short, jshort)
SET_STATIC_TYPE_FIELD(Int, jint)
SET_STATIC_TYPE_FIELD(Long, jlong)
SET_STATIC_TYPE_FIELD(Float, jfloat)
SET_STATIC_TYPE_FIELD(Double, jdouble)


jstring JNICALL
NewString(JNIEnv
*env,
const jchar *unicode, jsize
len)
{
jstring string = 0;

UTF16String chars(unicode, len);

THREAD_BLOCK
    java_lang_String::new_(chars, &string);
THREAD_UNBLOCK

return
string;
}


jsize JNICALL
GetStringLength(JNIEnv
*env,
jstring str
)
{
jint length = 0;

THREAD_BLOCK
    java_lang_String::length(str, &length);
THREAD_UNBLOCK

return
length;
}


const jchar *JNICALL
GetStringChars(JNIEnv
*env,
jstring str, jboolean
*isCopy)
{
jclass stringClass = _vm->builtin.stringClass->object;
if (stringClass != 0)
{
jmethodID method = GetMethodID(env, stringClass, "toCharArray", "()[C");
if (method != 0)
{
jcharArray charArray = static_cast<jcharArray>(CallObjectMethod(env, str,
    method));
if (charArray != 0)
{
jsize length = GetArrayLength(env, charArray);

jchar *stringChars = new jchar[length + 1];
GetCharArrayRegion(env, charArray,
0, length, stringChars);
stringChars[length] = 0;

if (isCopy != 0)
{
*
isCopy = true;
}

return
stringChars;
}}}

return 0;
}


void JNICALL
ReleaseStringChars(JNIEnv
*env,
jstring str,
const jchar *chars
)
{
DELETE_ARRAY(chars)
}


jstring JNICALL
NewStringUTF(JNIEnv
*env,
const char *utf8_chars
)
{
UTF16String string = String(utf8_chars);
return
NewString(env, string
.
toCString(), string
.
length()
);
}


jsize JNICALL
GetStringUTFLength(JNIEnv
*env,
jstring str
)
{
const jchar *utf16_chars = GetStringChars(env, str, 0);
UTF16String utf16_string(utf16_chars);
ReleaseStringChars(env, str, utf16_chars
);

return utf16_string.
length();
}


const char *JNICALL
GetStringUTFChars(JNIEnv
*env,
jstring str, jboolean
*isCopy)
{
const jchar *utf16_chars = GetStringChars(env, str, isCopy);
String utf8_string = UTF16String(utf16_chars);
ReleaseStringChars(env, str, utf16_chars
);

char *utf8_chars = new char[utf8_string.length() + 1];
memcpy(utf8_chars, utf8_string
.
c_str(),
sizeof(char) * utf8_string.
length()
);
utf8_chars[utf8_string.
length()
] = 0;

if (isCopy != 0)
{
*
isCopy = true;
}

return
utf8_chars;
}


void JNICALL
ReleaseStringUTFChars(JNIEnv
*env,
jstring str,
const char *chars
)
{
DELETE_ARRAY(chars);
}


jsize JNICALL
GetArrayLength(JNIEnv
*env,
jarray array
)
{
return array == 0 ? 0 : array->
length();
}


jobjectArray JNICALL
NewObjectArray(JNIEnv
*env,
jsize len, jclass
clazzObj,
jobject init
)
{
jobjectArray objectArray = 0;

Class *clazz = Class::from_class_object(clazzObj);
if (clazz != 0)
{
error_t errorValue = Array::new_object_array(clazz, len, &objectArray);
if (errorValue == RETURN_OK)
{
objectArray = (jobjectArray) env->NewLocalRef(objectArray);

for (
jsize i = 0;
i<len;
++i)
{
env->
SetObjectArrayElement(objectArray, i, init
);
}}}

return
objectArray;
}


jobject JNICALL
GetObjectArrayElement(JNIEnv
*env,
jobjectArray array, jsize
index)
{
jobject element = 0;

if (array != 0)
{
THREAD_BLOCK
{
error_t errorValue = array->get_value<Object *>(index, &element);
if (errorValue == RETURN_OK)
{
element = env->NewLocalRef(element);
}}
THREAD_UNBLOCK
}

return
element;
}


void JNICALL
SetObjectArrayElement(JNIEnv
*env,
jobjectArray array, jsize
index,
jobject value
)
{
if (array != 0)
{
array->
set_value(index, value
);
}}


NEW_TYPE_ARRAY(Boolean, jboolean, len)
NEW_TYPE_ARRAY(Byte, jbyte, len)
NEW_TYPE_ARRAY(Char, jchar, len)
NEW_TYPE_ARRAY(Short, jshort, len)
NEW_TYPE_ARRAY(Int, jint, len)
NEW_TYPE_ARRAY(Long, jlong, len)
NEW_TYPE_ARRAY(Float, jfloat, len)
NEW_TYPE_ARRAY(Double, jdouble, len)


GET_TYPE_ARRAY_ELEMENTS(Boolean, jboolean)
GET_TYPE_ARRAY_ELEMENTS(Byte, jbyte)
GET_TYPE_ARRAY_ELEMENTS(Char, jchar)
GET_TYPE_ARRAY_ELEMENTS(Short, jshort)
GET_TYPE_ARRAY_ELEMENTS(Int, jint)
GET_TYPE_ARRAY_ELEMENTS(Long, jlong)
GET_TYPE_ARRAY_ELEMENTS(Float, jfloat)
GET_TYPE_ARRAY_ELEMENTS(Double, jdouble)


RELEASE_TYPE_ARRAY_ELEMENTS(Boolean, jboolean)
RELEASE_TYPE_ARRAY_ELEMENTS(Byte, jbyte)
RELEASE_TYPE_ARRAY_ELEMENTS(Char, jchar)
RELEASE_TYPE_ARRAY_ELEMENTS(Short, jshort)
RELEASE_TYPE_ARRAY_ELEMENTS(Int, jint)
RELEASE_TYPE_ARRAY_ELEMENTS(Long, jlong)
RELEASE_TYPE_ARRAY_ELEMENTS(Float, jfloat)
RELEASE_TYPE_ARRAY_ELEMENTS(Double, jdouble)


GET_TYPE_ARRAY_REGION(Boolean, jboolean)
GET_TYPE_ARRAY_REGION(Byte, jbyte)
GET_TYPE_ARRAY_REGION(Char, jchar)
GET_TYPE_ARRAY_REGION(Short, jshort)
GET_TYPE_ARRAY_REGION(Int, jint)
GET_TYPE_ARRAY_REGION(Long, jlong)
GET_TYPE_ARRAY_REGION(Float, jfloat)
GET_TYPE_ARRAY_REGION(Double, jdouble)


SET_TYPE_ARRAY_REGION(Boolean, jboolean)
SET_TYPE_ARRAY_REGION(Byte, jbyte)
SET_TYPE_ARRAY_REGION(Char, jchar)
SET_TYPE_ARRAY_REGION(Short, jshort)
SET_TYPE_ARRAY_REGION(Int, jint)
SET_TYPE_ARRAY_REGION(Long, jlong)
SET_TYPE_ARRAY_REGION(Float, jfloat)
SET_TYPE_ARRAY_REGION(Double, jdouble)


jint JNICALL
RegisterNatives(JNIEnv
*env,
jclass classObj,
const JNINativeMethod *methods, jint
nMethods)
{
Class *clazz = Class::from_class_object(classObj);
if (clazz == 0)
{
return 0;
}

for (
jint i = 0;
i<nMethods;
++i)
{
JNINativeMethod nativeMethod = methods[i];
if (nativeMethod.fnPtr == 0)
{
continue;
}

Method *method;
error_t errorValue = clazz->get_declared_method(
    Signature(nativeMethod.signature, nativeMethod.name), &method);
if (errorValue != RETURN_OK)
{
StringBuilder builder;
builder << "could not register method '" << nativeMethod.name <<
nativeMethod.signature << "'";
EXIT_FATAL(builder
.
str()
.
c_str()
);
}

NativeCall *nativeCall = new NativeCall(method,
    (Function_t) nativeMethod.fnPtr);
nativeCall->
init();

method->
set_native_call(nativeCall);
}

return 0;
}


jint JNICALL
UnregisterNatives(JNIEnv
*env,
jclass classObj
)
{
Class *clazz = Class::from_class_object(classObj);
if (clazz == 0)
{
return 0;
}

List<Method *> methods;
clazz->
get_declared_methods(methods);

for (
Method *method :
methods)
{
NativeCall *nativeCall = method->native_call();
DELETE_OBJECT(nativeCall)
method->set_native_call(0);
}

return 0;
}


jint JNICALL
MonitorEnter(JNIEnv
*env,
jobject obj
)
{
if (obj != 0)
{
obj->
ensure_monitor() -> enter();
return 0;
}

return -1;
}


jint JNICALL
MonitorExit(JNIEnv
*env,
jobject obj
)
{
if (obj != 0)
{
error_t errorValue = obj->ensure_monitor()->exit();
if (errorValue == RETURN_OK)
{
return
JNI_OK;
}}

return
JNI_ERR;
}


jint JNICALL
GetJavaVM(JNIEnv
*env,
JavaVM **vm
)
{
*
vm = _vm->vm_interface();
return 0;
}


void JNICALL
GetStringRegion(JNIEnv
*env,
jstring str, jsize
start,
jsize len, jchar
*buf)
{
jclass stringClass = GetObjectClass(env, str);
if (stringClass != 0)
{
jmethodID method = env->GetMethodID(stringClass, "toCharArray", "()[C");
if (method != 0)
{
jcharArray charArray = static_cast<jcharArray>(CallObjectMethod(env, str,
    method));
if (charArray != 0)
{
GetCharArrayRegion(env, charArray, start, len, buf
);
}}}}


void JNICALL
GetStringUTFRegion(JNIEnv
*env,
jstring str, jsize
start,
jsize len,
char *buf
)
{
jchar utf16Chars[len];
GetStringRegion(env, str, start, len, utf16Chars
);

UTF16String utf16String(utf16Chars, len);
String utf8String(utf16String);

memcpy(buf, utf8String
.
c_str(),
sizeof(char) * utf8String.
length()
);
buf[utf8String.
length()
] = 0;
}


void *JNICALL
GetPrimitiveArrayCritical(JNIEnv
*env,
jarray array, jboolean
*isCopy)
{
return
GetArrayElements<void *>(array, isCopy
);
}


void JNICALL
ReleasePrimitiveArrayCritical(JNIEnv
*env,
jarray array,
void *carray, jint
mode)
{
}


const jchar *JNICALL
GetStringCritical(JNIEnv
*env,
jstring string, jboolean
*isCopy)
{

return
GetStringChars(env, string, isCopy
);
}


void JNICALL
ReleaseStringCritical(JNIEnv
*env,
jstring string,
const jchar *cstring
)
{
ReleaseStringChars(env, string, cstring
);
}


jweak JNICALL
NewWeakGlobalRef(JNIEnv
*env,
jobject obj
)
{
EXIT_FATAL("unimplemented jni-method 'NewWeakGlobalRef'");
return 0;  // TODO implement
}


void JNICALL
DeleteWeakGlobalRef(JNIEnv
*env,
jweak ref
)
{
EXIT_FATAL("unimplemented jni-method 'DeleteWeakGlobalRef'");
// TODO implement
}


jboolean JNICALL
ExceptionCheck(JNIEnv
*env)
{
auto &frames = _current_executor->frames();

if (frames.
empty()
)
{
return _current_executor->
uncaught_exception()
!= 0;
}
else
{
Frame *topFrame = (Frame *) frames.peek();
return topFrame->exception != 0;
}}


jobject JNICALL
NewDirectByteBuffer(JNIEnv
*env,
void *address, jlong
capacity)
{
EXIT_FATAL("unimplemented jni-method 'NewDirectByteBuffer'");
return 0;  // TODO implement
}


// TODO Move to jdk-specifics
void *JNICALL
GetDirectBufferAddress(JNIEnv
*env,
jobject buffer
)
{
jclass bufferClass = env->GetObjectClass(buffer);
jfieldID addressField = env->GetFieldID(bufferClass, "address",
    "Lgnu/classpath/Pointer;");
jobject address = env->GetObjectField(buffer, addressField);

if (address != 0)
{
jclass addressClassObj = env->GetObjectClass(address);
Class *addressClass = Class::from_class_object(addressClassObj);

#if defined(__i386__)
jfieldID dataField = env->GetFieldID(addressClassObj, "data", "I");
return (void *) env->GetIntField(address, dataField);
#elif defined(__x86_64__)
jfieldID dataField = env->GetFieldID(addressClassObj, "data", "J");
return (void *) env->GetLongField(address, dataField);
#else
    #error "unknown architecture"
#endif
}

return 0;
}


jlong JNICALL
GetDirectBufferCapacity(JNIEnv
*env,
jobject buffer
)
{
jclass bufferClass = env->GetObjectClass(buffer);
jmethodID capacityMethod = env->GetMethodID(bufferClass, "capacity", "()I");

return env->
CallIntMethod(buffer, capacityMethod
);
}


jobjectRefType JNICALL
GetObjectRefType(JNIEnv
*env,
jobject obj
)
{
EXIT_FATAL("unimplemented jni-method 'GetObjectRefType'");

return
JNIInvalidRefType;  // TODO implement
}


jint DestroyJavaVM(JavaVM *javaVm)
{
    VirtualMachine *vm = _vm;
    vm->release();

    DELETE_OBJECT(vm);

    return 0;
}


jint AttachCurrentThread(JavaVM *jvm, void **penv, void *args)
{
    _vm->attach_thread(0, false);
    *penv = _vm->jni_interface();

    return 0;
}


jint DetachCurrentThread(JavaVM *vm)
{
    _vm->detach_thread();

    return 0;
}


jint GetEnv(JavaVM *jvm, void **penv, jint version)
{
    _vm->get_interface(version, penv);

    return 0;
}


jint AttachCurrentThreadAsDaemon(JavaVM *jvm, void **penv, void *args)
{
    _vm->attach_thread(0, true);

    *penv = _vm->jni_interface();

    return 0;
}


jint JNICALL
JNI_GetDefaultJavaVMInitArgs(void *args)
{
    if (args != 0)
    {
        JavaVMInitArgs *initArgs = (JavaVMInitArgs *) args;
        initArgs->nOptions = 0;
    }

    return 0;
}


jint JNICALL
JNI_CreateJavaVM(JavaVM
**pvm,
void **v_penv,
void *v_args
)
{
// Initialize vm options
Options *options = new Options;
JavaVMInitArgs *initArgs = (JavaVMInitArgs *) v_args;

// Iterate options
for (
jint i = 0;
i < 0; ++i)
{
// Current option and skip '-'
char *option = ++initArgs->options[i].optionString;

// Set vm specific options
if (option[0] == 'X')
{
if (strcmp(++option, "verbose:debug") == 0)
{
options->
verboseDebug = true;
}
else if (
strcmp(option,
"verbose:execute") == 0)
{
options->
verboseExecute = true;
}}
// Set system property
else if (option[0] == 'D')
{
char *key = strtok(++option, "=");
char *value = strtok(0, "=");

if (key != 0 && value != 0)
{
options->systemProperties.
put(key, value
);
}}
// Set classpath
else if (
strcmp(option,
"cp") == 0 ||
strcmp(option,
"classpath") == 0)
{
if (++i<initArgs->nOptions)
{
char *classpath = initArgs->options[i].optionString;
options->systemProperties.put("java.class.path", classpath);
}}
// Set verbose
else if (
strcmp(option,
"verbose:class") == 0)
{
options->
verboseClass = true;
}
else if (
strcmp(option,
"verbose:gc") == 0)
{
options->
verboseGC = true;
}
else if (
strcmp(option,
"verbose:jni") == 0)
{
options->
verboseJNI = true;
}
// Unrecognized option
else if (!initArgs->ignoreUnrecognized)
{
LOG_ERROR("unrecognized option: " << --option)
exit(1);
}}

//options->verboseClass = true;
//options->verboseExecute = true;
//options->verboseGC = true;
//options->verboseJNI = true;
//options->verboseDebug = true;

VirtualMachine *vm = new VirtualMachine;

*
pvm = vm->vm_interface();
*
v_penv = vm->jni_interface();

return vm->
initialize(options);
}


jint JNICALL
JNI_GetCreatedJavaVMs(JavaVM
**vmBuf,
jsize bufLen, jsize
*nVMs)
{
*
nVMs = 0;

if (bufLen > 0)
{
vmBuf[0] = _vm->
vm_interface();
*
nVMs = 1;
}

return
JNI_OK;
}

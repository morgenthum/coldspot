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

#include <jvm/Environment.hpp>
#include <jvm/Global.hpp>

#define LOG(msg) LOG_INFO("theUnsafe -> " << msg)

#if defined(JDK_OPENJDK)

using namespace coldspot;

static const uint32_t STATIC_BIT = 0x80000000;

bool static_bit_set(uint32_t offset)
{
  return (offset & STATIC_BIT) != 0;
}


template<typename T>
T get_value(jobject object, jlong offset)
{
  if (static_bit_set(offset))
  {
    return Class::from_class_object(object)->get_value<T>(offset);
  }
  else
  {
    return object->get_value<T>(offset);
  }
}


template<typename T>
void set_value(jobject object, jlong offset, T value)
{
  if (static_bit_set(offset))
  {
    Class::from_class_object(object)->set_value(offset, value);
  }
  else
  {
    object->set_value(offset, value);
  }
}


template<typename T>
jboolean Unsafe_compareAndSwap(jobject obj, jlong offset, T expected, T x)
{
  if (get_value<T>(obj, offset) == expected)
  {
    set_value(obj, offset, x);
  }

  return JNI_TRUE;
}


void Unsafe_registerNatives(JNIEnv *env, jclass clazz) { }


    #define UNSAFE_GETTYPE_OBJECT(name, type) \
  type Unsafe_get##name##_object(JNIEnv *env, jobject self, jobject obj, \
    jlong offset) \
  { \
    return get_value<type>(obj, offset); \
  }

UNSAFE_GETTYPE_OBJECT(Object, jobject)
UNSAFE_GETTYPE_OBJECT(Boolean, jboolean)
UNSAFE_GETTYPE_OBJECT(Byte, jbyte)
UNSAFE_GETTYPE_OBJECT(Short, jshort)
UNSAFE_GETTYPE_OBJECT(Char, jchar)
UNSAFE_GETTYPE_OBJECT(Int, jint)
UNSAFE_GETTYPE_OBJECT(Long, jlong)
UNSAFE_GETTYPE_OBJECT(Float, jfloat)
UNSAFE_GETTYPE_OBJECT(Double, jdouble)


    #define UNSAFE_PUTTYPE_OBJECT(name, type) \
  void Unsafe_put##name##_object(JNIEnv *env, jobject self, jobject obj, \
    jlong offset, type value) \
  { \
    set_value(obj, offset, value); \
  }

UNSAFE_PUTTYPE_OBJECT(Object, jobject)
UNSAFE_PUTTYPE_OBJECT(Boolean, jboolean)
UNSAFE_PUTTYPE_OBJECT(Byte, jbyte)
UNSAFE_PUTTYPE_OBJECT(Short, jshort)
UNSAFE_PUTTYPE_OBJECT(Char, jchar)
UNSAFE_PUTTYPE_OBJECT(Int, jint)
UNSAFE_PUTTYPE_OBJECT(Long, jlong)
UNSAFE_PUTTYPE_OBJECT(Float, jfloat)
UNSAFE_PUTTYPE_OBJECT(Double, jdouble)


    #define UNSAFE_GETTYPE_ADDRESS(name, type) \
  type Unsafe_get##name##_address(JNIEnv *env, jobject self, jlong address) \
  { \
    return *((type*) address); \
  }

UNSAFE_GETTYPE_ADDRESS(Byte, jbyte)
UNSAFE_GETTYPE_ADDRESS(Short, jshort)
UNSAFE_GETTYPE_ADDRESS(Char, jchar)
UNSAFE_GETTYPE_ADDRESS(Int, jint)
UNSAFE_GETTYPE_ADDRESS(Long, jlong)
UNSAFE_GETTYPE_ADDRESS(Float, jfloat)
UNSAFE_GETTYPE_ADDRESS(Double, jdouble)
UNSAFE_GETTYPE_ADDRESS(Address, jlong)


    #define UNSAFE_PUTTYPE_ADDRESS(name, type) \
  void Unsafe_put##name##_address(JNIEnv *env, jobject self, jlong address, type value) \
  { \
    *((type *) address) = value; \
  }

UNSAFE_PUTTYPE_ADDRESS(Byte, jbyte)
UNSAFE_PUTTYPE_ADDRESS(Short, jshort)
UNSAFE_PUTTYPE_ADDRESS(Char, jchar)
UNSAFE_PUTTYPE_ADDRESS(Int, jint)
UNSAFE_PUTTYPE_ADDRESS(Long, jlong)
UNSAFE_PUTTYPE_ADDRESS(Float, jfloat)
UNSAFE_PUTTYPE_ADDRESS(Double, jdouble)
UNSAFE_PUTTYPE_ADDRESS(Address, jlong)


jint Unsafe_addressSize(JNIEnv *env, jobject self)
{
  return sizeof(Object *);
}


jlong Unsafe_allocateMemory(JNIEnv *env, jobject self, jlong size)
{
  return (jlong) malloc(size);
}


jlong Unsafe_reallocateMemory(JNIEnv *env, jobject self, jlong address,
  jlong size)
{
  return (jlong) realloc((void *) address, size);
}


void Unsafe_freeMemory(JNIEnv *env, jobject self, jlong address)
{
  free((void *) address);
}


void Unsafe_setMemory(JNIEnv *env, jobject self, jobject obj, jlong offset,
  jlong bytes, jbyte value)
{
}


jint Unsafe_arrayIndexScale(JNIEnv *env, jobject self, jclass class_object)
{
  LOG("arrayIndexScale")

  if (class_object != 0)
  {
    Class *clazz = Class::from_class_object(class_object);
    return clazz->component_type->type_size;
  }

  return 0;
}


jint Unsafe_arrayBaseOffset(JNIEnv *env, jobject self, jclass classObj)
{
  return 0;
}


jobject Unsafe_staticFieldBase(JNIEnv *env, jobject self, jobject fieldObj)
{
  Field *field;
  error_t errorValue = Field::from_field_object(fieldObj, &field);
  RETURN_VALUE_ON_FAIL(errorValue, 0)

  return field->declaring_class()->object;
}


jlong Unsafe_staticFieldOffset(JNIEnv *env, jobject self, jobject fieldObj)
{
  Field *field;
  error_t errorValue = Field::from_field_object(fieldObj, &field);
  RETURN_VALUE_ON_FAIL(errorValue, 0)

  return ((jlong) field->offset()) | STATIC_BIT;
}


jlong Unsafe_objectFieldOffset(JNIEnv *env, jobject self, jobject fieldObj)
{
  Field *field;
  error_t errorValue = Field::from_field_object(fieldObj, &field);
  RETURN_VALUE_ON_FAIL(errorValue, 0)

  jlong offset = field->offset();
  if (static_bit_set(offset))
  {
    EXIT_FATAL("Unsafe_objectFieldOffset: invalid offset")
  }

  return offset;
}


jobject Unsafe_allocateInstance(JNIEnv *env, jobject self, jclass class_obj)
{
  Class *clazz = Class::from_class_object(class_obj);
  if (clazz == 0)
  {
    LOG_WARN("Unsafe_allocateInstance - invalid parameter")
    return 0;
  }

  Object *obj;
  if (_vm->memory_manager()->allocate_object(clazz, &obj) != RETURN_OK)
  {
    LOG_WARN("Unsafe_allocateInstance - could not allocate object")
    return 0;
  }

  return obj;
}


void Unsafe_ensureClassInitialized(JNIEnv *env, jobject self, jclass classObj)
{
  if (classObj != 0)
  {
    Class *clazz = Class::from_class_object(classObj);

    if (!clazz->initialized)
    {
      _vm->class_loader()->initialize_class(clazz);
    }
  }
}


void Unsafe_monitorEnter(JNIEnv *env, jobject self, jobject obj)
{
  if (obj == 0)
  {
    obj->ensure_monitor()->enter();
  }
}


void Unsafe_monitorExit(JNIEnv *env, jobject self, jobject obj)
{
  if (obj != 0)
  {
    obj->ensure_monitor()->exit();
  }
}


jboolean Unsafe_tryMonitorEnter(JNIEnv *env, jobject self, jobject obj)
{
  if (obj == 0)
  {
    LOG_WARN("Unsafe_tryMonitorEnter - invalid parameter")
  }

  return obj->ensure_monitor()->try_enter();
}


void Unsafe_throwException(JNIEnv *env, jobject self, jthrowable exception)
{
  if (exception != 0)
  {
    _current_executor->throw_exception(exception);
  }
}


    #define COMPARE_AND_SWAP(name, type) \
    jboolean Unsafe_compareAndSwap##name(JNIEnv *env, jobject self, jobject obj, \
      jlong offset, type expected, type x) \
    { \
      return Unsafe_compareAndSwap<type>(obj, offset, expected, x); \
    }

COMPARE_AND_SWAP(Object, jobject)
COMPARE_AND_SWAP(Int, jint)
COMPARE_AND_SWAP(Long, jlong)

#endif

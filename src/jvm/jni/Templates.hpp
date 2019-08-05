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

#ifndef COLDSPOT_JVM_JNI_TEMPLATES_HPP_
#define COLDSPOT_JVM_JNI_TEMPLATES_HPP_

#define CALL_NONVIRTUAL_TYPE_METHOD(name, type, getter) \
  type JNICALL CallNonvirtual##name##Method(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) \
  { \
    va_list args; \
    va_start(args, methodID); \
    Value value = call_nonvirtual(obj, methodID, args); \
    va_end(args); \
    return value.getter(); \
  } \
  type JNICALL CallNonvirtual##name##MethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) \
  { \
    return call_nonvirtual(obj, methodID, args).getter(); \
  } \
  type JNICALL CallNonvirtual##name##MethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) \
  { \
    return call_nonvirtual(obj, methodID, args).getter(); \
  }


#define CALL_STATIC_TYPE_METHOD(name, type, getter) \
  type JNICALL CallStatic##name##Method(JNIEnv *env, jclass clazz, jmethodID methodID, ...) \
  { \
    va_list args; \
    va_start(args, methodID); \
    Value value = call_static(methodID, args); \
    va_end(args); \
    return value.getter(); \
  } \
  type JNICALL CallStatic##name##MethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) \
  { \
    return call_static(methodID, args).getter(); \
  } \
  type JNICALL CallStatic##name##MethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) \
  { \
    return call_static(methodID, args).getter(); \
  }


#define CALL_TYPE_METHOD(name, type, getter) \
  type JNICALL Call##name##Method(JNIEnv *env, jobject obj, jmethodID methodID, ...) \
  { \
    va_list args; \
    va_start(args, methodID); \
    Value value = call_virtual(obj, methodID, args); \
    va_end(args); \
    return value.getter(); \
  } \
  type JNICALL Call##name##MethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) \
  { \
    return call_virtual(obj, methodID, args).getter(); \
  } \
  type JNICALL Call##name##MethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) \
  { \
    return call_virtual(obj, methodID, args).getter(); \
  }


#define FROM_VA_LIST(type, args, value) \
  switch (type) { \
    case TYPE_BOOLEAN: { \
      value = Value((jboolean) va_arg(args, jint)); break; \
    } \
    case TYPE_BYTE: { \
      value = Value((jbyte) va_arg(args, jint)); break; \
    } \
    case TYPE_CHAR: { \
      value = Value((jchar) va_arg(args, jint)); break; \
    } \
    case TYPE_SHORT: { \
      value = Value((jshort) va_arg(args, jint)); break; \
    } \
    case TYPE_INT: { \
      value = Value(va_arg(args, jint)); break; \
    } \
    case TYPE_FLOAT: { \
      value = Value((jfloat) va_arg(args, jdouble)); break; \
    } \
    case TYPE_REFERENCE: { \
      jobject object = va_arg(args, jobject); \
      value = Value(object); break; \
    } \
    case TYPE_LONG: { \
      value = Value(va_arg(args, jlong)); break; \
    } \
    case TYPE_DOUBLE: { \
      value = Value(va_arg(args, jdouble)); break; \
    } \
    default: { \
      EXIT_FATAL("invalid argument from va_list"); \
    } \
  }


#define NEW_TYPE_ARRAY(name, type, length) \
  type##Array JNICALL New##name##Array(JNIEnv *env, jsize len) \
  { \
    return coldspot::NewArray<type>(env, length); \
  }


#define GET_STATIC_TYPE_FIELD(name, type, getter) \
  type JNICALL GetStatic##name##Field(JNIEnv *env, jclass clazz, jfieldID fieldID) \
  { \
    return fieldID->get_static().getter(); \
  }


#define GET_TYPE_ARRAY_ELEMENTS(name, type) \
  type *JNICALL Get##name##ArrayElements(JNIEnv *env, type##Array array, jboolean *isCopy) \
  { \
    return GetArrayElements<type>(array, isCopy); \
  }


#define GET_TYPE_ARRAY_REGION(name, type) \
  void JNICALL Get##name##ArrayRegion(JNIEnv *env, type##Array array, \
    jsize start, jsize len, type *buf) \
  { \
    GetArrayRegion(array, start, len, buf); \
  }


#define GET_TYPE_FIELD(name, type) \
  type JNICALL Get##name##Field(JNIEnv *env, jobject obj, jfieldID fieldID) \
  { \
    return fieldID->get<type>(obj); \
  }


#define RELEASE_TYPE_ARRAY_ELEMENTS(name, type) \
  void JNICALL Release##name##ArrayElements \
    (JNIEnv *env, type##Array array, type *elems, jint mode) { }


#define SET_STATIC_TYPE_FIELD(name, type) \
  void JNICALL SetStatic##name##Field(JNIEnv *env, jclass clazz, jfieldID field, type value) \
  { \
    field->set_static(value); \
  }


#define SET_TYPE_ARRAY_REGION(name, type) \
  void JNICALL Set##name##ArrayRegion(JNIEnv *env, type##Array array, jsize start, jsize len, const type *buf) \
  { \
    SetArrayRegion(array, start, len, buf); \
  }


#define SET_TYPE_FIELD(name, type) \
  void JNICALL Set##name##Field(JNIEnv *env, jobject obj, jfieldID field, type value) \
  { \
    field->set<type>(obj, value); \
  }

namespace coldspot
{

    inline Value call_nonvirtual(jobject object, jmethodID methodID,
        va_list args)
    {
        Value value;

        if (methodID != 0)
        {
            dynarray <Value> parameters(methodID->parameter_types().size());
            uint32_t count = 0;
            for (auto parameterType : methodID->parameter_types())
            {
                Value parameterValue;
                FROM_VA_LIST(parameterType->type, args, parameterValue)
                parameters[count++] = parameterValue;
            }

            if (object == 0)
            {
                _current_executor->throw_exception(
                    CLASSNAME_NULLPOINTEREXCEPTION);
            }
            else
            {
                methodID->invoke(object, parameters, &value, false);
            }
        }

        return value;
    }


    inline Value call_nonvirtual(jobject object, jmethodID methodID,
        const jvalue *args)
    {
        Value value;

        if (methodID != 0)
        {
            dynarray <Value> parameters(methodID->parameter_types().size());
            jint argsLength = 0;
            for (auto parameterType : methodID->parameter_types())
            {
                jvalue value = args[argsLength];
                parameters[argsLength++] = Value::by(value,
                    parameterType->type);
            }

            if (object == 0)
            {
                _current_executor->throw_exception(
                    CLASSNAME_NULLPOINTEREXCEPTION);
            }
            else
            {
                methodID->invoke(object, parameters, &value, false);
            }
        }

        return value;
    }


    inline Value call_static(jmethodID methodID, va_list args)
    {
        Value value;

        if (methodID != 0)
        {
            dynarray <Value> parameters(methodID->parameter_types().size());
            uint32_t count = 0;
            for (auto parameterType : methodID->parameter_types())
            {
                Value parameterValue;
                FROM_VA_LIST(parameterType->type, args, parameterValue)
                parameters[count++] = parameterValue;
            }

            methodID->invoke(0, parameters, &value);
        }

        return value;
    }


    inline Value call_static(jmethodID methodID, const jvalue *args)
    {
        Value value;

        if (methodID != 0)
        {
            dynarray <Value> parameters(methodID->parameter_types().size());
            jint argsLength = 0;
            for (auto parameterType : methodID->parameter_types())
            {
                jvalue value = args[argsLength];
                parameters[argsLength++] = Value::by(value,
                    parameterType->type);
            }

            methodID->invoke(0, parameters, &value);
        }

        return value;
    }


    inline Value call_virtual(jobject object, jmethodID methodID, va_list args)
    {
        Value value;

        if (methodID != 0)
        {
            dynarray <Value> parameters(methodID->parameter_types().size());
            uint32_t count = 0;
            for (auto parameterType : methodID->parameter_types())
            {
                Value parameterValue;
                FROM_VA_LIST(parameterType->type, args, parameterValue)
                parameters[count++] = parameterValue;
            }

            if (object == 0)
            {
                _current_executor->throw_exception(
                    CLASSNAME_NULLPOINTEREXCEPTION);
            }
            else
            {
                methodID->invoke(object, parameters, &value);
            }
        }

        return value;
    }


    inline Value call_virtual(jobject object, jmethodID methodID,
        const jvalue *args)
    {
        Value value;

        if (methodID != 0)
        {
            dynarray <Value> parameters(methodID->parameter_types().size());
            jint argsLength = 0;
            for (auto parameterType : methodID->parameter_types())
            {
                jvalue value = args[argsLength];
                parameters[argsLength++] = Value::by(value,
                    parameterType->type);
            }

            if (object == 0)
            {
                _current_executor->throw_exception(
                    CLASSNAME_NULLPOINTEREXCEPTION);
            }
            else
            {
                methodID->invoke(object, parameters, &value);
            }
        }

        return value;
    }


    template<typename T>
    inline T *GetArrayElements(jarray array, jboolean *copy)
    {
        if (array != 0)
        {
            if (copy != 0)
            {
                *copy = false;
            }

            return (T *) array->memory();
        }

        return 0;
    }


    template<typename T>
    inline void GetArrayRegion(jarray array, jsize start, jsize len, T *buf)
    {
        T *elements = GetArrayElements<T>(array, 0);

        if (elements != 0)
        {
            memcpy(buf, &elements[start], sizeof(T) * len);
        }
    }


    template<typename T>
    inline jarray NewArray(JNIEnv *env, jsize length)
    {
        jarray array = 0;

        THREAD_BLOCK
        error_t errorValue = Array::new_primitive_array<T>(length, &array);
        if (errorValue == RETURN_OK)
        {
            array = (jarray) env->NewLocalRef(array);
        }
        THREAD_UNBLOCK

        return array;
    }


    template<typename T>
    inline void SetArrayRegion(jarray array, jsize start, jsize len,
        const T *buf)
    {
        T *elements = GetArrayElements<T>(array, 0);

        if (elements != 0)
        {
            memcpy(&elements[start], buf, sizeof(T) * len);
        }
    }

}

#endif
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

namespace coldspot
{

    error_t Method::lookupInterface(Class **clazz, Method **method,
        Object *object)
    {
        return lookupVirtual(clazz, method, object);
    }


    error_t Method::lookupSpecial(Class **clazz, Method **method)
    {
        Class *currentClassType = *clazz;

        auto &frames = _current_executor->frames();
        if (!frames.empty())
        {
            Frame *frame = (Frame *) frames.peek();
            currentClassType = frame->clazz;
        }

        ClassFile *currentClassFile = currentClassType->class_file;
        if ((currentClassFile->accessFlags &
             ACCESS_FLAG_SUPER) // TODO getter for super
            && currentClassType->is_subclass_of(*clazz) &&
            (*method)->_signature.name != METHODNAME_CONSTRUCTOR)
        {
            Signature signature((*method)->_signature);
            *method = 0;
            Class *currentSuperClassType = currentClassType->super_class;

            error_t errorValue = RETURN_ERROR;
            while (currentSuperClassType != 0)
            {
                *clazz = currentSuperClassType;
                errorValue = currentSuperClassType->get_declared_method(
                    signature, method);
                if (errorValue == RETURN_OK)
                {
                    break;
                }
                currentSuperClassType = currentSuperClassType->super_class;
            }

            if (errorValue != RETURN_OK)
            {
                _current_executor->throw_exception(
                    CLASSNAME_ABSTRACTMETHODERROR, signature.name.c_str());
                return RETURN_EXCEPTION;
            }
        }

        return RETURN_OK;
    }


    error_t Method::lookupVirtual(Class **clazz, Method **method,
        Object *object)
    {
        // Object must not be null for virtual calls
        if (object == 0)
        {
            _current_executor->throw_exception(CLASSNAME_NULLPOINTEREXCEPTION);
            return RETURN_EXCEPTION;
        }

        // Search in the current object and all super-objects for the method to call
        error_t errorValue;
        Signature signature((*method)->_signature);
        Class *type = object->type();
        do
        {
            Method *currentMethod;
            errorValue = type->get_declared_method(signature, &currentMethod);
            if (errorValue == RETURN_OK)
            {
                (*clazz) = type;
                (*method) = currentMethod;
                break;
            }
            type = type->super_class;
        } while (type);

        // Throw exception if the method does not exist
        if (errorValue != RETURN_OK)
        {
            _current_executor->throw_exception(CLASSNAME_ABSTRACTMETHODERROR,
                signature.name.c_str());
            return RETURN_EXCEPTION;
        }

        return RETURN_OK;
    }


    Method::~Method()
    {
        DELETE_CONTAINER_OBJECTS(_exception_handlers)
        DELETE_OBJECT(_debug_infos)
        DELETE_OBJECT(_native_call)
    }


    error_t Method::invoke(Object *object, Value *parameters,
        Value *returnValue, bool lookup)
    {
        Class *invokeClass = _declaring_class;
        Method *invokeMethod = this;

        // Lookup is only needed for non-static methods
        if (lookup && !isStatic())
        {
            error_t errorValue;

            if (isPrivate() || object->type()->is_subclass_of(invokeClass) ||
                _signature.name == METHODNAME_CONSTRUCTOR)
            {
                errorValue = Method::lookupSpecial(&invokeClass, &invokeMethod);
            }
            else if (_declaring_class->is_interface())
            {
                errorValue = Method::lookupInterface(&invokeClass,
                    &invokeMethod, object);
            }
            else
            {
                errorValue = Method::lookupVirtual(&invokeClass, &invokeMethod,
                    object);
            }

            RETURN_ON_FAIL(errorValue);
        }

        // Initialize class
        _vm->class_loader()->initialize_class(invokeClass);

        // Inokve method
        if (isNative())
        {
            if (isStatic())
            {
                return invokeMethod->invokeNative(_declaring_class->object,
                    parameters, returnValue);
            }
            return invokeMethod->invokeNative(object, parameters, returnValue);
        }
        else
        {
            if (isStatic())
            {
                return invokeMethod->invokeJava(0, parameters, returnValue);
            }
            return invokeMethod->invokeJava(object, parameters, returnValue);
        }
    }


    error_t Method::invokeJava(Object *object, Value *parameters,
        Value *returnValue)
    {
        // Create frame
        auto &frames = _current_executor->frames();
        uint8_t *frameMemory = frames.push(Frame::size(this));
        Frame *frame = Frame::create(FRAMETYPE_JAVA, _declaring_class, this,
            frameMemory);

        if (_signature.name == "startsWith")
        { ;
        }

        // Copy object to the local variables
        uint32_t index = 0;
        if (object != 0)
        {
            frame->localVariables[index++] = object;
        }

        // Copy parameters to the local variables
        for (uint32_t i = 0; i < _parameter_types.size(); ++i)
        {
            frame->localVariables[index++] = parameters[i];

            // Long and double need two indices
            Type type = parameters[i].type();
            if (type == TYPE_LONG || type == TYPE_DOUBLE)
            {
                ++index;
            }
        }

        // Enter monitor for synchronized methods
        if (is_synchronized())
        {
            if (isStatic())
            {
                _declaring_class->object->ensure_monitor()->enter();
            }
            else
            {
                object->ensure_monitor()->enter();
            }
        }

        // Execute method
        return _current_executor->execute(frame, returnValue);
    }


    error_t Method::invokeNative(Object *object, Value *parameters,
        Value *returnValue)
    {
        // Get and store pointer to native-call if it was never called before
        if (_native_call == 0)
        {
            // Build the name of the native method
            StringBuilder builder;
            native_method_name(builder);
            String method_name = builder.str();

            // Get pointer to native function
            Function_t native_function = _vm->library_binder()->get_function(
                method_name);
            if (native_function == 0)
            {
                StringBuilder message;
                message << "method '" << method_name << "' could not be bound";
                _current_executor->throw_exception(
                    CLASSNAME_UNSATISFIEDLINKERROR, message.str().c_str());
                return RETURN_EXCEPTION;
            }

            // Create and initialize native call
            _native_call = new NativeCall(this, native_function);
            _native_call->init();
        }

        // Create frame
        auto &frames = _current_executor->frames();
        uint8_t *frame_memory = frames.push(_frame_size);
        Frame *frame = Frame::create(FRAMETYPE_NATIVE, _declaring_class, this,
            frame_memory);

        // Create local-references
        List <jobject> local_references;

        // Allocate object as local-reference
        local_references.addBack(object);

        // Allocate local-reference for each reference-type parameter
        for (uint32_t i = 0; i < _parameter_types.size(); ++i)
        {
            if (parameters[i].type() == TYPE_REFERENCE)
            {
                local_references.addBack(parameters[i].as_object());
            }
        }

        // Associate local-references with the frame
        frame->localReferences = &local_references;

        // Enter monitor for synchronized methods
        bool synchronized = is_synchronized();
        if (synchronized)
        {
            object->ensure_monitor()->enter();
        }

        // Call native function
        uint64_t resultValue = _native_call->call(object, parameters);

        // Exit monitor for synchronized methods
        if (synchronized)
        {
            object->ensure_monitor()->exit();
        }

        // Exception that was not handled by the method
        Object *unhandled_exception = frame->exception;

        // Pop frame
        frames.pop();

        // Rethrow unhandled exception to the remaining stack
        if (unhandled_exception != 0)
        {
            _current_executor->throw_exception(unhandled_exception);
            return RETURN_EXCEPTION;
        }

        // Store return value
        if (_return_type != PrimitiveClass<void>::get())
        {
            returnValue->set_type(_return_type->type);
            returnValue->set_value(resultValue);
        }

        return RETURN_OK;
    }


    // TODO RAEUDIG!!!
    void Method::unboxParameters(Value *parameters)
    {
        auto begin = _parameter_types.begin();
        auto end = _parameter_types.end();

        uint32_t count = 0;

        while (begin != end)
        {
            Class *parameterType = *begin;
            Value &parameterValue = parameters[count++];

            // Unboxing only to primitive types,
            // only from reference-types
            // and only for non-null values
            if (!parameterType->is_primitive() ||
                parameterValue.type() != Type::TYPE_REFERENCE ||
                parameterValue.value() == 0)
            {
                ++begin;
                continue;
            }

            Object *internParameterObject = parameterValue.as_object();
            Class *internParameterClass = internParameterObject->type();

            // Only non-array objects can be boxed
            if (internParameterClass->is_array())
            {
                ++begin;
                continue;
            }

            JNIEnv *env = _vm->jni_interface();  // TODO no jni
            jobject parameterObject = _vm->add_local_ref(internParameterObject);
            jclass parameterClass = env->GetObjectClass(parameterObject);

            if (parameterType->type == Type::TYPE_BOOLEAN &&
                internParameterClass->name == CLASSNAME_BOOLEAN)
            {
                jmethodID unboxMethod = env->GetMethodID(parameterClass,
                    "booleanValue", "()Z");
                jboolean unboxedValue = env->CallBooleanMethod(parameterObject,
                    unboxMethod);
                Value internUnboxedValue(unboxedValue);
                parameterValue.set_type(internUnboxedValue.type());
                parameterValue.set_value(internUnboxedValue.value());

            }
            else if (parameterType->type == Type::TYPE_BYTE &&
                     internParameterClass->name == CLASSNAME_BYTE)
            {
                jmethodID unboxMethod = env->GetMethodID(parameterClass,
                    "byteValue", "()B");
                jbyte unboxedValue = env->CallByteMethod(parameterObject,
                    unboxMethod);
                Value internUnboxedValue(unboxedValue);
                parameterValue.set_type(internUnboxedValue.type());
                parameterValue.set_value(internUnboxedValue.value());

            }
            else if (parameterType->type == Type::TYPE_CHAR &&
                     internParameterClass->name == CLASSNAME_CHARACTER)
            {
                jmethodID unboxMethod = env->GetMethodID(parameterClass,
                    "charValue", "()C");
                jchar unboxedValue = env->CallCharMethod(parameterObject,
                    unboxMethod);
                Value internUnboxedValue(unboxedValue);
                parameterValue.set_type(internUnboxedValue.type());
                parameterValue.set_value(internUnboxedValue.value());

            }
            else if (parameterType->type == Type::TYPE_SHORT &&
                     internParameterClass->name == CLASSNAME_SHORT)
            {
                jmethodID unboxMethod = env->GetMethodID(parameterClass,
                    "shortValue", "()S");
                jshort unboxedValue = env->CallShortMethod(parameterObject,
                    unboxMethod);
                Value internUnboxedValue(unboxedValue);
                parameterValue.set_type(internUnboxedValue.type());
                parameterValue.set_value(internUnboxedValue.value());

            }
            else if (parameterType->type == Type::TYPE_INT &&
                     internParameterClass->name == CLASSNAME_INTEGER)
            {
                jmethodID unboxMethod = env->GetMethodID(parameterClass,
                    "intValue", "()I");
                jint unboxedValue = env->CallIntMethod(parameterObject,
                    unboxMethod);
                Value internUnboxedValue(unboxedValue);
                parameterValue.set_type(internUnboxedValue.type());
                parameterValue.set_value(internUnboxedValue.value());

            }
            else if (parameterType->type == Type::TYPE_FLOAT &&
                     internParameterClass->name == CLASSNAME_FLOAT)
            {
                jmethodID unboxMethod = env->GetMethodID(parameterClass,
                    "floatValue", "()F");
                jfloat unboxedValue = env->CallFloatMethod(parameterObject,
                    unboxMethod);
                Value internUnboxedValue(unboxedValue);
                parameterValue.set_type(internUnboxedValue.type());
                parameterValue.set_value(internUnboxedValue.value());

            }
            else if (parameterType->type == Type::TYPE_LONG &&
                     internParameterClass->name == CLASSNAME_LONG)
            {
                jmethodID unboxMethod = env->GetMethodID(parameterClass,
                    "longValue", "()J");
                jlong unboxedValue = env->CallLongMethod(parameterObject,
                    unboxMethod);
                Value internUnboxedValue(unboxedValue);
                parameterValue.set_type(internUnboxedValue.type());
                parameterValue.set_value(internUnboxedValue.value());

            }
            else if (parameterType->type == Type::TYPE_DOUBLE &&
                     internParameterClass->name == CLASSNAME_DOUBLE)
            {
                jmethodID unboxMethod = env->GetMethodID(parameterClass,
                    "doubleValue", "()D");
                jdouble unboxedValue = env->CallDoubleMethod(parameterObject,
                    unboxMethod);
                Value internUnboxedValue(unboxedValue);
                parameterValue.set_type(internUnboxedValue.type());
                parameterValue.set_value(internUnboxedValue.value());
            }
            else
            {
                EXIT_FATAL("could not unbox value");
            }

            ++begin;
        }
    }


    bool Method::isAbstract() const
    {
        return _access_flags & ACCESS_FLAG_ABSTRACT;
    }


    bool Method::isBridge() const
    {
        return _access_flags & ACCESS_FLAG_BRIDGE;
    }


    bool Method::isFinal() const
    {
        return _access_flags & ACCESS_FLAG_FINAL;
    }


    bool Method::isNative() const
    {
        return _access_flags & ACCESS_FLAG_NATIVE;
    }


    bool Method::isPrivate() const
    {
        return _access_flags & ACCESS_FLAG_PRIVATE;
    }


    bool Method::isProtected() const
    {
        return _access_flags & ACCESS_FLAG_PROTECTED;
    }


    bool Method::isPublic() const
    {
        return _access_flags & ACCESS_FLAG_PUBLIC;
    }


    bool Method::isStatic() const
    {
        return _access_flags & ACCESS_FLAG_STATIC;
    }


    bool Method::isStrictfp() const
    {
        return _access_flags & ACCESS_FLAG_STRICT;
    }


    bool Method::is_synchronized() const
    {
        return _access_flags & ACCESS_FLAG_SYNCHRONIZED;
    }


    bool Method::isSynthetic() const
    {
        return _access_flags & ACCESS_FLAG_SYNCHRONIZED;
    }


    bool Method::isVarargs() const
    {
        return _access_flags & ACCESS_FLAG_VARARGS;
    }


    void Method::native_method_name(StringBuilder &builder)
    {
        builder << "Java_";
        append_mapped(_declaring_class->name.substring(0,
            _declaring_class->name.length()), builder);
        builder << '_';
        append_mapped(_signature.name, builder);

        for (uint16_t i = 0;
             i < _declaring_class->declared_methods.length(); ++i)
        {
            Method *overloadedMethod = _declaring_class->declared_methods[i];

            bool overloaded =
                overloadedMethod != this &&  // Not the current method
                overloadedMethod->isNative() && // Only native methods
                overloadedMethod->_signature.name ==
                _signature.name; // Same name

            if (overloaded)
            {
                builder << "__";

                for (auto parameterType : _parameter_types)
                {
                    if (parameterType->is_primitive())
                    {
                        builder << parameterType->name;
                    }
                    else
                    {
                        if (parameterType->name[0] == '[')
                        {
                            append_mapped(parameterType->name, builder);
                        }
                        else
                        {
                            append_mapped("L", builder);
                            append_mapped(parameterType->name, builder);
                            append_mapped(";", builder);
                        }
                    }
                }

                break;
            }
        }
    }


    void Method::append_mapped(const String &str, StringBuilder &builder)
    {
        for (uint32_t i = 0; i < str.length(); ++i)
        {
            if (str[i] == '/')
            {
                builder << '_';
            }
            else if (str[i] == '_')
            {
                builder << "_1";
            }
            else if (str[i] == ';')
            {
                builder << "_2";
            }
            else if (str[i] == '[')
            {
                builder << "_3";
            }
            else if (str[i] == '$')
            {
                builder << "_00024";
            }
            else
            {
                builder << str[i];
            }
        }
    }

}

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

#include <ffi.h>

namespace coldspot
{

    class NativeCall::Impl
    {
    public:

        Impl(Method *method, Function_t nativeFunction) : _method(method),
                                                          _native_function(
                                                              nativeFunction),
                                                          _return_type(0),
                                                          _parameter_types(0)
        {
        }

        ~Impl()
        {
            DELETE_ARRAY(_parameter_types)
        }

        void init()
        {
            unsigned int parameter_count =
                _method->parameter_types().size() + 2;
            unsigned int parameter_index = 0;

            _parameter_types = new ffi_type *[parameter_count];

            // Set parameter-types
            _parameter_types[parameter_index++] = &ffi_type_pointer;  // JNIEnv*
            _parameter_types[parameter_index++] = &ffi_type_pointer;  // jobject or jclass

            for (auto &parameterType : _method->parameter_types())
            {
                _parameter_types[parameter_index++] = to_ffi_type(
                    parameterType->type);
            }

            // Set return-type
            _return_type = to_ffi_type(_method->return_type()->type);

            // Build call interface
            ffi_status status = ffi_prep_cif(&_call_interface, FFI_DEFAULT_ABI,
                parameter_count, _return_type, _parameter_types);
            if (status != ffi_status::FFI_OK)
            {
                EXIT_FATAL("could not build call-interface for native method");
            }
        }

        uint64_t call(Object *object, Value *parameters)
        {
            unsigned int parameter_count =
                _method->parameter_types().size() + 2;
            unsigned int parameter_index = 0;

            void *ffi_parameters[parameter_count];

            // The first parameter of a jni-function
            // is always a pointer to the jni-interface
            JNIEnv *env = _vm->jni_interface();
            ffi_parameters[parameter_index++] = &env;

            Value object_value = object;
            ffi_parameters[parameter_index++] = &object_value.value();

            // Set parameters
            for (uint32_t i = 0; i < _method->parameter_types().size(); ++i)
            {
                ffi_parameters[parameter_index++] = &parameters[i].value();
            }

            // Call function
            ffi_arg result = 0;
            ffi_call(&_call_interface, (function_t) _native_function,
                (void *) &result, ffi_parameters);

            return (uint64_t) result;
        }

    private:

        using function_t = void (*)();

        Method *_method;
        Function_t _native_function;

        ffi_cif _call_interface;
        ffi_type *_return_type;
        ffi_type **_parameter_types;

        ffi_type *to_ffi_type(Type type)
        {
            switch (type)
            {
                case Type::TYPE_BOOLEAN:
                {
                    return &ffi_type_uint8;
                }
                case Type::TYPE_BYTE:
                {
                    return &ffi_type_sint8;
                }
                case Type::TYPE_CHAR:
                {
                    return &ffi_type_uint16;
                }
                case Type::TYPE_DOUBLE:
                {
                    return &ffi_type_double;
                }
                case Type::TYPE_FLOAT:
                {
                    return &ffi_type_float;
                }
                case Type::TYPE_INT:
                {
                    return &ffi_type_sint32;
                }
                case Type::TYPE_LONG:
                {
                    return &ffi_type_sint64;
                }
                case Type::TYPE_SHORT:
                {
                    return &ffi_type_sint16;
                }
                case Type::TYPE_REFERENCE:
                {
                    return &ffi_type_pointer;
                }
                case Type::TYPE_VOID:
                {
                    return &ffi_type_void;
                }
                default:
                {
                    EXIT_FATAL("invalid type for native-call");
                }
            }
            return 0;  // Never reached
        }
    };


    NativeCall::NativeCall(Method *method, Function_t nativeFunction)
    {
        _impl = new Impl(method, nativeFunction);
    }


    NativeCall::~NativeCall()
    {
        DELETE_OBJECT(_impl)
    }


    void NativeCall::init()
    {
        _impl->init();
    }


    uint64_t NativeCall::call(Object *object, Value *parameters)
    {
        return _impl->call(object, parameters);
    }

}

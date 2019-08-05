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

#ifndef COLDSPOT_JVM_CLASS_METHOD_HPP_
#define COLDSPOT_JVM_CLASS_METHOD_HPP_

#include <jvm/jdk/Global.hpp>
#include <jvm/class/MethodDebugInfos.hpp>
#include <jvm/common/HashMap.hpp>
#include <jvm/common/List.hpp>
#include <jvm/common/StringBuilder.hpp>
#include <jvm/system/NativeTypes.hpp>
#include <jvm/Error.hpp>

#include "ExceptionHandler.hpp"
#include "Signature.hpp"

namespace coldspot
{

    class Class;
    class Object;
    class MethodInfo;
    class NativeCall;

    class Method
    {
    public:

        using LocalVariableMap = HashMap<uint16_t, Signature>;

        static error_t lookupInterface(Class **clazz, Method **method,
            Object *object);
        static error_t lookupSpecial(Class **clazz, Method **method);

        static error_t lookupVirtual(Class **clazz, Method **method,
            Object *object);

        Method(Class *declaringClass, const Signature &signature)
            : _declaring_class(declaringClass), _signature(signature),
              _return_type(0), _code(0), _locals_count(0), _operands_count(0),
              _frame_size(0), _debug_infos(0), _native_call(0), _slot(0) { }
        ~Method();

        // Invokes a method.
        error_t invoke(Object *object, Value *parameters, Value *returnValue,
            bool lookup = true);

        // Invokes a java (non-native) method.
        error_t invokeJava(Object *object, Value *parameters,
            Value *returnValue);

        // Invokes a native method.
        // The first parameters must be either:
        // - a java-class (static methods)
        // - object (instance-methods)
        error_t invokeNative(Object *object, Value *parameters,
            Value *returnValue);

        // Unboxing values inline (e.g. java/lang/Integer to int).
        void unboxParameters(Value *parameters);

        // Checks access-flags.
        bool isAbstract() const;
        bool isBridge() const;
        bool isFinal() const;
        bool isNative() const;
        bool isPrivate() const;
        bool isProtected() const;
        bool isPublic() const;
        bool isStatic() const;
        bool isStrictfp() const;
        bool is_synchronized() const;
        bool isSynthetic() const;
        bool isVarargs() const;

        // Getters.
        Class *declaring_class() const { return _declaring_class; }
        const Signature &signature() const { return _signature; }
        Class *return_type() const { return _return_type; }
        List<Class *> &parameter_types() { return _parameter_types; }
        uint16_t access_flags() const { return _access_flags; }
        List<ExceptionHandler *> &exception_handlers() { return _exception_handlers; }
        uint8_t *code() const { return _code; }
        const uint16_t &locals_count() const { return _locals_count; }
        const uint16_t &operands_count() const { return _operands_count; }
        MethodDebugInfos *debug_infos() const { return _debug_infos; }
        NativeCall *native_call() const { return _native_call; }
        uint16_t slot() const { return _slot; }

        // Setters.
        void set_return_type(Class *return_type) { _return_type = return_type; }
        void set_access_flags(uint16_t flags) { _access_flags = flags; }
        void set_code(uint8_t *code) { _code = code; }
        void set_locals_count(
            uint16_t locals_count) { _locals_count = locals_count; }
        void set_operands_count(
            uint16_t operands_count) { _operands_count = operands_count; }
        void set_frame_size(uint32_t frame_size) { _frame_size = frame_size; }
        void set_debug_infos(
            MethodDebugInfos *debug_infos) { _debug_infos = debug_infos; }
        void set_native_call(
            NativeCall *native_call) { _native_call = native_call; }
        void set_slot(uint16_t slot) { _slot = slot; }

    private:

        // General
        Class *_declaring_class;
        Signature _signature;

        // Types of the descriptor
        Class *_return_type;
        List<Class *> _parameter_types;

        uint16_t _access_flags;

        // Exception Handlers
        List<ExceptionHandler *> _exception_handlers;

        // Bytecode
        uint8_t *_code;

        // Limits
        uint16_t _locals_count;
        uint16_t _operands_count;

        // Size of the frame of this class
        uint32_t _frame_size;

        // Debug infos
        MethodDebugInfos *_debug_infos;

        // Handle to native
        NativeCall *_native_call;

        // Mapping to java/lang/reflect/Method object
        uint16_t _slot;

        // Builds the native method name.
        void native_method_name(StringBuilder &builder);

        // Applies unicode character translation.
        void append_mapped(const String &str, StringBuilder &builder);
    };

}

#endif

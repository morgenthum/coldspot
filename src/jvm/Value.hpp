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

#ifndef COLDSPOT_JVM_VALUE_HPP_
#define COLDSPOT_JVM_VALUE_HPP_

#include <cstdint>

#include <jvm/jni/Types.hpp>
#include <jvm/Logging.hpp>

#include "Type.hpp"

namespace coldspot
{

    class Array;
    class Object;

    // Represents a primitive or reference value
    class Value
    {
    public:

        static Value by(jvalue value, Type type)
        {
            switch (type)
            {
                case Type::TYPE_BOOLEAN:
                    return value.z;
                case Type::TYPE_BYTE:
                    return value.b;
                case Type::TYPE_CHAR:
                    return value.c;
                case Type::TYPE_SHORT:
                    return value.s;
                case Type::TYPE_INT:
                    return value.i;
                case Type::TYPE_FLOAT:
                    return value.f;
                case Type::TYPE_REFERENCE:
                    return value.l;
                case Type::TYPE_LONG:
                    return value.j;
                case Type::TYPE_DOUBLE:
                    return value.d;
                default:
                    EXIT_FATAL("could not convert jvalue to internal value")
            }
            return Value(type, 0);
        }

        Value() : Value(Type::TYPE_VOID, 0) { }
        Value(Type type, uint64_t value) : _type(type), _value(value) { }

        // Constructors with implicit conversion
        Value(jboolean b) : _type(Type::TYPE_BOOLEAN), _value(b) { }
        Value(jbyte b) : _type(Type::TYPE_BYTE), _value(b) { }
        Value(jchar c) : _type(Type::TYPE_CHAR), _value(c) { }
        Value(jshort s) : _type(Type::TYPE_SHORT), _value(s) { }
        Value(jint i) : _type(Type::TYPE_INT), _value(i) { }
        Value(jfloat f) : _type(Type::TYPE_FLOAT),
                          _value(*(reinterpret_cast<uint64_t *>(&f))) { }
        Value(jlong l) : _type(Type::TYPE_LONG), _value(l) { }
        Value(jdouble d) : _type(Type::TYPE_DOUBLE),
                           _value(*(reinterpret_cast<uint64_t *>(&d))) { }
        Value(uint32_t r) : _type(Type::TYPE_RETURNADDRESS), _value(r) { }
        Value(Object *o) : _type(Type::TYPE_REFERENCE),
                           _value(reinterpret_cast<uint64_t>(o)) { }

        // Getters
        Type type() { return _type; }
        uint64_t &value() { return _value; }

        // Setters
        void set_type(Type type) { _type = type; }
        void set_value(uint64_t value) { _value = value; }

        // Converters
        jboolean as_boolean() { return static_cast<jboolean>(_value); }
        jbyte as_byte() { return static_cast<jbyte>(_value); }
        jchar as_char() { return static_cast<jchar>(_value); }
        jshort as_short() { return static_cast<jshort>(_value); }
        jint as_int() { return static_cast<jint>(_value); }
        jfloat as_float() { return *(reinterpret_cast<jfloat *>(&_value)); }
        jlong as_long() { return static_cast<jlong>(_value); }
        jdouble as_double() { return *(reinterpret_cast<jdouble *>(&_value)); }
        uint32_t as_return_addr() { return static_cast<uint32_t>(_value); }
        Object *as_object() { return reinterpret_cast<Object *>(_value); }
        Array *as_array() { return reinterpret_cast<Array *>(_value); }

    private:

        Type _type;
        uint64_t _value;
    };

}

#endif

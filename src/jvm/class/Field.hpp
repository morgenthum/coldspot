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

#ifndef COLDSPOT_JVM_CLASS_FIELD_HPP_
#define COLDSPOT_JVM_CLASS_FIELD_HPP_

#include <jvm/jdk/Global.hpp>
#include <jvm/Value.hpp>

#include "Signature.hpp"

namespace coldspot
{

    class Class;
    class FieldInfo;
    class Object;

    class Field
    {
    public:

        // Converts a java/lang/reflect/Field object to an internal Field.
        static error_t from_field_object(Object *object, Field **field);

        Field(Class *declaring_class, const Signature &signature)
            : _declaring_class(declaring_class), _signature(signature),
              _type(0), _slot(0), _offset(0) { }

        // Static values
        Value get_static() const;
        void set_static(Value value) const;

        template<typename T>
        T get_static() const;
        template<typename T>
        void set_static(T value) const;

        // Non static values
        Value get(Object *object) const;
        void set(Object *object, Value value);

        template<typename T>
        T get(Object *object);
        template<typename T>
        void set(Object *object, T value);

        // Checks access-flags.
        bool is_enum_element() const;
        bool is_final() const;
        bool is_private() const;
        bool is_protected() const;
        bool is_public() const;
        bool is_static() const;
        bool is_synthetic() const;
        bool is_transient() const;
        bool is_volatile() const;

        // Getters.
        Class *declaring_class() const { return _declaring_class; }
        const Signature &signature() const { return _signature; }
        Class *type() const { return _type; }
        uint16_t slot() const { return _slot; }
        jint offset() const { return _offset; }
        uint16_t access_flags() const { return _access_flags; }

        // Setters.
        void set_type(Class *type) { _type = type; }
        void set_slot(uint16_t slot) { _slot = slot; }
        void set_offset(jint offset) { _offset = offset; }
        void set_access_flags(uint16_t flags) { _access_flags = flags; }

    private:

        Class *_declaring_class;
        Signature _signature;
        Class *_type;
        uint16_t _slot;
        jint _offset;

        uint16_t _access_flags;

        inline Value value_from_memory(uint8_t *memory) const;
        inline void value_to_memory(uint8_t *memory, Value &value) const;
    };

}

#endif

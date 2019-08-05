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

    error_t Field::from_field_object(Object *object, Field **field)
    {
        Class *declaring_class;
        error_t errorValue = java_lang_reflect_Field::getClass(object,
            &declaring_class);
        RETURN_ON_FAIL(errorValue)

        jint slot;
        errorValue = java_lang_reflect_Field::getSlot(object, &slot);
        RETURN_ON_FAIL(errorValue)

        * field = declaring_class->get_declared_field(slot);

        return RETURN_OK;
    }


    Value Field::get_static() const
    {
        if (!_declaring_class->initialized)
        {
            _vm->class_loader()->initialize_class(_declaring_class);
        }

        return value_from_memory(_declaring_class->static_memory);
    }


    void Field::set_static(Value value) const
    {
        if (!_declaring_class->initialized)
        {
            _vm->class_loader()->initialize_class(_declaring_class);
        }

        value_to_memory(_declaring_class->static_memory, value);
    }


    template<typename T>
    T Field::get_static() const
    {
        return *((T *) (_declaring_class->static_memory + _offset));
    }

#define EXPLICIT(type) template type Field::get_static<type>() const;
    EXPLICIT(Object *)
    EXPLICIT(jboolean)
    EXPLICIT(jbyte)
    EXPLICIT(jchar)
    EXPLICIT(jshort)
    EXPLICIT(jint)
    EXPLICIT(jfloat)
    EXPLICIT(jlong)
    EXPLICIT(jdouble)
#undef EXPLICIT


    template<typename T>
    void Field::set_static(T value) const
    {
        *((T *) (_declaring_class->static_memory + _offset)) = value;
    }

#define EXPLICIT(type) template void Field::set_static<type>(type value) const;
    EXPLICIT(Object *)
    EXPLICIT(jboolean)
    EXPLICIT(jbyte)
    EXPLICIT(jchar)
    EXPLICIT(jshort)
    EXPLICIT(jint)
    EXPLICIT(jfloat)
    EXPLICIT(jlong)
    EXPLICIT(jdouble)
#undef EXPLICIT


    Value Field::get(Object *object) const
    {
        return value_from_memory(object->memory());
    }


    void Field::set(Object *object, Value value)
    {
        value_to_memory(object->memory(), value);
    }


    template<typename T>
    T Field::get(Object *object)
    {
        return *((T *) (object->memory() + _offset));
    }

#define EXPLICIT(type) template type Field::get<type>(Object *object);
    EXPLICIT(Object *)
    EXPLICIT(jboolean)
    EXPLICIT(jbyte)
    EXPLICIT(jchar)
    EXPLICIT(jshort)
    EXPLICIT(jint)
    EXPLICIT(jfloat)
    EXPLICIT(jlong)
    EXPLICIT(jdouble)
#undef EXPLICIT


    template<typename T>
    void Field::set(Object *object, T value)
    {
        *((T *) (object->memory() + _offset)) = value;
    }

#define EXPLICIT(type) template void Field::set<type>(Object *object, type value);
    EXPLICIT(Object *)
    EXPLICIT(jboolean)
    EXPLICIT(jbyte)
    EXPLICIT(jchar)
    EXPLICIT(jshort)
    EXPLICIT(jint)
    EXPLICIT(jfloat)
    EXPLICIT(jlong)
    EXPLICIT(jdouble)
#undef EXPLICIT


    bool Field::is_enum_element() const
    {
        return _access_flags & ACCESS_FLAG_ENUM;
    }


    bool Field::is_final() const
    {
        return _access_flags & ACCESS_FLAG_FINAL;
    }


    bool Field::is_private() const
    {
        return _access_flags & ACCESS_FLAG_PRIVATE;
    }


    bool Field::is_protected() const
    {
        return _access_flags & ACCESS_FLAG_PROTECTED;
    }


    bool Field::is_public() const
    {
        return _access_flags & ACCESS_FLAG_PUBLIC;
    }


    bool Field::is_static() const
    {
        return _access_flags & ACCESS_FLAG_STATIC;
    }


    bool Field::is_synthetic() const
    {
        return _access_flags & ACCESS_FLAG_SYNTHETIC;
    }


    bool Field::is_transient() const
    {
        return _access_flags & ACCESS_FLAG_TRANSIENT;
    }


    bool Field::is_volatile() const
    {
        return _access_flags & ACCESS_FLAG_VOLATILE;
    }


    Value Field::value_from_memory(uint8_t *memory) const
    {
        Value value(_type->type, 0);
        memcpy(&value.value(), &memory[_offset], _type->type_size);
        return value;
    }


    void Field::value_to_memory(uint8_t *memory, Value &value) const
    {
        memcpy(&memory[_offset], &value.value(), _type->type_size);
    }

}

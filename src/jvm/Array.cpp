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

#define LOAD_CHAR_ARRAY_CLASS(errorValue) \
  if (_charArrayClass == 0) { \
    errorValue = _vm->class_loader()->load_array("[C", 0, &_charArrayClass); \
    RETURN_ON_FAIL(errorValue); \
    if (!_charArrayClass->initialized) { \
      errorValue = _vm->class_loader()->initialize_class(_charArrayClass); \
      RETURN_ON_FAIL(errorValue); \
    } \
  }

namespace coldspot
{

    static Class *_charArrayClass = 0;


    void Array::copy_elements(Array *src, Array *dest, jint srcStart,
        jint destStart, jint length)
    {
        uint8_t typeSize = src->type()->component_type->type_size;

        uint8_t *srcMemory = src->memory() + (srcStart * typeSize);
        uint8_t *destMemory = dest->memory() + (destStart * typeSize);

        memcpy(destMemory, srcMemory, length * typeSize);
    }


    template<typename T>
    error_t Array::new_primitive_array(jint length, Array **array)
    {
        if (length < 0)
        {
            _current_executor->throw_exception(
                CLASSNAME_NEGATIVEARRAYSIZEEXCEPTION);
            return RETURN_EXCEPTION;
        }

        char name[3];
        name[0] = '[';
        name[1] = PrimitiveClass<T>::get()->name[0];
        name[2] = 0;

        // Load array class
        Class *arrayClass;
        error_t errorValue = _vm->class_loader()->load_array(name, 0,
            &arrayClass);
        RETURN_ON_FAIL(errorValue);

        // Allocate array
        return _vm->memory_manager()->allocate_array(arrayClass, length, array);
    }

#define EXPLICIT(type) template error_t Array::new_primitive_array<type>(jint length, Array **array);
    EXPLICIT(jboolean)
    EXPLICIT(jbyte)
    EXPLICIT(jchar)
    EXPLICIT(jshort)
    EXPLICIT(jint)
    EXPLICIT(jfloat)
    EXPLICIT(jlong)
    EXPLICIT(jdouble)
#undef EXPLICIT


    error_t Array::new_array(Class *clazz, jsize length, Array **array)
    {
        if (length < 0)
        {
            _current_executor->throw_exception(
                CLASSNAME_NEGATIVEARRAYSIZEEXCEPTION);
            return RETURN_EXCEPTION;
        }

        // Build name of array-class
        StringBuilder nameBuilder;
        nameBuilder << '[';
        if (clazz->is_primitive())
        {
            nameBuilder << clazz->name;
        }
        else
        {
            nameBuilder << 'L' << clazz->name << ';';
        }

        // Load array class
        Class *arrayClass;
        error_t errorValue = _vm->class_loader()->load_array(nameBuilder.str(),
            0, &arrayClass);
        RETURN_ON_FAIL(errorValue);

        // Allocate array
        return _vm->memory_manager()->allocate_array(arrayClass, length, array);
    }


    error_t Array::new_object_array(Class *clazz, jint length, Array **array)
    {
        if (length < 0)
        {
            _current_executor->throw_exception(
                CLASSNAME_NEGATIVEARRAYSIZEEXCEPTION);
            return RETURN_EXCEPTION;
        }

        // Build name of array-class
        StringBuilder name;
        name << "[L" << clazz->name << ';';

        // Load array class
        Class *arrayClass;
        error_t errorValue = _vm->class_loader()->load_array(name.str(), 0,
            &arrayClass);
        RETURN_ON_FAIL(errorValue);

        // Allocate array
        return _vm->memory_manager()->allocate_array(arrayClass, length, array);
    }


    error_t Array::new_char_array(const UTF16String &string, Array **array)
    {
        // Load char-array-class
        error_t errorValue;
        LOAD_CHAR_ARRAY_CLASS(errorValue)

        // Allocate char-array
        errorValue = _vm->memory_manager()->allocate_array(_charArrayClass,
            string.length(), array);
        RETURN_ON_FAIL(errorValue);

        // Fill char-array
        for (size_t i = 0; i < string.length(); ++i)
        {
            errorValue = (*array)->set_value(i, (jchar) string[i]);
            RETURN_ON_FAIL(errorValue);
        }

        return RETURN_OK;
    }


    error_t Array::get_value(jint index, Value *value)
    {
        error_t errorValue = validate_index(index);
        RETURN_ON_FAIL(errorValue);

        Class *componentType = type()->component_type;

        value->set_type(componentType->type);
        uint8_t componentSize = static_cast<uint8_t>(componentType->type_size);

        memcpy(&value->value(), &memory()[componentSize * index],
            componentSize);

        return RETURN_OK;
    }


    template<typename T>
    error_t Array::get_value(jint index, T *value)
    {
        error_t errorValue = validate_index(index);
        RETURN_ON_FAIL(errorValue);

        *value = ((T *) memory())[index];

        return RETURN_OK;
    }

#define EXPLICIT(type) template error_t Array::get_value<type>(jint index, type *value);
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


    error_t Array::set_value(jint index, Value value)
    {
        error_t errorValue = validate_index(index);
        RETURN_ON_FAIL(errorValue);

        Class *componentType = type()->component_type;

        uint8_t componentSize = static_cast<uint8_t>(componentType->type_size);

        memcpy(&memory()[componentSize * index], &value.value(), componentSize);

        return RETURN_OK;
    }


    error_t Array::clone(Object **destination) const
    {
        error_t errorValue = Array::new_array(type()->component_type, _length,
            (Array **) destination);
        RETURN_ON_FAIL(errorValue)

        if (*destination != 0)
        {
            copy_memory(this, *destination);
        }

        return RETURN_OK;
    }


    error_t Array::validate_index(jint index)
    {
        if (index < 0 || index >= _length)
        {
            _current_executor->throw_exception(
                CLASSNAME_ARRAYINDEXOUTOFBOUNDSEXCEPTION,
                String::valueOf(index).c_str());
            return RETURN_EXCEPTION;
        }

        return RETURN_OK;
    }

}

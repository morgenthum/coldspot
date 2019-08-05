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
#include <jvm/VirtualMachine.hpp>

namespace coldspot
{

    error_t java_lang_String::new_(const UTF16String &utf16String,
        Object **string)
    {
        // Create char-array
        Array *charArray;
        error_t errorValue = Array::new_char_array(utf16String, &charArray);
        RETURN_ON_FAIL(errorValue)

        // Load string-constructor to initialize with char-array
        Method *constructor;
        errorValue = _vm->builtin.stringClass->get_method(
            Signature("([C)V", METHODNAME_CONSTRUCTOR), &constructor);
        RETURN_ON_FAIL(errorValue)

        // Create string
        Value parameter = charArray;
        errorValue = Object::new_object(constructor, &parameter, string);
        RETURN_ON_FAIL(errorValue)

        return RETURN_OK;
    }


    error_t java_lang_String::intern(const UTF16String &utf16_string,
        Object **string)
    {
        auto entry = _vm->string_pool().get(utf16_string);
        if (entry != 0)
        {
            *string = entry->value;
            return RETURN_OK;
        }

        error_t errorValue = java_lang_String::new_(utf16_string, string);
        RETURN_ON_FAIL(errorValue)

        _vm->string_pool().put(utf16_string, *string);

        return RETURN_OK;
    }


    error_t java_lang_String::length(Object *string, jint *length)
    {

        // Exception if the string is null
        if (string == 0)
        {
            _current_executor->throw_exception(CLASSNAME_NULLPOINTEREXCEPTION);
            return RETURN_EXCEPTION;
        }

        // Get object-class
        Class *clazz = string->type();

        // Load char storage
        Field *value_field;
        error_t error_value = clazz->get_declared_field(
            Signature("[C", "value"), &value_field);
        RETURN_ON_FAIL(error_value)

        // Read array and its length
        Array *value_array = (Array *) value_field->get<Object *>(string);
        *length = value_array->length();

        return RETURN_OK;
    }


    error_t java_lang_String::chars(Object *string, jchar **chars, jint *length)
    {
        // Load char storage
        Field *value_field;
        error_t error_value = _vm->builtin.stringClass->get_declared_field(
            Signature("[C", "value"), &value_field);
        RETURN_ON_FAIL(error_value)

        // Read array and its length
        Array *value_array = (Array *) value_field->get<Object *>(string);

        *chars = (jchar *) value_array->memory();
        *length = value_array->length();

        return RETURN_OK;
    }

}
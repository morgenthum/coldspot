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

    error_t java_lang_Throwable::new_(Class *clazz, const UTF16String &message,
        Object **throwable)
    {
        Object *string;
        error_t errorValue = java_lang_String::new_(message, &string);
        RETURN_ON_FAIL(errorValue)

        Method *constructor;
        String descriptor("(Ljava/lang/String;)V");
        errorValue = clazz->get_method(
            Signature(descriptor, METHODNAME_CONSTRUCTOR), &constructor);
        RETURN_ON_FAIL(errorValue)

        Value parameter = string;
        return Object::new_object(constructor, &parameter, throwable);
    }


    error_t java_lang_Throwable::detailMessage(Object *throwable,
        Object **message)
    {
        Class *throwable_class;
        error_t error_value = _vm->class_loader()->load_class(
            "java/lang/Throwable", &throwable_class);
        RETURN_ON_FAIL(error_value)

        Field *msg_field;
        error_value = throwable_class->get_declared_field(
            Signature("Ljava/lang/String;", "detailMessage"), &msg_field);
        RETURN_ON_FAIL(error_value)

        * message = msg_field->get<Object *>(throwable);

        return RETURN_OK;
    }

}
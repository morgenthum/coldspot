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

    error_t java_lang_reflect_Field::getClass(Object *field, Class **clazz)
    {

        Class *fieldClass = field->type();

        Field *classField;
        error_t errorValue = fieldClass->get_field(
            Signature("Ljava/lang/Class;", "clazz"), &classField);
        RETURN_ON_FAIL(errorValue)

        Object *classObject = classField->get<Object *>(field);

        *clazz = Class::from_class_object(classObject);

        return RETURN_OK;
    }

    error_t java_lang_reflect_Field::getSlot(Object *field, jint *slot)
    {

        Class *fieldClass = field->type();

        Field *slotField;
        error_t errorValue = fieldClass->get_field(Signature("I", "slot"),
            &slotField);
        RETURN_ON_FAIL(errorValue)

        * slot = slotField->get<jint>(field);

        return RETURN_OK;
    }

}
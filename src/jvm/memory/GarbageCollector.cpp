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

    error_t GarbageCollector::mark_unused(Object *object)
    {
        if (object != 0 && object->used())
        {
            object->set_used(false);
        }

        return RETURN_OK;
    }


    error_t GarbageCollector::mark_used(Object *object)
    {
        if (object != 0 && !object->used())
        {
            object->set_used(true);

            // Mark class-loader
            mark_used(object->type()->class_loader);

            // Mark class-object
            mark_used(object->type()->object);

            auto &declared_fields = object->type()->declared_fields;

            for (uint16_t i = 0; i < declared_fields.length(); ++i)
            {
                auto declared_field = declared_fields[i];

                if (!declared_field->type()->is_primitive())
                {
                    Object *value = declared_field->get<Object *>(object);
                    error_t error_value = mark_used(value);
                    RETURN_ON_FAIL(error_value);
                }
            }

            // Special handling for arrays
            if (object->type()->is_array())
            {
                return mark_array_used(static_cast<Array *>(object));
            }
        }

        return RETURN_OK;
    }


    error_t GarbageCollector::mark_array_used(Array *array)
    {
        Class *clazz = array->type();

        if (!clazz->component_type->is_primitive())
        {
            jint length = array->length();

            for (jint i = 0; i < length; ++i)
            {
                Object *value;
                error_t errorValue = array->get_value<Object *>(i, &value);
                RETURN_ON_FAIL(errorValue);

                if (value != 0)
                {
                    errorValue = mark_used(value);
                    RETURN_ON_FAIL(errorValue);
                }
            }
        }

        return RETURN_OK;
    }

}

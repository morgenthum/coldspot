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

#include <cstdlib>

#include <jvm/Global.hpp>

namespace coldspot
{

    error_t ObjectAllocator::allocate_object(Class *clazz, Object **object)
    {
        // Allocate memory
        uint8_t *memory = (uint8_t *) calloc(1,
            sizeof(Object) + clazz->object_size);

        // Initialize objects
        Object *current_object = new(memory) Object(clazz);
        uint8_t *current_memory = memory + sizeof(Object);

        current_object->set_memory_size(clazz->object_size);
        current_object->set_memory(current_memory);

        // Calculate and set offsets
        calculate_offsets(current_object, clazz, 0);

        *object = (Object *) memory;

        return RETURN_OK;
    }


    error_t ObjectAllocator::allocate_array(Class *clazz, jsize length,
        Array **array)
    {
        // Get the size of the java array
        uint32_t array_size = clazz->component_type->type_size * length;

        // Get the size of all super-objects
        uint32_t java_size = array_size;
        if (clazz->super_class != 0)
        {
            java_size += clazz->object_size;
        }

        // Allocate memory
        uint8_t *memory = (uint8_t *) calloc(1, sizeof(Array) + java_size);

        // Initialize objects
        Object *current_object = new(memory) Array(clazz, length);
        uint8_t *current_memory = memory + sizeof(Array);

        current_object->set_memory_size(java_size);
        current_object->set_memory(current_memory);

        // Calculate and set offsets
        calculate_offsets(current_object, clazz->super_class, array_size);

        *array = (Array *) memory;

        return RETURN_OK;
    }


    void ObjectAllocator::release_object(Object *object)
    {
        object->~Object();
        free(object);
    }


    uint32_t ObjectAllocator::calculate_offsets(Object *object, Class *clazz,
        uint32_t offset)
    {
        if (clazz->super_class != 0)
        {
            offset = calculate_offsets(object, clazz->super_class, offset);
        }

        for (uint16_t i = 0; i < clazz->declared_fields.length(); ++i)
        {
            Field *field = clazz->declared_fields[i];
            if (!field->is_static())
            {
                field->set_offset(offset);
                offset += field->type()->type_size;
            }
        }

        return offset;
    }
}

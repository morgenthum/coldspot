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

    error_t Object::new_object(Method *constructor, Value *parameters,
        Object **object)
    {
        // Initialize class to instantiate
        Class *clazz = constructor->declaring_class();

        if (clazz->is_abstract())
        {
            _current_executor->throw_exception(CLASSNAME_INSTANTIATIONERROR,
                clazz->name.c_str());
            return RETURN_EXCEPTION;
        }

        error_t error_value;
        if (!clazz->initialized)
        {
            error_value = _vm->class_loader()->initialize_class(clazz);
            RETURN_ON_FAIL(error_value);
        }

        // Allocate object
        error_value = _vm->memory_manager()->allocate_object(clazz, object);
        RETURN_ON_FAIL(error_value);

        // Invoke constructor
        Value value;
        error_value = constructor->invoke(*object, parameters, &value);
        RETURN_ON_FAIL(error_value);

        return RETURN_OK;
    }


    error_t Object::new_object_default(Class *clazz, Object **object)
    {
        // Load the default-constructor
        Method *constructor;
        error_t errorValue = clazz->get_method(
            Signature("()V", METHODNAME_CONSTRUCTOR), &constructor);
        RETURN_ON_FAIL(errorValue)

        // Create the object
        return Object::new_object(constructor, 0, object);
    }


    error_t Object::clone(Object **destination) const
    {
        error_t errorValue = _vm->memory_manager()->allocate_object(_type,
            destination);
        RETURN_ON_FAIL(errorValue)

        if (*destination != 0)
        {
            copy_memory(this, *destination);
        }

        return RETURN_OK;
    }


    template<typename T>
    T Object::get_value(uint32_t offset)
    {
        return *((T *) (_memory + offset));
    }

#define EXPLICIT(type) template type Object::get_value<type>(uint32_t offset);
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
    void Object::set_value(uint32_t offset, T value)
    {
        *((T *) (_memory + offset)) = value;
    }

#define EXPLICIT(type) template void Object::set_value<type>(uint32_t offset, type value);
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


    void Object::copy_memory(const Object *source, Object *target)
    {
        memcpy(target->memory(), source->memory(), source->memory_size());
    }

}

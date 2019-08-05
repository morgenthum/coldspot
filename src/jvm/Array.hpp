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

#ifndef COLDSPOT_JVM_ARRAY_HPP_
#define COLDSPOT_JVM_ARRAY_HPP_

#include <jvm/jdk/Global.hpp>
#include <jvm/Error.hpp>

#include "Object.hpp"

namespace coldspot
{

    class UTF16String;

    // Represents a java-array.
    class Array : public Object
    {
    public:

        // Copies all elements from this array to destination.
        static void copy_elements(Array *src, Array *dest, jint srcStart,
            jint destStart, jint length);

        template<typename T>
        static error_t new_primitive_array(jint length, Array **array);

        // Creates a new array.
        static error_t new_array(Class *clazz, jint length, Array **array);

        // Creates a new array of object type.
        static error_t new_object_array(Class *clazz, jint length,
            Array **array);

        // Creates a new char-array from a string.
        static error_t new_char_array(const UTF16String &string, Array **array);

        Array(Class *type, jint length) : Object(type), _length(length) { }

        error_t clone(Object **destination) const override;

        // Returns the generic or specific value at the specified index.
        error_t get_value(jint index, Value *value);
        template<typename T>
        error_t get_value(jint index, T *value);

        // Sets the value at the specified index.
        error_t set_value(jint index, Value value);

        // Getters.
        jint length() const { return _length; }

    private:

        jint _length;

        inline error_t validate_index(jint index);
    };

}

#endif

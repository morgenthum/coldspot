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

#ifndef COLDSPOT_JVM_CLASSFILE_CLASSFILEREADER_HPP_
#define COLDSPOT_JVM_CLASSFILE_CLASSFILEREADER_HPP_

#include <jvm/Error.hpp>

namespace coldspot
{

    class Annotation;
    class AttributeInfo;
    class ClassFile;
    class ElementValue;
    class ClassFileInputStream;
    class VerificationTypeInfo;

    class ClassFileReader
    {
    public:

        ClassFileReader(ClassFileInputStream *input_stream,
            ClassFile *class_file) : _input_stream(input_stream),
                                     _class_file(class_file)
        {
        }

        // Initializes the the reader with the specified  class-name.
        error_t init(const String &className);

        // Reads the class-file.
        error_t read_class();

    private:

        ClassFileInputStream *_input_stream;
        ClassFile *_class_file;

        /**
         * Reads the constant-pool-entries.
         *
         * @return the error-code
         */
        error_t read_constant_pool();

        /**
         * Reads the interfaces.
         */
        void read_interfaces();

        /**
         * Reads the fields.
         *
         * @return the error-code
         */
        error_t read_fields();

        /**
         * Reads the methods.
         *
         * @return the error-code
         */
        error_t read_methods();

        /**
         * Reads the attributes and stores them to the attributes of the target.
         *
         * @param target to store the attributes
         * @return the error-code
         */
        template<typename T>
        error_t read_attributes(T *target);

        /**
         * Reads an attribute.
         *
         * @param attribute to read
         * @return the error-code
         */
        error_t read_attribute(AttributeInfo **attribute);

        /**
         * Reads a verification-type-info.
         *
         * @param info to read
         * @return the error-code
         */
        error_t read_verification_type(VerificationTypeInfo **info);

        /**
         * Reads an annotation.
         *
         * @param annotation to read
         * @return the error-code
         */
        error_t read_annotation(Annotation **annotation);

        /**
         * Reads an element-value.
         *
         * @param value to read
         * @return the error-code
         */
        error_t read_element_value(ElementValue **value);

        /**
         * Reads sizeof(T)-bytes from the input-stream.
         *
         * @param target to store the bytes
         */
        template<typename T>
        void read(T *target);

        /**
         * Swaps the endianess of target.
         *
         * @param target
         */
        template<typename T>
        void swap_endianness(T *target);
    };

}

#endif

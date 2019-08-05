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

#ifndef COLDSPOT_JVM_CLASS_CLASS_HPP_
#define COLDSPOT_JVM_CLASS_CLASS_HPP_

#include <jvm/class/Signature.hpp>
#include <jvm/common/HashMap.hpp>
#include <jvm/common/List.hpp>
#include <jvm/common/SmartArray.hpp>
#include <jvm/common/String.hpp>
#include <jvm/jdk/Global.hpp>
#include <jvm/Error.hpp>
#include <jvm/Value.hpp>
#include <jvm/Array.hpp>
#include "Method.hpp"

namespace coldspot
{

    class ClassFile;
    class Object;
    class Field;
    class Method;
    class RunTimeConstantPoolEntry;

    class Class
    {
    public:

        // Converts a class object to the internal class.
        static Class *from_class_object(Object *object);

        // Converts from e.g. "java/lang/Object" to "java.lang.Object".
        static String to_java_class_name(const String &internalName);

        // Parsed class-file
        ClassFile *class_file;

        // Name of this class
        String name;

        // Name of the source-file of this class
        String source_file;

        // Extending super-class
        Class *super_class;

        // Defining class-loader
        Object *class_loader;

        // Associated java-class
        Object *object;

        // Component-type of array-class
        Class *component_type;

        // Type info
        Type type;
        uint8_t type_size;
        uint32_t object_size;

        // Implementing interfaces
        HashMap<String, Class *> interface_classes;

        // Mapping between signatures and slots
        HashMap <Signature, uint16_t> declared_field_slots;
        HashMap <Signature, uint16_t> declared_method_slots;

        // Declared fields and methods
        SmartArray<Field *, uint16_t> declared_fields;
        SmartArray<Method *, uint16_t> declared_methods;

        // Resolved fields and methods
        HashMap<Signature, Field *> fields;
        HashMap<Signature, Method *> methods;

        // Static field storage
        uint32_t static_memory_size;
        uint8_t *static_memory;

        SmartArray<RunTimeConstantPoolEntry *, uint16_t> contant_pool;

        // State of initialization
        bool resolved;
        bool initialized;
        bool primitive;

        Class() : class_file(0), super_class(0), class_loader(0), object(0),
                  component_type(0), type(TYPE_VOID), type_size(0),
                  object_size(0), static_memory_size(0), static_memory(0),
                  resolved(false), initialized(false), primitive(false) { }
        ~Class();

        // Member access without lookup.
        error_t get_declared_field(const Signature &signature, Field **field);
        Field *get_declared_field(uint16_t slot);
        void get_declared_fields(List<Field *> &fields);
        error_t get_declared_method(const Signature &signature,
            Method **method);
        Method *get_declared_method(uint16_t slot);
        void get_declared_methods(List<Method *> &methods);

        // Member access with lookup.
        error_t get_field(const Signature &signature, Field **field);
        error_t get_method(const Signature &signature, Method **method);

        // Type checking.
        bool is_abstract();
        bool is_array();
        bool is_implementing(Class *clazz);
        bool is_interface();
        bool is_primitive();
        bool is_subclass_of(Class *clazz);
        bool is_castable_to(Class *clazz);

        // Resolve constant pool entries.
        error_t get_class_from_cp(uint16_t index, Class **clazz);
        error_t get_field_from_cp(uint16_t index, Field **field);
        error_t get_method_from_cp(uint16_t index, Method **method);
        error_t get_string_from_cp(uint16_t index, Object **string);
        jint get_integer_from_cp(uint16_t index);
        jfloat get_float_from_cp(uint16_t index);
        jlong get_long_from_cp(uint16_t index);
        jdouble get_double_from_cp(uint16_t index);
        String &get_utf8_from_cp(uint16_t index);

        // Access to static memory
        template<typename T>
        T get_value(uint32_t offset);
        template<typename T>
        void set_value(uint32_t offset, T value);

    private:

        // Member lookup.
        inline error_t lookup_field(const Signature &signature, Field **field);
        inline error_t lookup_method(const Signature &signature,
            Method **method);
    };

}

#endif

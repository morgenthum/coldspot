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

#ifndef COLDSPOT_JVM_CLASS_CLASSLOADER_HPP_
#define COLDSPOT_JVM_CLASS_CLASSLOADER_HPP_

#include <jvm/common/HashMap.hpp>
#include <jvm/common/Pair.hpp>
#include <jvm/common/String.hpp>

#include <jvm/thread/Mutex.hpp>

#include <jvm/Error.hpp>
#include <jvm/classfile/FieldInfo.hpp>
#include <jvm/classfile/MethodInfo.hpp>

namespace coldspot
{

    class ClassFileInputStream;
    class Object;
    class Class;
    class Field;
    class Method;
    class Signature;

    using ClassIdentifier = Pair<Object *, String>;

    class ClassLoader
    {
    public:

        ~ClassLoader();

        // Loads a class with the bootstrap loader or the specified loader.
        error_t load_class(const String &name);
        error_t load_class(const String &name, Class **clazz);
        error_t load_class(const String &name, Object *classLoader,
            Class **clazz);

        // Loads an array class with the specified loader.
        error_t load_array(const String &name, Object *classLoader,
            Class **clazz);

        // Loads a primitive class with the bootstrap loader.
        error_t load_primitive(const String &name, Class **clazz);

        // Derives a class with the loader from the input stream.
        error_t define_class(const String &name, Object *classLoader,
            ClassFileInputStream *inputStream, Class **clazz);

        // Initializes the class if it is not already initialized.
        error_t initialize_class(Class *clazz);

        // Getters.
        HashMap<Object *, Class *> &object_mapping() { return _object_mapping; }
        HashMap<ClassIdentifier, Class *> &loaded_classes() { return _loaded_classes; }

    private:

        Mutex _load_mutex;
        HashMap<Object *, Class *> _object_mapping;
        HashMap<ClassIdentifier, Class *> _loaded_classes;

        // Parses the class file and associates it with the class.
        error_t read_classfile(Class *clazz,
            ClassFileInputStream *input_stream);

        // Creates an object of java/lang/Class and associates it with the class.
        error_t create_object(Class *clazz);

        // Linking resolves and prepares the class.
        error_t link_class(Class *clazz);

        // Sets all static fields of the class to their default-values.
        void prepare_class(Class *clazz);

        // Resolves the class and it elements
        error_t resolve_class(Class *clazz);
        error_t resolve_field(Class *clazz, const Signature &signature);
        error_t resolve_method(Class *clazz, const Signature &signature);

        // Resolves the types.
        error_t resolve_field_signature(Class *clazz, FieldInfo *info,
            Field *field);
        error_t resolve_method_signature(Class *clazz, MethodInfo *info,
            Method *method);

        // For example:
        // descriptor = "(Ljava/lang/String;I)V" => (field or method descriptor)
        // index = 1 (char index in the string)
        // Loads the String class using the classLoader and sets
        // the type and class attribute of typeInfo with this class.
        error_t resolve_by_descriptor(const String &descriptor, uint16_t &index,
            Object *classLoader, Class **typeInfo);

        // Sets attributes of the method (e.g. max locals, exception handlers).
        error_t collect_method_attributes(Class *clazz, MethodInfo *info,
            Method *method);
    };

}

#endif

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

#ifndef COLDSPOT_JVM_OBJECT_HPP_
#define COLDSPOT_JVM_OBJECT_HPP_

#include <cstdint>
#include <limits>

#include <jvm/common/HashMap.hpp>
#include <jvm/common/Memory.hpp>
#include <jvm/jdk/Global.hpp>
#include <jvm/Monitor.hpp>

namespace coldspot
{

    class Class;
    class Field;
    class Method;
    class Value;

    // Represents a java-object.
    class Object
    {
    public:

        // Creates a new object with the specified constructor and parameters.
        static error_t new_object(Method *constructor, Value *parameters,
            Object **object);

        // Creates a new object from the class using the default constructor.
        static error_t new_object_default(Class *clazz, Object **object);

        Object(Class *type) : _type(type), _monitor(0), _memory_size(0),
                              _memory(0), _used(true) { }

        virtual ~Object()
        {
            DELETE_OBJECT(_monitor);
        }

        // Clones the object.
        virtual error_t clone(Object **destination) const;

        // Returns the identity-hash-code of the object.
        jint identity_hash_code() const
        {
            return (jint)(((jlong)
            this) % std::numeric_limits<jint>::max());
        }

        template<typename T>
        T get_value(uint32_t offset);
        template<typename T>
        void set_value(uint32_t offset, T value);

        // Lazy creation of the monitor
        Monitor *ensure_monitor()
        {
            return _monitor == 0 ? (_monitor = new Monitor) : _monitor;
        }

        // Getters
        Class *type() const { return _type; }
        uint32_t memory_size() const { return _memory_size; }
        uint8_t *memory() const { return _memory; }
        bool used() const { return _used; }

        // Setters
        void set_memory_size(
            uint32_t memory_size) { _memory_size = memory_size; }
        void set_memory(uint8_t *memory) { _memory = memory; }
        void set_used(bool used) { _used = used; }

    protected:

        // Copies the memory of the object
        static void copy_memory(const Object *source, Object *target);

    private:

        Class *_type;
        Monitor *_monitor;
        uint32_t _memory_size;
        uint8_t *_memory;
        bool _used;
    };

}

#endif

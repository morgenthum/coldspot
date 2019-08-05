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

#ifndef COLDSPOT_JVM_MEMORY_MEMORYMANAGER_HPP_
#define COLDSPOT_JVM_MEMORY_MEMORYMANAGER_HPP_

#include <jvm/common/heap.hpp>
#include <jvm/thread/Lockable.hpp>
#include <jvm/Error.hpp>

namespace coldspot
{

    class Array;
    class Class;
    class Object;
    class ObjectAllocator;

    class MemoryManager
    {
    public:

        MemoryManager();

        ~MemoryManager();

        // Allocates a object and stores it in the heap-space.
        error_t allocate_object(Class *clazz, Object **object);

        // Allocates an array and stores it in the heap-space.
        error_t allocate_array(Class *clazz, jsize length, Array **array);

        // Releases the object from the heap-space and releases its memory.
        void release_object(Object *object);

        // Getters.
        Lockable <heap<Object *>> &get_objects()
        {
            return _objects;
        }

    protected:

        Lockable <heap<Object *>> _objects;

        ObjectAllocator *_objectAllocator;
    };

}

#endif

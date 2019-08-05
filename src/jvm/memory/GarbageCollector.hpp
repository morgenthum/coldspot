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

#ifndef COLDSPOT_JVM_MEMORY_GARBAGECOLLECTOR_HPP_
#define COLDSPOT_JVM_MEMORY_GARBAGECOLLECTOR_HPP_

#include <jvm/Error.hpp>

namespace coldspot
{

    class Array;

    class FinalizerThread;

    class Object;

    class GarbageCollector
    {
    public:

        virtual ~GarbageCollector()
        {
        }

        // Removes all unused objects.
        virtual void collectGarbage() = 0;

        // Removes all objects and deletes every running thread
        // but the finalizer
        virtual void collectGarbageForExit() = 0;

    protected:

        // Marks the object and all dependent objects as unused
        // (super-object, fields, etc.).
        error_t mark_unused(Object *object);

        // Marks the object and all dependent objects as used
        // (super-object, fields, etc.).
        error_t mark_used(Object *object);

    private:

        // Marks the array-object as used.
        // If the component-type of the array-object is a reference-type,
        // all fields will be also marked as used.
        error_t mark_array_used(Array *array);
    };

}

#endif

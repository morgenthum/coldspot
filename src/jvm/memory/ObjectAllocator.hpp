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

#ifndef COLDSPOT_JVM_MEMORY_OBJECTALLOCATOR_HPP_
#define COLDSPOT_JVM_MEMORY_OBJECTALLOCATOR_HPP_

#include <jvm/Error.hpp>

namespace coldspot
{

    class Array;
    class Class;
    class Object;

    class ObjectAllocator
    {
    public:

        error_t allocate_object(Class *clazz, Object **object);
        error_t allocate_array(Class *clazz, jsize length, Array **array);
        virtual void release_object(Object *object);

    private:

        inline uint32_t calculate_offsets(Object *object, Class *clazz,
            uint32_t offset);
    };

}

#endif

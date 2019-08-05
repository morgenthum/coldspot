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

    MemoryManager::MemoryManager()
    {
        _objectAllocator = new ObjectAllocator;
    }


    MemoryManager::~MemoryManager()
    {
        auto iterator = _objects->begin();
        while (iterator != _objects->end())
        {
            release_object(*iterator);
            iterator = _objects->erase(iterator);
        }

        DELETE_OBJECT(_objectAllocator)
    }


    error_t MemoryManager::allocate_object(Class *clazz, Object **object)
    {
        error_t errorValue = _objectAllocator->allocate_object(clazz, object);
        RETURN_ON_FAIL(errorValue)

        _objects.lock();
        _objects->add(*object);
        _objects.unlock();

        return RETURN_OK;
    }


    error_t MemoryManager::allocate_array(Class *clazz, jsize length,
        Array **array)
    {
        error_t errorValue = _objectAllocator->allocate_array(clazz, length,
            array);
        RETURN_ON_FAIL(errorValue)

        _objects.lock();
        _objects->add(*array);
        _objects.unlock();

        return RETURN_OK;
    }


    void MemoryManager::release_object(Object *object)
    {
        _objectAllocator->release_object(object);
    }

}
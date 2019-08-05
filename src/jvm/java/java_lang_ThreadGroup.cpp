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

    error_t java_lang_ThreadGroup::newSystemThreadGroup(Object **threadGroup)
    {
        // Load system-thread-group-constructor
        Method *constructor;
        error_t errorValue = _vm->builtin.threadGroupClass->get_method(
            Signature("()V", METHODNAME_CONSTRUCTOR), &constructor);
        RETURN_ON_FAIL(errorValue);

        // Create system-thread-group
        errorValue = Object::new_object(constructor, 0, threadGroup);
        RETURN_ON_FAIL(errorValue);

        return RETURN_OK;
    }


    error_t java_lang_ThreadGroup::newThreadGroup(Object *parent, Object *name,
        Object **threadGroup)
    {
        // Load system-thread-group-constructor
        Method *constructor;
        error_t errorValue = _vm->builtin.threadGroupClass->get_method(
            Signature("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V",
                METHODNAME_CONSTRUCTOR), &constructor);
        RETURN_ON_FAIL(errorValue);

        // Create thread-group
        Value parameters[2];
        parameters[0] = parent;
        parameters[1] = name;
        errorValue = Object::new_object(constructor, parameters, threadGroup);
        RETURN_ON_FAIL(errorValue)

        return RETURN_OK;
    }

}
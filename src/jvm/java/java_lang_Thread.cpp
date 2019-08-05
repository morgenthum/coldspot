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

    error_t java_lang_Thread::newThread(Object **thread)
    {
        return Object::new_object_default(_vm->builtin.threadClass, thread);
    }


    error_t java_lang_Thread::newThread(Object *vmThread, Object *name,
        jint priority, jboolean b, Object **thread)
    {
    }


    error_t java_lang_Thread::isDaemon(Object *thread, jboolean *daemon)
    {
        Method *isDaemonMethod;
        error_t errorValue = _vm->builtin.threadClass->get_method(
            Signature("()Z", "isDaemon"), &isDaemonMethod);
        RETURN_ON_FAIL(errorValue)

        Value value;
        errorValue = isDaemonMethod->invoke(thread, 0, &value);
        RETURN_ON_FAIL(errorValue)

        * daemon = value.as_boolean();

        return RETURN_OK;
    }


    error_t java_lang_Thread::setPriority(Object *thread, jint priority)
    {
        // Load priority-field
        Field *priorityField;
        error_t errorValue = _vm->builtin.threadClass->get_field(
            Signature("I", "priority"), &priorityField);
        RETURN_ON_FAIL(errorValue);

        // Set priority
        priorityField->set(thread, priority);

        return RETURN_OK;
    }

}
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
#include <jvm/VirtualMachine.hpp>

namespace coldspot
{

    error_t java_lang_Object::finalize(Object *object)
    {
        // Nothing to do for a null-object
        if (object == 0)
        {
            return RETURN_OK;
        }

        // Get object-class
        Class *clazz = object->type();

        // Get finalize-method
        Method *method;
        error_t errorValue = clazz->get_method(Signature("()V", "finalize"),
            &method);
        RETURN_ON_FAIL(errorValue);

        // Nothing to do for objects that don't overwrite the finalize-method
        if (method->declaring_class() == _vm->builtin.objectClass)
        {
            return RETURN_OK;
        }

        // Invoke finalize-method
        Value value;
        return method->invoke(object, 0, &value);
    }

}
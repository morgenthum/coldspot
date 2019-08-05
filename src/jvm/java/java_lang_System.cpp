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

    error_t java_lang_System::initializeSystemClass()
    {
        // Initialize the system class
        _vm->class_loader()->initialize_class(_vm->builtin.systemClass);

        // Load method
        Method *method;
        error_t errorValue = _vm->builtin.systemClass->get_method(
            Signature("()V", "initializeSystemClass"), &method);
        RETURN_ON_FAIL(errorValue)

        // Invoke
        errorValue = method->invoke(0, 0, 0);
        RETURN_ON_FAIL(errorValue)

        return RETURN_OK;
    }

}
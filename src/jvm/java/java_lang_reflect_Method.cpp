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

    error_t java_lang_reflect_Method::getParameterTypes(Method *method,
        Array **parameterTypes)
    {
        Class *clazz;
        error_t errorValue = _vm->class_loader()->load_class(CLASSNAME_CLASS,
            &clazz);
        RETURN_ON_FAIL(errorValue)

        errorValue = Array::new_object_array(clazz,
            method->parameter_types().size(), parameterTypes);
        RETURN_ON_FAIL(errorValue)

        jint i = 0;
        for (auto parameterType : method->parameter_types())
        {
            errorValue = (*parameterTypes)->set_value(i++,
                parameterType->object);
            RETURN_ON_FAIL(errorValue)
        }

        return RETURN_OK;
    }

}
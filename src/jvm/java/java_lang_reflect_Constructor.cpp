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


    error_t java_lang_reflect_Constructor::newInstance(Method *method,
        Object **object)
    {
        Class *clazz;
        error_t errorValue = _vm->class_loader()->load_class(
            "java/lang/reflect/Constructor", &clazz);
        RETURN_ON_FAIL(errorValue)

        Method *constructor;
        errorValue = clazz->get_declared_method(Signature(
            "(Ljava/lang/Class;[Ljava/lang/Class;[Ljava/lang/Class;IILjava/lang/String;[B[B)V",
            METHODNAME_CONSTRUCTOR), &constructor);
        RETURN_ON_FAIL(errorValue)

        Object *signature;
        errorValue = java_lang_String::intern(method->signature().descriptor,
            &signature);
        RETURN_ON_FAIL(errorValue)

        Array *parameterTypes;
        errorValue = java_lang_reflect_Method::getParameterTypes(method,
            &parameterTypes);
        RETURN_ON_FAIL(errorValue)

        Value parameters[8];
        parameters[0] = method->declaring_class()->object; // declaringClass
        parameters[1] = parameterTypes;
        parameters[2] = (Object *) 0; // TODO checkedExceptions
        parameters[3] = (jint) method->access_flags(); // modifiers
        parameters[4] = (jint) method->slot(); // slot
        parameters[5] = signature; // signatures
        parameters[6] = (Object *) 0; // TODO annotations
        parameters[7] = (Object *) 0; // TODO parameterAnnotations

        return Object::new_object(constructor, parameters, object);
    }

}

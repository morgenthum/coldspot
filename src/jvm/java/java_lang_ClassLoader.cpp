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

    error_t java_lang_ClassLoader::loadClass(Object *classLoader,
        const String &name, Object **clazz)
    {
        // Load loadClass method
        Method *loadClassMethod;
        Signature signature("(Ljava/lang/String;)Ljava/lang/Class;",
            "loadClass");
        error_t errorValue = classLoader->type()->get_method(signature,
            &loadClassMethod);
        RETURN_ON_FAIL(errorValue);

        // Create string object
        Object *classNameObject;
        errorValue = java_lang_String::new_(name, &classNameObject);
        RETURN_ON_FAIL(errorValue);

        // Inovke loadClass method
        Value result;
        Value parameter = classNameObject;
        errorValue = loadClassMethod->invoke(classLoader, &parameter, &result);
        RETURN_ON_FAIL(errorValue);

        *clazz = result.as_object();

        return RETURN_OK;
    }

}
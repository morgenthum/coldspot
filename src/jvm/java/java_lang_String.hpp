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

#ifndef COLDSPOT_JVM_JAVA_JAVALANGSTRING_HPP_
#define COLDSPOT_JVM_JAVA_JAVALANGSTRING_HPP_

#include <jvm/jdk/Global.hpp>
#include <jvm/Error.hpp>
#include <jvm/Object.hpp>
#include <jvm/common/UTF16String.hpp>

namespace coldspot
{

    class Object;

    class java_lang_String
    {
    public:

        static error_t new_(const UTF16String &utf16String, Object **string);

        static error_t intern(const UTF16String &utf16_string, Object **string);

        static error_t length(Object *string, jint *length);

        static error_t chars(Object *string, jchar **chars, jint *length);
    };

}

#endif

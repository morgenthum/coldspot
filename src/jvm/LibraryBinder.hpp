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

#ifndef COLDSPOT_JVM_LIBRARYBINDER_HPP_
#define COLDSPOT_JVM_LIBRARYBINDER_HPP_

#include <jvm/common/HashMap.hpp>
#include <jvm/common/String.hpp>
#include <jvm/jdk/Global.hpp>
#include <jvm/system/NativeTypes.hpp>

namespace coldspot
{

    class LibraryBinder
    {
    public:

        ~LibraryBinder();

        // Returns a pointer to the function with the specified name.
        // All libraries are searched for the function-name.
        Function_t get_function(const String &functionName);

        // Loads the library from the specified path.
        Library_t load_lib(const String &filePath);

    private:

        using OnLoadFunction = jint (*)(JavaVM *, void *);
        using OnUnloadFunction = void (*)(JavaVM *, void *);

        HashMap <String, Library_t> _libraries;
        HashMap <String, Function_t> _functions;

        // Calls the "JNI_OnLoad" function if it exists in the library.
        void init_lib(Library_t library);

        // Calls the "JNI_OnUnload" function if it exists in the library.
        void release_lib(Library_t library);
    };

}

#endif

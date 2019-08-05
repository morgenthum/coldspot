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

    LibraryBinder::~LibraryBinder()
    {
        auto begin = _libraries.begin();
        auto end = _libraries.end();

        while (begin != end)
        {
            Library_t library = begin->value;

            release_lib(library);
            System::releaseLibrary(library);

            ++begin;
        }
    }


    Function_t LibraryBinder::get_function(const String &functionName)
    {
        Function_t function = 0;

        auto entry = _functions.get(functionName);
        if (entry == 0)
        {
            auto begin = _libraries.begin();
            auto end = _libraries.end();

            while (begin != end)
            {
                Library_t library = begin->value;

                function = System::getFunction(library, functionName);
                if (function)
                {
                    _functions.put(functionName, function);
                    break;
                }

                ++begin;
            }
        }
        else
        {
            function = entry->value;
        }

        return function;
    }


    Library_t LibraryBinder::load_lib(const String &filePath)
    {
        Library_t library = 0;

        auto entry = _libraries.get(filePath);
        if (entry == 0)
        {
            library = System::loadLibrary(filePath);
            if (library)
            {
                _libraries.put(filePath, library);
                init_lib(library);
            }
        }
        else
        {
            library = entry->value;
        }

        return library;
    }


    void LibraryBinder::init_lib(Library_t library)
    {
        OnLoadFunction function = (OnLoadFunction) System::getFunction(library,
            FUNCNAME_JNIONLOAD);
        if (function != 0)
        {
            jint jni_version = function(_vm->vm_interface(), 0);
            if (!_vm->jni_version_supported(jni_version))
            {
                EXIT_FATAL("unsupported jni version");
            }
        }
    }


    void LibraryBinder::release_lib(Library_t library)
    {
        OnUnloadFunction function = (OnUnloadFunction) System::getFunction(
            library, FUNCNAME_JNIONUNLOAD);
        if (function != 0)
        {
            function(_vm->vm_interface(), 0);
        }
    }
}

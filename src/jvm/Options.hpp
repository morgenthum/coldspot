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

#ifndef COLDSPOT_JVM_OPTIONS_HPP_
#define COLDSPOT_JVM_OPTIONS_HPP_

#include <jvm/common/HashMap.hpp>
#include <jvm/common/String.hpp>

#define IS_VERBOSE(module)  _vm->options()->verbose##module

namespace coldspot
{

    // Global options used by the vm.
    class Options
    {
    public:

        HashMap <String, String> systemProperties;
        bool verboseClass;
        bool verboseGC;
        bool verboseExecute;
        bool verboseJNI;
        bool verboseDebug;

        Options() : verboseClass(false), verboseGC(false),
                    verboseExecute(false), verboseJNI(false),
                    verboseDebug(false)
        {
        }

        // Sets a property if it is not already set.
        void set_property(const String &key, const String &value)
        {
            if (systemProperties.get(key) == 0)
            {
                systemProperties.put(key, value);
            }
        }
    };

}

#endif

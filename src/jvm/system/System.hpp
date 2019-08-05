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

#ifndef COLDSPOT_JVM_SYSTEM_SYSTEM_HPP_
#define COLDSPOT_JVM_SYSTEM_SYSTEM_HPP_

#include <jvm/common/String.hpp>
#include <jvm/jdk/Global.hpp>
#include <jvm/system/NativeTypes.hpp>

namespace coldspot
{

    class System
    {
    public:

        static String architecture();

        static Thread_t currentThread();

        static String environmentVariable(const String &variable);

        static Function_t getFunction(Library_t library, const String &name);

        static Library_t loadLibrary(const String &path);

        static jlong millis();

        static String name();

        static void releaseLibrary(Library_t library);

        static void sleep(jlong ms);

        static String userhome();

        static String username();

        static String version();

        static String workingDirectory();

        static void createThread(void *function, void *parameter);

        static void stopThread(Thread_t thread);

        static void yield();

        static void join(Thread_t thread);
    };

}

#endif

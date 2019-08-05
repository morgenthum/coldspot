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

#ifndef COLDSPOT_JVM_OPENJDKHANDLER_HPP_
#define COLDSPOT_JVM_OPENJDKHANDLER_HPP_

#include <jvm/JDKHandler.hpp>

namespace coldspot
{

    class OpenJDKHandler : public JDKHandler
    {
    public:

        error_t initialize() override;

        error_t release() override;

        List<void *> &library() override;

    private:

        List<void *> _javaLibrary;

        // Loads the library to bind vm-specific methods.
        void loadBindingLibrary();

        // Creates a new vm-thread, attaches it to the current
        // native-thread and binds a new java-thread to it.
        error_t createInitialThread();

        // Sets jdk-specific system-properties.
        void setupProperties();

        // Registers native methods for
        // sun.misc.Unsafe
        void registerMethods();
    };

}

#endif

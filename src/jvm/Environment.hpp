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

#ifndef COLDSPOT_JVM_ENVIRONMENT_HPP_
#define COLDSPOT_JVM_ENVIRONMENT_HPP_

// Operating system
#if defined(__APPLE__)
    #define OS_POSIX
    #define OS_MAC
#elif defined(__linux__)
    #define OS_POSIX
    #define OS_LINUX
#elif defined(__WIN32)
    #define OS_WINDOWS
#elif defined(__CYGWIN__)
    #define OS_POSIX
    #define OS_WINDOWS
#elif defined(_GHOST_)
    #define OS_GHOST
#else
    #error "Unsupported operating system"
#endif

// JDK
#define JDK_OPENJDK

#endif

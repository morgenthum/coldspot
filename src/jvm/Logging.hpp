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

#ifndef COLDSPOT_JVM_LOGGING_HPP_
#define COLDSPOT_JVM_LOGGING_HPP_

#include <iostream>

#include <jvm/Options.hpp>

#define LOG_LEVEL_FATAL       4
#define LOG_LEVEL_ERROR       3
#define LOG_LEVEL_WARN        2
#define LOG_LEVEL_INFO        1
#define LOG_LEVEL_DEBUG       0

#ifndef LOG_LEVEL
    #define LOG_LEVEL           LOG_LEVEL_INFO
#endif

#define IS_LOG_LEVEL_ERROR    LOG_LEVEL <= LOG_LEVEL_ERROR
#define IS_LOG_LEVEL_WARN     LOG_LEVEL <= LOG_LEVEL_WARN
#define IS_LOG_LEVEL_INFO     LOG_LEVEL <= LOG_LEVEL_INFO
#define IS_LOG_LEVEL_DEBUG    LOG_LEVEL <= LOG_LEVEL_DEBUG

// Debug
#if IS_LOG_LEVEL_DEBUG
    #define LOG_DEBUG(msg...)                   std::cout << "DEBUG: " << msg << std::endl;
    #define LOG_DEBUG_VERBOSE(module, msg...)   if (IS_VERBOSE(module)) LOG_DEBUG(msg)
#else
    #define LOG_DEBUG(msg...)
    #define LOG_DEBUG_VERBOSE(module, msg...)
#endif

// Info
#if IS_LOG_LEVEL_INFO
    #define LOG_INFO(msg...)                    std::cout << "INFO: " << msg << std::endl;
    #define LOG_INFO_VERBOSE(module, msg...)    if (IS_VERBOSE(module)) LOG_INFO(msg)
#else
    #define LOG_INFO(msg...)
    #define LOG_INFO_VERBOSE(module, msg...)
#endif

// Warning
#if IS_LOG_LEVEL_WARN
    #define LOG_WARN(msg...)                    std::cout << "WARN: " << msg << std::endl;
#else
    #define LOG_WARN(msg...)
#endif

// Error
#if IS_LOG_LEVEL_ERROR
    #define LOG_ERROR(msg...)                   std::cerr << "ERROR: " << msg << std::endl;
#else
    #define LOG_ERROR(msg...)
#endif

// Fatal
#define EXIT_FATAL(msg...)                     std::cerr << "FATAL: " << msg << std::endl; std::exit(1);

#endif

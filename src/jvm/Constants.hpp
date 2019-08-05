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

#ifndef COLDSPOT_JVM_CONSTANTS_HPP_
#define COLDSPOT_JVM_CONSTANTS_HPP_

namespace coldspot
{

    // Basics
    extern const char *CLASSNAME_CLASS;
    extern const char *CLASSNAME_CLONEABLE;
    extern const char *CLASSNAME_FIELD;
    extern const char *CLASSNAME_OBJECT;
    extern const char *CLASSNAME_SERIALIZABLE;
    extern const char *CLASSNAME_STRING;
    extern const char *CLASSNAME_SYSTEM;
    extern const char *CLASSNAME_THREAD;
    extern const char *CLASSNAME_THREADGROUP;

    // Primitive wrappers
    extern const char *CLASSNAME_BOOLEAN;
    extern const char *CLASSNAME_BYTE;
    extern const char *CLASSNAME_CHARACTER;
    extern const char *CLASSNAME_SHORT;
    extern const char *CLASSNAME_INTEGER;
    extern const char *CLASSNAME_FLOAT;
    extern const char *CLASSNAME_LONG;
    extern const char *CLASSNAME_DOUBLE;

    // Errors
    extern const char *CLASSNAME_ABSTRACTMETHODERROR;
    extern const char *CLASSNAME_INCOMPATIBLECLASSCHANGEERROR;
    extern const char *CLASSNAME_INSTANTIATIONERROR;
    extern const char *CLASSNAME_LINKAGEERROR;
    extern const char *CLASSNAME_NOCLASSDEFFOUNDERROR;
    extern const char *CLASSNAME_NOSUCHFIELDERROR;
    extern const char *CLASSNAME_NOSUCHMETHODERROR;
    extern const char *CLASSNAME_OUTOFMEMORYERROR;
    extern const char *CLASSNAME_STACKOVERFLOWERROR;
    extern const char *CLASSNAME_UNSATISFIEDLINKERROR;

    // Exceptions
    extern const char *CLASSNAME_ARITHMETICEXCEPTION;
    extern const char *CLASSNAME_ARRAYINDEXOUTOFBOUNDSEXCEPTION;
    extern const char *CLASSNAME_CLASSCASTEXCEPTION;
    extern const char *CLASSNAME_CLASSNOTFOUNDEXCEPTION;
    extern const char *CLASSNAME_ILLEGALARGUMENTEXCEPTION;
    extern const char *CLASSNAME_ILLEGALMONITORSTATEEXCEPTION;
    extern const char *CLASSNAME_NEGATIVEARRAYSIZEEXCEPTION;
    extern const char *CLASSNAME_NULLPOINTEREXCEPTION;

    // Method names
    extern const char *METHODNAME_CONSTRUCTOR;
    extern const char *METHODNAME_STATICINIT;

    // Function names
    extern const char *FUNCNAME_JNIONLOAD;
    extern const char *FUNCNAME_JNIONUNLOAD;

}

#endif

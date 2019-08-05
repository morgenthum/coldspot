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

#ifndef COLDSPOT_JVM_ERROR_HPP_
#define COLDSPOT_JVM_ERROR_HPP_

#define RETURN_OK          0
#define RETURN_ERROR      -1
#define RETURN_EXCEPTION  -2

#define RETURN_VALUE_ON_FAIL(errorValue, returnValue) \
  if (errorValue != RETURN_OK) { \
    return returnValue; \
  }

#define RETURN_ON_FAIL(errorValue) \
  RETURN_VALUE_ON_FAIL(errorValue, errorValue)

#define RETURN_ON_FAIL_UNLOCK(errorValue, mutex) \
  if (errorValue != RETURN_OK) { \
    mutex.unlock(); \
    return errorValue; \
  }

#define RETURN_ON_SUCCESS(errorValue) \
  if (errorValue == RETURN_OK) { \
    return errorValue; \
  }

#define RETURN_UNLOCK(errorValue, mutex) \
  mutex.unlock(); \
  return errorValue;

using error_t = int;

#endif

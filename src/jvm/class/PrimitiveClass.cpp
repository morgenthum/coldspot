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

#define SPECIALIZATION(type, descriptor) \
  template <> \
  Class* PrimitiveClass<type>::get() { \
    if (_clazz == 0) { \
      _vm->class_loader()->load_primitive(descriptor, &_clazz); \
    } \
    return _clazz; \
  }

namespace coldspot
{

    template<typename T> Class *PrimitiveClass<T>::_clazz = 0;

    SPECIALIZATION(void, "V")
    SPECIALIZATION(jboolean, "Z")
    SPECIALIZATION(jbyte, "B")
    SPECIALIZATION(jchar, "C")
    SPECIALIZATION(jshort, "S")
    SPECIALIZATION(jint, "I")
    SPECIALIZATION(jfloat, "F")
    SPECIALIZATION(jlong, "J")
    SPECIALIZATION(jdouble, "D")

}

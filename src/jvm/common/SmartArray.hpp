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

#ifndef COLDSPOT_JVM_COMMON_SMARTARRAY_HPP_
#define COLDSPOT_JVM_COMMON_SMARTARRAY_HPP_

#include "Memory.hpp"

namespace coldspot
{

    template<typename ELEMENT_T, typename SIZE_T>
    class SmartArray
    {
    public:

        SmartArray() : _data(0), _size(0)
        {
        }

        SmartArray(const SmartArray &other) = delete;

        ~SmartArray()
        {
            DELETE_ARRAY(_data)
        }

        SmartArray &operator=(const SmartArray &rhs) = delete;

        ELEMENT_T &operator[](SIZE_T index)
        {
            return _data[index];
        }

        void init(SIZE_T size)
        {
            _data = new ELEMENT_T[_size = size]();
        }

        SIZE_T length() const
        {
            return _size;
        }

    private:

        ELEMENT_T *_data;
        SIZE_T _size;
    };

}

#endif

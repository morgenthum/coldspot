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

#ifndef COLDSPOT_JVM_COMMON_LOCAL_HPP_
#define COLDSPOT_JVM_COMMON_LOCAL_HPP_

#include "Memory.hpp"

namespace coldspot
{

    /**
     *
     */
    template<typename T>
    class local
    {
    public:

        local(T *original = 0) : _original(original)
        {
        }

        ~local()
        {
            DELETE_OBJECT(_original);
        }

        local(const local &rhs) = delete;
        local &operator=(const local &rhs) = delete;

        T *operator->() const { return _original; }

        T *get() const { return _original; }
        void set(T *original) { _original = original; }

        T *release()
        {
            T *temp = _original;
            _original = 0;
            return temp;
        }

    private:

        T *_original;
    };

}

#endif

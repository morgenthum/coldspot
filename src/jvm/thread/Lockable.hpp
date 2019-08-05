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

#ifndef COLDSPOT_JVM_COMMON_LOCKABLE_HPP_
#define COLDSPOT_JVM_COMMON_LOCKABLE_HPP_

#include <jvm/thread/Mutex.hpp>

namespace coldspot
{

    template<typename T>
    class Lockable
    {
    public:

        Lockable() = default;

        Lockable(const Lockable &rhs) = delete;
        Lockable &operator=(const Lockable &rhs) = delete;

        T &operator*() { return _locked; }
        T *operator->() { return &_locked; }

        void lock() { _mutex.lock(); }
        void unlock() { _mutex.unlock(); }

    private:

        Mutex _mutex;

        T _locked;
    };

}

#endif

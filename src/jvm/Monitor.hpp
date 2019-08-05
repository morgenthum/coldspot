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

#ifndef COLDSPOT_JVM_MONITOR_HPP_
#define COLDSPOT_JVM_MONITOR_HPP_

#include <jvm/jni/Types.hpp>
#include <jvm/thread/Condition.hpp>
#include <jvm/thread/Mutex.hpp>
#include <jvm/Error.hpp>

namespace coldspot
{

    class Thread;

    // The monitor is used to synchronize an object.
    class Monitor
    {
    public:

        Monitor() : _owner(0)
        {
        }

        // Enters the monitor.
        void enter();

        bool try_enter();

        // Exits the monitor.
        error_t exit();

        // Notifies one waiting thread.
        error_t notify();

        // Notifies all waiting threads.
        error_t notify_all();

        // Causes the current thread to wait until another thread
        // invokes the notify or notifyAll method.
        error_t wait(jlong ms);

    private:

        Mutex _mutex;
        Condition _condition;
        Thread *_owner;
    };

}

#endif

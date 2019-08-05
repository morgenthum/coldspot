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

#ifndef COLDSPOT_JVM_THREAD_CONDITION_HPP_
#define COLDSPOT_JVM_THREAD_CONDITION_HPP_

#include <jvm/jdk/Global.hpp>

namespace coldspot
{

    class Mutex;

    // A condition enables a thread to wait for another thread
    // and notifies all waiting threads.
    class Condition
    {
    public:

        Condition();
        ~Condition();

        // Notifies a single thread that is waiting.
        void notify();

        // Notifies all waiting threads.
        void notify_all();

        // Causes the current thread to wait until
        // - the next owner notifies
        // - or the time has elapsed
        void wait(Mutex &mutex, jlong timeout = 0);

        // Getters.
        bool wait_requested() const { return _wait_requested; }

        // Setters.
        void set_wait_requested(bool requested) { _wait_requested = requested; }

    private:

        class ConditionImpl;

        ConditionImpl *_impl;

        bool _wait_requested; // TODO move
    };

}

#endif

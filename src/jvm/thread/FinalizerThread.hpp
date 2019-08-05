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

#ifndef COLDSPOT_JVM_THREAD_FINALIZERTHREAD_HPP_
#define COLDSPOT_JVM_THREAD_FINALIZERTHREAD_HPP_

#include "VMThread.hpp"

namespace coldspot
{

    class Finalizer;
    class Object;

    class FinalizerThread : public VMThread
    {
    public:

        FinalizerThread() : VMThread(THREADTYPE_FINALIZER, THREADSTATE_NEW),
                            _running(true)
        {
            set_daemon(true);
        }

        virtual ~FinalizerThread();

        void run() override;

        // Getters.
        Finalizer *finalizer() const { return _finalizer; }

        // Setters.
        void set_running(bool running) { _running = running; }

    private:

        bool _running;

        Finalizer *_finalizer;
    };

}

#endif

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

#ifndef COLDSPOT_JVM_THREAD_GCTHREAD_HPP_
#define COLDSPOT_JVM_THREAD_GCTHREAD_HPP_

#include "Thread.hpp"

namespace coldspot
{

    class GCThread : public Thread
    {
    public:

        GCThread() : Thread(THREADTYPE_GC), _running(true)
        {

            set_daemon(true);
        }

        void run() override;

        // Setters.
        void set_running(bool running) { _running = running; }

    private:

        bool _running;
    };

}

#endif

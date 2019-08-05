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

namespace coldspot
{

    FinalizerThread::~FinalizerThread()
    {
        DELETE_OBJECT(_finalizer)
    }


    void FinalizerThread::run()
    {
        _finalizer = new SimpleFinalizer;

        // Cancel the execution if the vm is shutting down
        while (_running)
        {
            // Set waiting
            set_state(THREADSTATE_WAITING);

            // Idle while there is nothing to do
            while (_running && _finalizer->in_objects()->empty())
            {
                System::sleep(10);
            }

            // Set runnable
            set_state(THREADSTATE_RUNNABLE);

            _finalizer->finalize();
        }

        // Remove from thread list manually
        auto &threads = _vm->threads();
        threads.lock();
        threads->erase(threads->find(this));
        threads.unlock();
    }

}

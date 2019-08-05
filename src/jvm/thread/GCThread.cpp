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

    void GCThread::run()
    {

        SimpleGarbageCollector gc;

        auto &objects = _vm->memory_manager()->get_objects();

        // Cancel the execution if the vm is shutting down
        while (_running)
        {
            uint32_t lastSize = objects->size();

            // Set waiting
            set_state(THREADSTATE_WAITING);

            // Idle while there is nothing to do
            for (; ;)
            {
                System::sleep(250);

                // Run gc if the vm is exiting
                if (!_running)
                {
                    break;
                }

                // Run if there are more than 100 objects to gc
                if (objects->size() - lastSize > 100)
                {
                    break;
                }
            }

            set_state(THREADSTATE_RUNNABLE);

            jlong startMillis = System::millis();
            gc.collectGarbage();
            jint neededMillis = (jint)(System::millis() - startMillis);

            LOG_DEBUG_VERBOSE(GC, "cycle needed: " << neededMillis << " ms")
        }

        gc.collectGarbageForExit();

        // Remove from thread list manually
        auto &threads = _vm->threads();
        threads.lock();
        threads->erase(threads->find(this));
        threads.unlock();
    }

}

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

    __thread Thread *_current_thread;


    void *threadStart(void *parameter)
    {
#ifdef OS_POSIX
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
#endif

        Thread *thread = (Thread *) parameter;

        _vm->attach_thread(thread, thread->is_daemon());
        thread->run();
        _vm->detach_thread();

        return 0;
    }


    void Thread::attach_native()
    {
        _native_thread = System::currentThread();
        _current_thread = this;
    }


    void Thread::detach_native()
    {
        _native_thread = 0;
        _current_thread = 0;
    }


    void Thread::join() const
    {
        System::join(_native_thread);
    }


    void Thread::start(bool daemon)
    {
        // Set daemon
        set_daemon(daemon);

        // Create thread
        System::createThread((void *) &threadStart, (void *) this);

        // Wait until the thread was started
        while (_state == THREADSTATE_NEW)
        {
            System::yield();
        }
    }


    void Thread::stop()
    {
        System::stopThread(_native_thread);
        System::join(_native_thread);

        _vm->detach_thread(this);
    }

}

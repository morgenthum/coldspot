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

#ifndef COLDSPOT_JVM_THREAD_THREAD_HPP_
#define COLDSPOT_JVM_THREAD_THREAD_HPP_

#include <jvm/system/NativeTypes.hpp>

#include "Condition.hpp"
#include "Mutex.hpp"

#define THREAD_BLOCK \
  _current_thread->block_mutex().lock();

#define THREAD_UNBLOCK \
  _current_thread->block_mutex().unlock();

#define SAFEPOINT \
  while (_current_thread->wait_condition().wait_requested()) { \
    _current_thread->wait_condition().wait(_current_thread->wait_mutex()); \
  }

namespace coldspot
{

    enum ThreadType
    {
        THREADTYPE_FINALIZER, THREADTYPE_GC, THREADTYPE_VM
    };

    enum ThreadState
    {
        THREADSTATE_NEW,
        THREADSTATE_RUNNABLE,
        THREADSTATE_BLOCKED,
        THREADSTATE_WAITING,
        THREADSTATE_TIMED_WAITING,
        THREADSTATE_TERMINATED
    };

    class Thread
    {
    public:


        Thread(ThreadType type) : _type(type), _state(THREADSTATE_NEW),
                                  _native_thread(0), _daemon(false) { }
        Thread(ThreadType type, ThreadState state) : _type(type), _state(state),
                                                     _native_thread(0),
                                                     _daemon(false) { }
        virtual ~Thread() { }

        // Executes the thread.
        virtual void run() = 0;

        // Connect or disconnect the current native thread from this thread.
        virtual void attach_native();
        virtual void detach_native();

        // Waits until the thread is terminated.
        void join() const;

        bool is_alive() const
        {
            return _state != THREADSTATE_NEW &&
                   _state != THREADSTATE_TERMINATED;
        }

        // Getters.
        ThreadType type() const { return _type; }
        ThreadState state() const { return _state; }
        Mutex &block_mutex() { return _block_mutex; }
        Mutex &wait_mutex() { return _wait_mutex; }
        Condition &wait_condition() { return _wait_condition; }
        bool is_daemon() const { return _daemon; }

        // Setters.
        void set_state(ThreadState state) { _state = state; }
        void set_daemon(bool daemon) { _daemon = daemon; }

    private:

        ThreadType _type;
        ThreadState _state;
        Thread_t _native_thread;

        Mutex _block_mutex;
        Mutex _wait_mutex;
        Condition _wait_condition;

        bool _daemon;

    public:

        // Creates a new native thread and executes the run-method.
        void start(
            bool daemon); // TODO remove parameter and call setDaemon instead

        // Stops the exeuction of this thread.
        void stop();
    };

    extern __thread Thread *_current_thread;

}

#endif

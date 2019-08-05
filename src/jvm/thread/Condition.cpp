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

    class Condition::ConditionImpl
    {
    public:
#ifdef OS_GHOST
        // TODO
#else
        pthread_cond_t condition;
#endif
    };


    Condition::Condition() : _wait_requested(false)
    {
#ifdef OS_GHOST
        // TODO
#else
        _impl = new ConditionImpl;

        pthread_cond_init(&_impl->condition, 0);
#endif
    }


    Condition::~Condition()
    {
#ifdef OS_GHOST
        // TODO
#else
        pthread_cond_destroy(&_impl->condition);

        DELETE_OBJECT(_impl);
#endif
    }


    void Condition::notify()
    {
#ifdef OS_GHOST
        // TODO
#else
        pthread_cond_signal(&_impl->condition);
#endif
    }


    void Condition::notify_all()
    {
#ifdef OS_GHOST
        // TODO
#else
        pthread_cond_broadcast(&_impl->condition);
#endif
    }


    void Condition::wait(Mutex &mutex, jlong timeout)
    {
        if (_current_thread != 0)
        {
            _current_thread->set_state(THREADSTATE_WAITING);
        }

#ifdef OS_GHOST
        // TODO
#else
        pthread_mutex_t *mutexHandle = static_cast<pthread_mutex_t *>(mutex.native_handle_ptr());
        if (timeout == 0)
        {
            pthread_cond_wait(&_impl->condition, mutexHandle);
        }
        else
        {
            pthread_cond_wait(&_impl->condition, mutexHandle); // TODO
        }
#endif

        if (_current_thread != 0)
        {
            _current_thread->set_state(THREADSTATE_RUNNABLE);
        }
    }

}

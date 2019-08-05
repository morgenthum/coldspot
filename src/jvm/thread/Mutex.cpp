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

    class Mutex::MutexImpl
    {
    public:
        pthread_mutexattr_t attributes;
        pthread_mutex_t mutex;
    };


    Mutex::Mutex()
    {
        _impl = new MutexImpl;

        // TODO error handling
        pthread_mutexattr_init(&_impl->attributes);
        pthread_mutexattr_settype(&_impl->attributes, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&_impl->mutex, &_impl->attributes);
    }


    Mutex::~Mutex()
    {
        pthread_mutexattr_destroy(&_impl->attributes);
        pthread_mutex_destroy(&_impl->mutex);

        DELETE_OBJECT(_impl)
    }


    bool Mutex::try_lock()
    {
        return pthread_mutex_trylock(&_impl->mutex) == 0;
    }


    void Mutex::lock()
    {
        if (_current_thread != 0)
        {
            _current_thread->set_state(THREADSTATE_BLOCKED);
        }

        int error;
        if ((error = pthread_mutex_lock(&_impl->mutex)) != 0)
        {
            EXIT_FATAL("pthread_mutex_lock failed: " << error)
        }

        if (_current_thread != 0)
        {
            _current_thread->set_state(THREADSTATE_RUNNABLE);
        }
    }


    void Mutex::unlock()
    {
        int error;
        if ((error = pthread_mutex_unlock(&_impl->mutex)) != 0)
        {
            EXIT_FATAL("pthread_mutex_unlock failed: " << error)
        }
    }


    void *Mutex::native_handle_ptr() const
    {
        return &_impl->mutex;
    }

}

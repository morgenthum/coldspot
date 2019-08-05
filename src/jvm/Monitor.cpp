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

    void Monitor::enter()
    {
        _mutex.lock();

        _owner = _current_thread;
    }


    bool Monitor::try_enter()
    {
        bool success = _mutex.try_lock();
        if (success)
        {
            _owner = _current_thread;
        }

        return success;
    }


    error_t Monitor::exit()
    {
        if (_current_thread != _owner)
        {
            _current_executor->throw_exception(
                CLASSNAME_ILLEGALMONITORSTATEEXCEPTION);
            return RETURN_EXCEPTION;
        }

        _mutex.unlock();

        return RETURN_OK;
    }


    error_t Monitor::notify()
    {
        if (_current_thread != _owner)
        {
            _current_executor->throw_exception(
                CLASSNAME_ILLEGALMONITORSTATEEXCEPTION);
            return RETURN_EXCEPTION;
        }

        _condition.notify();

        return RETURN_OK;
    }


    error_t Monitor::notify_all()
    {
        if (_current_thread != _owner)
        {
            _current_executor->throw_exception(
                CLASSNAME_ILLEGALMONITORSTATEEXCEPTION);
            return RETURN_EXCEPTION;
        }

        _condition.notify_all();

        return RETURN_OK;
    }


    error_t Monitor::wait(jlong ms)
    {
        if (_current_thread != _owner)
        {
            _current_executor->throw_exception(
                CLASSNAME_ILLEGALMONITORSTATEEXCEPTION);
            return RETURN_EXCEPTION;
        }

        if (ms < 0)
        {
            _current_executor->throw_exception(
                CLASSNAME_ILLEGALARGUMENTEXCEPTION);
            return RETURN_EXCEPTION;
        }

        // TODO InterruptedException

        _condition.wait(_mutex);

        _owner = _current_thread;

        return RETURN_OK;
    }

}

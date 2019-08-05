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

    HashMap<Object *, VMThread *> VMThread::_object_mapping;

    VMThread *VMThread::from_object(Object *threadObject)
    {
        auto entry = _object_mapping.get(threadObject);
        if (entry != 0)
        {
            return entry->value;
        }

        return 0;
    }


    VMThread::VMThread(ThreadState state) : Thread(THREADTYPE_VM, state),
                                            _object(0), _invoke_method(0),
                                            _invoke_object(0)
    {
        _executor = new Interpreter;
    }


    VMThread::VMThread(ThreadType type, ThreadState state) : Thread(type,
        state), _object(0), _invoke_method(0), _invoke_object(0)
    {
        _executor = new Interpreter;
    }


    VMThread::~VMThread()
    {
        _object_mapping.remove(_object);
        DELETE_OBJECT(_executor);
    }


    void VMThread::run()
    {
        if (_invoke_method != 0)
        {
            Value value;
            _invoke_method->invoke(_invoke_object, 0, &value);
        }
    }


    void VMThread::attach_native()
    {
        Thread::attach_native();
        _current_executor = _executor;
    }


    void VMThread::detach_native()
    {
        Thread::detach_native();
        _current_executor = 0;
    }


    void VMThread::bind(Object *object)
    {
        _object = object;
        _object_mapping.put(_object, this);
    }


    void VMThread::start(bool daemon, Method *method, Object *object)
    {
        _invoke_method = method;
        _invoke_object = object;

        Thread::start(daemon);
    }

}

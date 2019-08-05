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

#ifndef COLDSPOT_JVM_THREAD_VMTHREAD_HPP_
#define COLDSPOT_JVM_THREAD_VMTHREAD_HPP_

#include <jvm/common/HashMap.hpp>

#include "Thread.hpp"

namespace coldspot
{

    class Executor;

    class Method;

    class Object;

    // Represents a thread that executes code.
    class VMThread : public Thread
    {
    public:

        static VMThread *from_object(Object *threadObject);

        VMThread(ThreadState state);
        VMThread(ThreadType type, ThreadState state);
        virtual ~VMThread();

        virtual void run() override;

        virtual void attach_native() override;
        virtual void detach_native() override;

        // Binds the thread-object to this thread.
        void bind(Object *object);

        // Creates a new native thread and executes the method.
        void start(bool daemon, Method *method, Object *object);

        // Getters.
        Executor *executor() const { return _executor; }
        Object *object() const { return _object; }

    private:

        static HashMap<Object *, VMThread *> _object_mapping;

        Executor *_executor;
        Object *_object;

        Method *_invoke_method;
        Object *_invoke_object;
    };

}

#endif

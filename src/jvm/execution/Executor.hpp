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

#ifndef COLDSPOT_JVM_EXECUTION_EXECUTOR_HPP_
#define COLDSPOT_JVM_EXECUTION_EXECUTOR_HPP_

#include <cstdint>

#include <jvm/common/dynamic_stack.hpp>
#include <jvm/Error.hpp>

namespace coldspot
{

    class Array;
    class Class;
    class Frame;
    class Object;
    class Thread;
    class Value;
    class VMThread;

    class Executor
    {
    public:

        // Initializes the executor and a frame-stack.
        Executor();

        virtual ~Executor()
        {
        }

        // Executes a frame and returns the return-value.
        virtual error_t execute(Frame *initialFrame, Value *returnValue) = 0;

        // Checks in the top frame, if there are no frames,
        // it checks if there is an uncaught exception.
        Object *occured_exception();

        // Calls the printStackTrace-method of the exception.
        error_t describe_exception(Object *exception);

        // Clears the top frame or the uncaught exception.
        void clear_exception();

        // Throws the exception.
        // Searches for the next exception handler
        // and invalidates all unwinded frames.
        error_t throw_exception(Object *exception);

        // Builds and throws an exception.
        error_t throw_exception(const char *exceptionName,
            const char *message = 0);

        // Getters.
        dynamic_stack &frames() { return _frames; }
        Object *uncaught_exception() const { return _uncaught_exception; }

    protected:

        dynamic_stack _frames;

        Object *_uncaught_exception;

        // TODO remove
        error_t createMultiArray(Class *clazz, const fixed_stack <jint> &sizes,
            jint dimension, jint dimensionSize, Array **array);
    };

    extern __thread Executor *_current_executor;

}

#endif

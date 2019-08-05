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

    __thread Executor *_current_executor = 0;

    Executor::Executor() : _uncaught_exception(0)
    {
        _frames.init(JAVA_STACK_SIZE);
    }


    Object *Executor::occured_exception()
    {
        if (_frames.empty())
        {
            return _uncaught_exception;
        }

        return ((Frame *) _frames.peek())->exception;
    }


    error_t Executor::describe_exception(Object *exception)
    {
        Method *print_method;
        error_t error_value = exception->type()->get_method(
            Signature("()V", "printStackTrace"), &print_method);
        RETURN_ON_FAIL(error_value)

        error_value = print_method->invoke(exception, 0, 0);
        RETURN_ON_FAIL(error_value)

        return RETURN_OK;
    }


    void Executor::clear_exception()
    {
        if (_frames.empty())
        {
            _uncaught_exception = 0;
        }
        else
        {
            ((Frame *) _frames.peek())->exception = 0;
        }
    }


    error_t Executor::throw_exception(Object *exception)
    {
        // Throw a NullPointerException if the exception to throw is null
        if (exception == 0)
        {
            return throw_exception(CLASSNAME_NULLPOINTEREXCEPTION);
        }

        Object *message;
        java_lang_Throwable::detailMessage(exception, &message);

        jchar *chars;
        jint length;
        java_lang_String::chars(message, &chars, &length);

        // Logging
        LOG_DEBUG_VERBOSE(Debug, "throw " << exception->type()->name.c_str())

        // Search for an exception-handler by stack-unwinding
        for (; ;)
        {
            Frame *frame = (Frame *) _frames.peek();

            // Unwind the stack until a native frame is on the top,
            // maybe the native method will handle the exception itself
            if (frame->type == FRAMETYPE_NATIVE)
            {
                frame->exception = exception;
                return RETURN_OK;
            }

            // Program counter of the current frame
            uint32_t pc = CURRENT_PC(frame);

            // Search for exception-handler
            for (auto &handler : frame->method->exception_handlers())
            {
                // Check if the range of the handler matches the current program-counter
                bool matches_range =
                    pc >= handler->startPc && pc <= handler->endPc;

                // Check if the handler can handle the exception-type,
                // if no catch-class is set, it is a finally-block
                bool matches_type = handler->catchClass == 0 ||
                                    handler->catchClass == exception->type() ||
                                    exception->type()->is_subclass_of(
                                        handler->catchClass);

                // If the range and type matches, prepare the frame to execute the handler
                if (matches_range && matches_type)
                {
                    frame->currentCode = &frame->method->code()[handler->handlerPc];
                    frame->operandsCount = 0;
                    frame->push(exception);
                    return RETURN_OK;
                }
            }

            // If we get here, the current method can not handle the exception ...

            // Exit monitor if the method we are unwinding is synchronized
            if (frame->method->is_synchronized())
            {
                frame->callee()->ensure_monitor()->exit();
            }

            // Invalidate frame
            frame->valid = false;

            // Pop frame from stack
            _frames.pop();

            // If the thread has no more frames, set the exception as uncaught
            if (_frames.empty())
            {
                _uncaught_exception = exception;
                return RETURN_EXCEPTION;
            }
        }

        // Should never be reached
        return RETURN_EXCEPTION;
    }


    error_t Executor::throw_exception(const char *exceptionName,
        const char *message)
    {
        Class *exceptionClass;
        error_t errorValue = _vm->class_loader()->load_class(exceptionName,
            &exceptionClass);
        RETURN_ON_FAIL(errorValue);

        errorValue = _vm->class_loader()->initialize_class(exceptionClass);
        RETURN_ON_FAIL(errorValue);

        Object *exception;

        if (message == 0)
        {
            Method *constructor;
            errorValue = exceptionClass->get_method(
                Signature("()V", METHODNAME_CONSTRUCTOR), &constructor);
            RETURN_ON_FAIL(errorValue);

            errorValue = Object::new_object(constructor, 0, &exception);
            RETURN_ON_FAIL(errorValue);
        }
        else
        {
            Method *constructor;
            errorValue = exceptionClass->get_method(
                Signature("(Ljava/lang/String;)V", METHODNAME_CONSTRUCTOR),
                &constructor);
            RETURN_ON_FAIL(errorValue);

            Object *messageString;
            errorValue = java_lang_String::new_(String(message),
                &messageString);
            RETURN_ON_FAIL(errorValue);

            Value messageStringParameter = messageString;
            errorValue = Object::new_object(constructor,
                &messageStringParameter, &exception);
            RETURN_ON_FAIL(errorValue);
        }

        return throw_exception(exception);
    }


    error_t Executor::createMultiArray(Class *clazz,
        const fixed_stack <jint> &sizes, jint dimension, jint dimensionSize,
        Array **array)
    {
        jint size = sizes.get(dimension);
        error_t errorValue = _vm->memory_manager()->allocate_array(clazz, size,
            array);
        RETURN_ON_FAIL(errorValue);

        Class *componentType = clazz->component_type;

        if (componentType->is_array())
        {
            for (jint i = 0; i < size; ++i)
            {
                Array *subArray;
                errorValue = createMultiArray(componentType, sizes,
                    dimension - 1, dimensionSize, &subArray);
                RETURN_ON_FAIL(errorValue);

                (*array)->set_value(i, subArray);
            }
        }

        return RETURN_OK;
    }

}

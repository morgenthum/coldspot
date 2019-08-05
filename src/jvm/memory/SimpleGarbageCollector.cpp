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

    void SimpleGarbageCollector::collectGarbage()
    {
        auto &objects = _vm->memory_manager()->get_objects();
        auto &threads = _vm->threads();

        // Prevent creating or deleting threads during gc
        threads.lock();

        // Suspend all threads
        _vm->suspend_vm_threads();

        // Mark all objects as unused
        for (auto object : *objects)
        {
            mark_unused(object);
        }

        // Mark all referenced objects as used
        for (auto thread : *threads)
        {
            // Don't analyze threads that are
            // not started yet or are already terminated
            if (thread->state() == THREADSTATE_NEW ||
                thread->state() == THREADSTATE_TERMINATED)
            {
                continue;
            }

            // Only vm-threads are relevant for gc
            if (thread->type() != THREADTYPE_VM &&
                thread->type() != THREADTYPE_FINALIZER)
            {
                continue;
            }

            VMThread *vmThread = static_cast<VMThread *>(thread);
            Executor *executor = vmThread->executor();

            // Check thread-object
            mark_used(vmThread->object());

            // Check uncaught exception
            mark_used(executor->uncaught_exception());

            // Check all frames
            auto &frames = executor->frames();
            auto frameIter = frames.begin();
            while (frameIter != frames.end())
            {
                Frame *frame = (Frame * ) * frameIter;

                // Check pending exception on frame
                mark_used(frame->exception);

                if (frame->type == FrameType::FRAMETYPE_JAVA)
                {

                    // Check local variables
                    auto &localVariables = frame->localVariables;
                    for (uint16_t j = 0; j < frame->method->locals_count(); ++j)
                    {
                        Value &value = localVariables[j];
                        if (value.type() == Type::TYPE_REFERENCE)
                        {
                            mark_used(value.as_object());
                        }
                    }

                    // Check operands
                    for (uint32_t j = 0; j < frame->operandsCount; ++j)
                    {
                        Value &operand = frame->operands[j];
                        if (operand.type() == Type::TYPE_REFERENCE)
                        {
                            mark_used(operand.as_object());
                        }
                    }

                }
                else if (frame->type == FrameType::FRAMETYPE_NATIVE)
                {
                    // Check local references
                    for (auto localReference : *(frame->localReferences))
                    {
                        mark_used(localReference);
                    }
                }

                ++frameIter;
            }
        }

        auto &loadedClasses = _vm->class_loader()->loaded_classes();

        // Mark all from the class referenced objects
        auto loadedClassesIterator = loadedClasses.begin();
        while (loadedClassesIterator != loadedClasses.end())
        {
            Class *clazz = loadedClassesIterator->value;

            // Mark class-object
            mark_used(clazz->object);

            // Mark class-loader
            mark_used(clazz->class_loader);

            // Mark static fields
            if (!clazz->is_array())
            {
                List < Field * > declared_fields;
                clazz->get_declared_fields(declared_fields);

                for (auto declared_field : declared_fields)
                {
                    if (declared_field->is_static() &&
                        !declared_field->type()->is_primitive())
                    {
                        mark_used(declared_field->get_static<jobject>());
                    }
                }
            }

            ++loadedClassesIterator;
        }

        // Mark string pool objects
        auto constantPoolIterator = _vm->string_pool().begin();
        while (constantPoolIterator != _vm->string_pool().end())
        {
            mark_used(constantPoolIterator->value);
            ++constantPoolIterator;
        }

        // Mark all local references
        for (auto localReference : *_vm->local_references())
        {
            mark_used(localReference);
        }

        // Mark all global references
        for (auto globalReference : *_vm->global_references())
        {
            mark_used(globalReference);
        }

        // Mark pre-allocated error-object
        mark_used(_vm->stack_overflow_error());

        // Delete threads that are terminated
        deleteTerminatedVMThreads();

        // Get finalizer-thread
        // and move unused objects from object-container to finalizer
        moveUnusedObjects(*objects,
            _vm->finalizer_thread()->finalizer()->in_objects());

        // Remove all finalized objects
        removeFinalizedObjects();

        // Resume all threads
        _vm->resume_vm_threads();

        threads.unlock();
    }


    void SimpleGarbageCollector::collectGarbageForExit()
    {

        deleteVMThreads();

        finalizeAllObjects();
    }


    void SimpleGarbageCollector::deleteTerminatedVMThreads()
    {

        auto &threads = _vm->threads();

        // Delete terminated threads
        auto iterator = threads->begin();

        while (iterator != threads->end())
        {
            Thread *thread = *iterator;

            if (thread->state() == THREADSTATE_TERMINATED)
            {
                bool deleteThread = true;

                if (thread->type() == THREADTYPE_VM)
                {
                    VMThread *vmThread = static_cast<VMThread *>(thread);

                    // We don't delete the thread if the java-object is still in use
                    if (vmThread->object()->used())
                    {
                        deleteThread = false;
                    }
                }

                if (deleteThread)
                {
                    DELETE_OBJECT(thread);
                    iterator = threads->erase(iterator);
                }
                else
                {
                    ++iterator;
                }
            }
            else
            {
                ++iterator;
            }
        }
    }


    void SimpleGarbageCollector::finalizeAllObjects()
    {

        auto &source = _vm->memory_manager()->get_objects();
        auto &target = _vm->finalizer_thread()->finalizer()->in_objects();

        target.lock();

        auto iterator = source->begin();

        while (iterator != source->end())
        {
            target->addBack(*iterator);
            iterator = source->erase(iterator);
        }

        target.unlock();
    }


    void SimpleGarbageCollector::deleteVMThreads()
    {

        auto &threads = _vm->threads();

        // Delete terminated threads
        auto iterator = threads->begin();

        while (iterator != threads->end())
        {
            Thread *thread = *iterator;

            if (thread->type() == THREADTYPE_VM)
            {
                DELETE_OBJECT(thread);
                iterator = threads->erase(iterator);
            }
            else
            {
                ++iterator;
            }
        }
    }


    void SimpleGarbageCollector::moveUnusedObjects(heap < Object * > &source,
        Lockable < List < Object * >> &target)
    {

        target.lock();

        auto iterator = source.begin();

        while (iterator != source.end())
        {
            Object *object = *iterator;

            if (!object->used())
            {
                target->addBack(object);
                iterator = source.erase(iterator);
            }
            else
            {
                ++iterator;
            }
        }

        target.unlock();
    }


    void SimpleGarbageCollector::removeFinalizedObjects()
    {

        auto &objects = _vm->finalizer_thread()->finalizer()->out_objects();

        objects.lock();

        auto iterator = objects->begin();

        while (iterator != objects->end())
        {
            _vm->memory_manager()->release_object(*iterator);
            iterator = objects->erase(iterator);
        }

        objects.unlock();
    }

}

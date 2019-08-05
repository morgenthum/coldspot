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

#ifndef COLDSPOT_JVM_VIRTUALMACHINE_HPP_
#define COLDSPOT_JVM_VIRTUALMACHINE_HPP_

#include <cstdint>

#include <jvm/common/HashMap.hpp>
#include <jvm/common/List.hpp>
#include <jvm/common/String.hpp>
#include <jvm/thread/Lockable.hpp>
#include <jvm/jdk/Global.hpp>
#include <jvm/Error.hpp>

namespace coldspot
{

    class ClassLoader;
    class FinalizerThread;
    class GCThread;
    class JDKHandler;
    class LibraryBinder;
    class MemoryManager;
    class Object;
    class Options;
    class Thread;
    class VMThread;

    // Represents the virtual machine.
    // It is used by all components to interact with each other.
    class VirtualMachine
    {
    public:

        struct
        {
            Class *classClass = 0;
            Class *cloneableClass = 0;
            Class *objectClass = 0;
            Class *fieldClass = 0;
            Class *nullPointerExceptionClass = 0;
            Class *serializableClass = 0;
            Class *stringClass = 0;
            Class *systemClass = 0;
            Class *threadClass = 0;
            Class *threadGroupClass = 0;
        } builtin;

        // Initializes all components and creates the interfaces.
        VirtualMachine();

        // Loads the native libraries,
        // attaches the current thread to the vm
        // and starts the garbage collection.
        jint initialize(Options *options);

        // Registers all builtin classes.
        error_t register_classes();

        // Stops all running threads, destroys all objects
        // and releases the virtual machine.
        void release();

        // Checks if the jni-version is supported.
        bool jni_version_supported(jint version) const;

        // Returns the interface by version
        void get_interface(jint version, void **interface) const;

        // Registers the object as global-reference and returns it.
        jobject add_global_ref(jobject object);

        // Registers the object as local-reference and returns it.
        jobject add_local_ref(jobject object);

        // Removes the object from global-references.
        void remove_global_ref(jobject object);

        // Removes the object from local-references.
        void remove_local_ref(jobject object);

        // Attaches the thread to the vm.
        // If no thread is specified, the vm- and java-thread are created
        // using the default constructor.
        error_t attach_thread(Thread *thread, bool daemon);

        // Removes the thread from thread-list
        // and detaches it from the native-thread.
        // If no thread is specified, the current thread is used.
        void detach_thread(Thread *thread = 0);

        // Creates a new vm-thread and binds the java-thread to it.
        error_t create_vm_thread(Object *javaThread, VMThread **vmThread);

        void resume_vm_threads();

        // Suspends all vm-threads.
        // Waits for all running threads until they reach their safepoints.
        // Native threads are automatically blocked if they return to the vm.
        void suspend_vm_threads();

        // Getters.
        Options *options() const { return _options; }
        JDKHandler *jdk_handler() const { return _jdk_handler; }
        ClassLoader *class_loader() const { return _class_Loader; }
        LibraryBinder *library_binder() const { return _library_binder; }
        MemoryManager *memory_manager() const { return _memory_manager; }
        FinalizerThread *finalizer_thread() const { return _finalizer_thread; }
        Lockable <List<Thread *>> &threads() { return _threads; }
        Lockable <List<jobject>> &local_references() { return _local_references; }
        Lockable <List<jobject>> &global_references() { return _global_references; }
        JavaVM *vm_interface() const { return _vm_interface; }
        JNIEnv *jni_interface() const { return _jni_interface; }
        HashMap<String, Object *> &string_pool() { return _string_pool; };
        Object *stack_overflow_error() const { return _stack_overflow_error; }

    private:

        // Count of non-daemon-threads.
        uint32_t _non_daemon_thread_count;

        // General components.
        Options *_options;
        JDKHandler *_jdk_handler;
        ClassLoader *_class_Loader;
        LibraryBinder *_library_binder;
        MemoryManager *_memory_manager;

        // Threading.
        GCThread *_gc_thread;
        FinalizerThread *_finalizer_thread;
        Lockable <List<Thread *>> _threads;

        // References.
        Lockable <List<jobject>> _local_references;
        Lockable <List<jobject>> _global_references;

        // Interfaces.
        JavaVM *_vm_interface;
        JNIEnv *_jni_interface;

        // Global pool of string literals
        HashMap<String, Object *> _string_pool;

        // Pre allocated error object.
        Object *_stack_overflow_error;

        // Creates the java-objects for error handling.
        error_t create_error_objects();

        // Initializes the garbage collection.
        void init_gc();

        // Sets basic properties.
        void setup_properties();

        // Waits for the termination of all non daemon threads.
        void wait_for_threads();

        // Create interface.
        void create_java_vm();
        void create_jni_env();

        // Release interfaces.
        void release_java_vm();
        void release_jni_env();
    };

    // Allow everyone to use the vm
    extern VirtualMachine *_vm;

}

#endif

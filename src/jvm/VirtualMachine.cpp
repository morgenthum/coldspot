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

#include <signal.h>

#include <jvm/Global.hpp>
#include "VirtualMachine.hpp"

#define SAFE_LOAD_CLASS(error_value, name, target) \
  error_value = _class_Loader->load_class(name, &target); \
  RETURN_ON_FAIL(error_value)

namespace coldspot
{

    VirtualMachine *_vm = 0;

    void handleSignal(int signal)
    {
        EXIT_FATAL("SIGSEGV occured");
    }

    VirtualMachine::VirtualMachine() : _non_daemon_thread_count(0), _options(0),
                                       _stack_overflow_error(0)
    {
        _vm = this;

#if defined(JDK_CLASSPATH)
        _jdkHandler = new ClasspathJDKHandler;
#elif defined(JDK_OPENJDK)
        _jdk_handler = new OpenJDKHandler;
#endif

        _class_Loader = new ClassLoader;
        _library_binder = new LibraryBinder;
        _memory_manager = new MemoryManager;

        create_java_vm();
        create_jni_env();
    }


    jint VirtualMachine::initialize(Options *options)
    {
        signal(SIGSEGV, handleSignal);

        // Make the options global
        _options = options;

        // Set basic properties
        setup_properties();

        // Initialize JDK-specifics
        error_t errorValue = _jdk_handler->initialize();
        if (errorValue != RETURN_OK)
        {
            EXIT_FATAL("failed to initialize jdk")
        }

        // Initialize critical error-objects
        errorValue = create_error_objects();
        if (errorValue != RETURN_OK)
        {
            EXIT_FATAL("failed to create preloaded error objects")
        }

        // Initialize garbage-collection
        // init_gc(); TODOc

        return JNI_OK;
    }


    error_t VirtualMachine::register_classes()
    {
        error_t errorValue;
        SAFE_LOAD_CLASS(errorValue, CLASSNAME_CLASS, builtin.classClass)
        SAFE_LOAD_CLASS(errorValue, CLASSNAME_CLONEABLE, builtin.cloneableClass)
        SAFE_LOAD_CLASS(errorValue, CLASSNAME_OBJECT, builtin.objectClass)
        SAFE_LOAD_CLASS(errorValue, CLASSNAME_FIELD, builtin.fieldClass)
        SAFE_LOAD_CLASS(errorValue, CLASSNAME_NULLPOINTEREXCEPTION,
            builtin.nullPointerExceptionClass)
        SAFE_LOAD_CLASS(errorValue, CLASSNAME_SERIALIZABLE,
            builtin.serializableClass)
        SAFE_LOAD_CLASS(errorValue, CLASSNAME_STRING, builtin.stringClass)
        SAFE_LOAD_CLASS(errorValue, CLASSNAME_SYSTEM, builtin.systemClass)
        SAFE_LOAD_CLASS(errorValue, CLASSNAME_THREAD, builtin.threadClass)
        SAFE_LOAD_CLASS(errorValue, CLASSNAME_THREADGROUP,
            builtin.threadGroupClass)
        return RETURN_OK;
    }


    void VirtualMachine::release()
    {
        wait_for_threads();

        _jdk_handler->release();

        release_java_vm();
        release_jni_env();

        DELETE_OBJECT(_options);
        DELETE_OBJECT(_jdk_handler);
        DELETE_OBJECT(_class_Loader);
        DELETE_OBJECT(_library_binder);
        DELETE_OBJECT(_memory_manager);

        DELETE_CONTAINER_OBJECTS(*_threads);

        _vm = 0;
    }


    bool VirtualMachine::jni_version_supported(jint version) const
    {
        return version <= JNI_VERSION_1_6;
    }


    void VirtualMachine::get_interface(jint version, void **interface) const
    {
        switch (version)
        {
            case JNI_VERSION_1_1:
            case JNI_VERSION_1_2:
            case JNI_VERSION_1_4:
            case JNI_VERSION_1_6:
            {
                *interface = _jni_interface;
                break;
            }
        }
    }


    jobject VirtualMachine::add_global_ref(jobject object)
    {
        if (object != 0)
        {
            _global_references.lock();
            _global_references->addBack(object);
            _global_references.unlock();
        }

        return object;
    }


    jobject VirtualMachine::add_local_ref(jobject object)
    {
        if (object != 0)
        {
            auto &frames = _current_executor->frames();

            if (frames.empty())
            {
                _local_references.lock();
                _local_references->addBack(object);
                _local_references.unlock();
            }
            else
            {
                Frame *frame = (Frame *) frames.peek();

                if (frame->type == FrameType::FRAMETYPE_NATIVE)
                {
                    frame->localReferences->addBack(object);
                }
                else
                {
                    EXIT_FATAL(
                        "failed to create local reference without native frame");
                }
            }
        }

        return object;
    }


    void VirtualMachine::remove_global_ref(jobject object)
    {
        if (object != 0)
        {
            _global_references.lock();

            auto iterator = _global_references->find(object);
            if (iterator != _global_references->end())
            {
                _global_references->erase(iterator);
            }

            _global_references.unlock();
        }
    }


    void VirtualMachine::remove_local_ref(jobject object)
    {
        if (object != 0)
        {
            auto &frames = _current_executor->frames();

            if (!frames.empty())
            {
                Frame *frame = (Frame *) frames.peek();
                auto &localReferences = frame->localReferences;

                auto iterator = localReferences->find(object);
                if (iterator != localReferences->end())
                {
                    localReferences->erase(iterator);
                    return;
                }
            }

            _local_references.lock();

            auto iterator = _local_references->find(object);
            if (iterator != _local_references->end())
            {
                _local_references->erase(iterator);
            }

            _local_references.unlock();
        }
    }


    error_t VirtualMachine::attach_thread(Thread *thread, bool daemon)
    {
        // Increment non-daemon-thread count
        if (!daemon)
        {
            ++_non_daemon_thread_count;
        }

        if (thread == 0)
        {
            // Create new java-thread
            Object *object;
            error_t errorValue = java_lang_Thread::newThread(&object);
            if (errorValue != RETURN_OK)
            {
                EXIT_FATAL("failed to create new java-thread (attaching)")
            }

            // Create new vm-thread
            thread = new VMThread(THREADSTATE_NEW);

            // Bind java-thread to vm-thread
            ((VMThread *) thread)->bind(object);
        }

        // Attach vm-thread to current native-thread
        thread->attach_native();

        // Lock the mutex for WAITING state
        thread->wait_mutex().lock();

        // Add thread to thread-list
        _threads.lock();
        _threads->addBack(thread);
        _threads.unlock();

        // Set thread as RUNNABLE
        thread->set_state(THREADSTATE_RUNNABLE);

        return RETURN_OK;
    }


    void VirtualMachine::detach_thread(Thread *thread)
    {
        // If no thread is specified, take the current thread
        if (thread == 0)
        {
            thread = _current_thread;
        }

        // Check for daemon before detach thread
        bool daemon = thread->is_daemon();

        // Unlock the mutex for WAITING state
        thread->wait_mutex().unlock();

        // Detach thread from native thread
        thread->detach_native();

        // Decrement running thread counter
        if (!daemon)
        {
            --_non_daemon_thread_count;
        }

        // Mark thread as terminated
        thread->set_state(THREADSTATE_TERMINATED);
    }


    error_t VirtualMachine::create_vm_thread(Object *javaThread,
        VMThread **vmThread)
    {
        // Create new vm-thread
        *vmThread = new VMThread(THREADSTATE_NEW);

        // Bind java-thread to vm-thread
        (*vmThread)->bind(javaThread);

        return RETURN_OK;
    }


    void VirtualMachine::resume_vm_threads()
    {
        for (auto thread : *_threads)
        {
            bool vmThreadType = thread->type() == THREADTYPE_VM ||
                                thread->type() == THREADTYPE_FINALIZER;

            // Only non-terminated vm-threads can be resumed
            if (vmThreadType && thread->is_alive())
            {
                // Unlock the thread
                thread->block_mutex().unlock();

                // Remove the waiting-condition
                thread->wait_condition().set_wait_requested(false);
                thread->wait_condition().notify_all();
            }
        }
    }


    void VirtualMachine::suspend_vm_threads()
    {
        // Request all vm-threads to pause at the next safepoint
        for (auto thread : *_threads)
        {
            bool vmThreadType = thread->type() == THREADTYPE_VM ||
                                thread->type() == THREADTYPE_FINALIZER;

            // Only living vm-threads are relevant
            if (vmThreadType && thread->is_alive())
            {
                // Block the thread (native code)
                thread->block_mutex().lock();

                // Tell the thread to wait at the next safepoint (executing java)
                thread->wait_condition().set_wait_requested(true);
            }
        }

        // Wait for all vm-threads to reach their safepoints or get blocked
        for (auto thread : *_threads)
        {
            bool vmThreadType = thread->type() == THREADTYPE_VM ||
                                thread->type() == THREADTYPE_FINALIZER;

            // Only living vm-threads are relevant
            if (vmThreadType && thread->is_alive())
            {
                Executor *executor = ((VMThread *) thread)->executor();
                auto &frames = executor->frames();

                if (!frames.empty())
                {
                    Frame *frame = (Frame *) frames.peek();

                    // Skip the thread if it executes a native frame,
                    // it will be blocked if it returns to the vm
                    if (frame->type == FrameType::FRAMETYPE_NATIVE)
                    {
                        continue;
                    }
                }

                // Threads that are not in runnable state (blocking, waiting, ...)
                // are already waiting, so we only have to wait for the runnable
                // threads until they reach their safepoints
                while (thread->state() == THREADSTATE_RUNNABLE)
                {
                    System::sleep(1);
                }
            }
        }
    }


    error_t VirtualMachine::create_error_objects()
    {
        // Create java/lang/StackOverflowError
        Class *clazz;
        error_t errorValue = _class_Loader->load_class(
            CLASSNAME_STACKOVERFLOWERROR, &clazz);
        RETURN_ON_FAIL(errorValue)

        errorValue = Object::new_object_default(clazz, &_stack_overflow_error);
        RETURN_ON_FAIL(errorValue)

        return RETURN_OK;
    }


    void VirtualMachine::init_gc()
    {
        // Create and initialize finalizer- and gc-thread
        _finalizer_thread = new FinalizerThread;
        _gc_thread = new GCThread;

        // Create new java-thread for finalizer-thread
        Object *finalizerJavaThread;
        error_t errorValue = java_lang_Thread::newThread(&finalizerJavaThread);
        if (errorValue != RETURN_OK)
        {
            EXIT_FATAL("failed to create finalizer-thread");
        }

        // Bind java-thread to finalizer-thread
        _finalizer_thread->bind(finalizerJavaThread);

        // Start threads
        _finalizer_thread->Thread::start(true);
        _gc_thread->start(true);
    }


    void VirtualMachine::setup_properties()
    {
        // TODO remove
        /*String classPath = ".:/Users/mario/Development/openjdk/jre/lib/rt:"
          "/Users/mario/Development/openjdk/jre/lib/charsets:"
          "/Users/mario/Development/openjdk/jre/lib/jce";*/

        String classPath = ".;C:/openjdk-1.7.0-u80/jre/lib/rt;"
            "C:/openjdk-1.7.0-u80/jre/lib/charsets;"
            "C:/openjdk-1.7.0-u80/jre/lib/jce";

        _options->set_property("file.encoding", "UTF-8");
        _options->set_property("file.separator", "/");
        _options->set_property("java.class.path", classPath);
        _options->set_property("java.class.version", "51.0");
        _options->set_property("java.specification.name",
            "Java Language Specification");
        _options->set_property("java.specification.vendor",
            "Oracle Corporation");
        _options->set_property("java.specification.version", "1.7");
        _options->set_property("java.vendor", "ColdSpot");
        _options->set_property("java.vendor.url", "http://www.coldspotvm.org");
        _options->set_property("java.version", "1.7");
        _options->set_property("java.vm.info", "interpreter mode");
        _options->set_property("java.vm.name", "ColdSpot");
        _options->set_property("java.vm.specification.name",
            "Java Virtual Machine Specification");
        _options->set_property("java.vm.specification.vendor",
            "Oracle Corporation");
        _options->set_property("java.vm.specification.version", "1.7");
        _options->set_property("java.vm.vendor", "Mario Morgenthum");
        _options->set_property("java.vm.version", "0.1");
        _options->set_property("line.separator", "\n");
        _options->set_property("os.arch", System::architecture());
        _options->set_property("os.name", System::name());
        _options->set_property("os.version", System::version());
        _options->set_property("path.separator", ":");
        _options->set_property("user.country", "DE");
        _options->set_property("user.dir", System::workingDirectory());
        _options->set_property("user.home", System::userhome());
        _options->set_property("user.name", System::username());
    }


    void VirtualMachine::wait_for_threads()
    {
        // Wait while non-daemon-threads are running
        while (_non_daemon_thread_count != 0)
        {
            Thread *non_daemon_thread = 0;

            _threads.lock();

            auto begin = _threads->begin();
            auto end = _threads->end();

            while (begin != end)
            {
                Thread *thread = *begin;

                if (!thread->is_daemon())
                {
                    non_daemon_thread = thread;
                    break;
                }

                ++begin;
            }

            _threads.unlock();

            if (non_daemon_thread != 0)
            {
                non_daemon_thread->join();
            }
        }

        // Stop finalizer-thread (runFinalizersOnExit not supported yet)
        _finalizer_thread->set_running(false);
        _finalizer_thread->join();

        // Stop all running daemon-threads
        _threads.lock();

        for (Thread *thread : *_threads)
        {
            if (thread->type() == THREADTYPE_VM && thread->is_alive())
            {
                thread->stop();
            }
        }

        _threads.unlock();

        // Stop gc-thread
        _gc_thread->set_running(false);
        _gc_thread->join();
    }


    void VirtualMachine::create_java_vm()
    {
        JNIInvokeInterface_ *invokeInterface = new JNIInvokeInterface_;
        invokeInterface->AttachCurrentThread = AttachCurrentThread;
        invokeInterface->AttachCurrentThreadAsDaemon = AttachCurrentThreadAsDaemon;
        invokeInterface->DestroyJavaVM = DestroyJavaVM;
        invokeInterface->DetachCurrentThread = DetachCurrentThread;
        invokeInterface->GetEnv = GetEnv;

        _vm_interface = new JavaVM;
        _vm_interface->functions = invokeInterface;
    }


    void VirtualMachine::create_jni_env()
    {
        JNINativeInterface_ *nativeInterface = new JNINativeInterface_;
        nativeInterface->GetVersion = JNIGetVersion;
        nativeInterface->DefineClass = DefineClass;
        nativeInterface->FindClass = FindClass;
        nativeInterface->FromReflectedMethod = FromReflectedMethod;
        nativeInterface->FromReflectedField = FromReflectedField;
        nativeInterface->ToReflectedMethod = ToReflectedMethod;
        nativeInterface->GetSuperclass = GetSuperclass;
        nativeInterface->IsAssignableFrom = IsAssignableFrom;
        nativeInterface->ToReflectedField = ToReflectedField;
        nativeInterface->Throw = Throw;
        nativeInterface->ThrowNew = ThrowNew;
        nativeInterface->ExceptionOccurred = ExceptionOccurred;
        nativeInterface->ExceptionDescribe = ExceptionDescribe;
        nativeInterface->ExceptionClear = ExceptionClear;
        nativeInterface->FatalError = FatalError;
        nativeInterface->PushLocalFrame = PushLocalFrame;
        nativeInterface->PopLocalFrame = PopLocalFrame;
        nativeInterface->NewGlobalRef = NewGlobalRef;
        nativeInterface->DeleteGlobalRef = DeleteGlobalRef;
        nativeInterface->DeleteLocalRef = DeleteLocalRef;
        nativeInterface->IsSameObject = IsSameObject;
        nativeInterface->NewLocalRef = NewLocalRef;
        nativeInterface->EnsureLocalCapacity = EnsureLocalCapacity;
        nativeInterface->AllocObject = AllocObject;
        nativeInterface->NewObject = NewObject;
        nativeInterface->NewObjectV = NewObjectV;
        nativeInterface->NewObjectA = NewObjectA;
        nativeInterface->GetObjectClass = GetObjectClass;
        nativeInterface->IsInstanceOf = IsInstanceOf;
        nativeInterface->GetMethodID = GetMethodID;
        nativeInterface->CallObjectMethod = CallObjectMethod;
        nativeInterface->CallObjectMethodV = CallObjectMethodV;
        nativeInterface->CallObjectMethodA = CallObjectMethodA;
        nativeInterface->CallBooleanMethod = CallBooleanMethod;
        nativeInterface->CallBooleanMethodV = CallBooleanMethodV;
        nativeInterface->CallBooleanMethodA = CallBooleanMethodA;
        nativeInterface->CallByteMethod = CallByteMethod;
        nativeInterface->CallByteMethodV = CallByteMethodV;
        nativeInterface->CallByteMethodA = CallByteMethodA;
        nativeInterface->CallCharMethod = CallCharMethod;
        nativeInterface->CallCharMethodV = CallCharMethodV;
        nativeInterface->CallCharMethodA = CallCharMethodA;
        nativeInterface->CallShortMethod = CallShortMethod;
        nativeInterface->CallShortMethodV = CallShortMethodV;
        nativeInterface->CallShortMethodA = CallShortMethodA;
        nativeInterface->CallIntMethod = CallIntMethod;
        nativeInterface->CallIntMethodV = CallIntMethodV;
        nativeInterface->CallIntMethodA = CallIntMethodA;
        nativeInterface->CallLongMethod = CallLongMethod;
        nativeInterface->CallLongMethodV = CallLongMethodV;
        nativeInterface->CallLongMethodA = CallLongMethodA;
        nativeInterface->CallFloatMethod = CallFloatMethod;
        nativeInterface->CallFloatMethodV = CallFloatMethodV;
        nativeInterface->CallFloatMethodA = CallFloatMethodA;
        nativeInterface->CallDoubleMethod = CallDoubleMethod;
        nativeInterface->CallDoubleMethodV = CallDoubleMethodV;
        nativeInterface->CallDoubleMethodA = CallDoubleMethodA;
        nativeInterface->CallVoidMethod = CallVoidMethod;
        nativeInterface->CallVoidMethodV = CallVoidMethodV;
        nativeInterface->CallVoidMethodA = CallVoidMethodA;
        nativeInterface->CallNonvirtualObjectMethod = CallNonvirtualObjectMethod;
        nativeInterface->CallNonvirtualObjectMethodV = CallNonvirtualObjectMethodV;
        nativeInterface->CallNonvirtualObjectMethodA = CallNonvirtualObjectMethodA;
        nativeInterface->CallNonvirtualBooleanMethod = CallNonvirtualBooleanMethod;
        nativeInterface->CallNonvirtualBooleanMethodV = CallNonvirtualBooleanMethodV;
        nativeInterface->CallNonvirtualBooleanMethodA = CallNonvirtualBooleanMethodA;
        nativeInterface->CallNonvirtualByteMethod = CallNonvirtualByteMethod;
        nativeInterface->CallNonvirtualByteMethodV = CallNonvirtualByteMethodV;
        nativeInterface->CallNonvirtualByteMethodA = CallNonvirtualByteMethodA;
        nativeInterface->CallNonvirtualCharMethod = CallNonvirtualCharMethod;
        nativeInterface->CallNonvirtualCharMethodV = CallNonvirtualCharMethodV;
        nativeInterface->CallNonvirtualCharMethodA = CallNonvirtualCharMethodA;
        nativeInterface->CallNonvirtualShortMethod = CallNonvirtualShortMethod;
        nativeInterface->CallNonvirtualShortMethodV = CallNonvirtualShortMethodV;
        nativeInterface->CallNonvirtualShortMethodA = CallNonvirtualShortMethodA;
        nativeInterface->CallNonvirtualIntMethod = CallNonvirtualIntMethod;
        nativeInterface->CallNonvirtualIntMethodV = CallNonvirtualIntMethodV;
        nativeInterface->CallNonvirtualIntMethodA = CallNonvirtualIntMethodA;
        nativeInterface->CallNonvirtualLongMethod = CallNonvirtualLongMethod;
        nativeInterface->CallNonvirtualLongMethodV = CallNonvirtualLongMethodV;
        nativeInterface->CallNonvirtualLongMethodA = CallNonvirtualLongMethodA;
        nativeInterface->CallNonvirtualFloatMethod = CallNonvirtualFloatMethod;
        nativeInterface->CallNonvirtualFloatMethodV = CallNonvirtualFloatMethodV;
        nativeInterface->CallNonvirtualFloatMethodA = CallNonvirtualFloatMethodA;
        nativeInterface->CallNonvirtualDoubleMethod = CallNonvirtualDoubleMethod;
        nativeInterface->CallNonvirtualDoubleMethodV = CallNonvirtualDoubleMethodV;
        nativeInterface->CallNonvirtualDoubleMethodA = CallNonvirtualDoubleMethodA;
        nativeInterface->CallNonvirtualVoidMethod = CallNonvirtualVoidMethod;
        nativeInterface->CallNonvirtualVoidMethodV = CallNonvirtualVoidMethodV;
        nativeInterface->CallNonvirtualVoidMethodA = CallNonvirtualVoidMethodA;
        nativeInterface->GetFieldID = GetFieldID;
        nativeInterface->GetObjectField = GetObjectField;
        nativeInterface->GetBooleanField = GetBooleanField;
        nativeInterface->GetByteField = GetByteField;
        nativeInterface->GetCharField = GetCharField;
        nativeInterface->GetShortField = GetShortField;
        nativeInterface->GetIntField = GetIntField;
        nativeInterface->GetLongField = GetLongField;
        nativeInterface->GetFloatField = GetFloatField;
        nativeInterface->GetDoubleField = GetDoubleField;
        nativeInterface->SetObjectField = SetObjectField;
        nativeInterface->SetBooleanField = SetBooleanField;
        nativeInterface->SetByteField = SetByteField;
        nativeInterface->SetCharField = SetCharField;
        nativeInterface->SetShortField = SetShortField;
        nativeInterface->SetIntField = SetIntField;
        nativeInterface->SetLongField = SetLongField;
        nativeInterface->SetFloatField = SetFloatField;
        nativeInterface->SetDoubleField = SetDoubleField;
        nativeInterface->GetStaticMethodID = GetStaticMethodID;
        nativeInterface->CallStaticObjectMethod = CallStaticObjectMethod;
        nativeInterface->CallStaticObjectMethodV = CallStaticObjectMethodV;
        nativeInterface->CallStaticObjectMethodA = CallStaticObjectMethodA;
        nativeInterface->CallStaticBooleanMethod = CallStaticBooleanMethod;
        nativeInterface->CallStaticBooleanMethodV = CallStaticBooleanMethodV;
        nativeInterface->CallStaticBooleanMethodA = CallStaticBooleanMethodA;
        nativeInterface->CallStaticByteMethod = CallStaticByteMethod;
        nativeInterface->CallStaticByteMethodV = CallStaticByteMethodV;
        nativeInterface->CallStaticByteMethodA = CallStaticByteMethodA;
        nativeInterface->CallStaticCharMethod = CallStaticCharMethod;
        nativeInterface->CallStaticCharMethodV = CallStaticCharMethodV;
        nativeInterface->CallStaticCharMethodA = CallStaticCharMethodA;
        nativeInterface->CallStaticShortMethod = CallStaticShortMethod;
        nativeInterface->CallStaticShortMethodV = CallStaticShortMethodV;
        nativeInterface->CallStaticShortMethodA = CallStaticShortMethodA;
        nativeInterface->CallStaticIntMethod = CallStaticIntMethod;
        nativeInterface->CallStaticIntMethodV = CallStaticIntMethodV;
        nativeInterface->CallStaticIntMethodA = CallStaticIntMethodA;
        nativeInterface->CallStaticLongMethod = CallStaticLongMethod;
        nativeInterface->CallStaticLongMethodV = CallStaticLongMethodV;
        nativeInterface->CallStaticLongMethodA = CallStaticLongMethodA;
        nativeInterface->CallStaticFloatMethod = CallStaticFloatMethod;
        nativeInterface->CallStaticFloatMethodV = CallStaticFloatMethodV;
        nativeInterface->CallStaticFloatMethodA = CallStaticFloatMethodA;
        nativeInterface->CallStaticDoubleMethod = CallStaticDoubleMethod;
        nativeInterface->CallStaticDoubleMethodV = CallStaticDoubleMethodV;
        nativeInterface->CallStaticDoubleMethodA = CallStaticDoubleMethodA;
        nativeInterface->CallStaticVoidMethod = CallStaticVoidMethod;
        nativeInterface->CallStaticVoidMethodV = CallStaticVoidMethodV;
        nativeInterface->CallStaticVoidMethodA = CallStaticVoidMethodA;
        nativeInterface->GetStaticFieldID = GetStaticFieldID;
        nativeInterface->GetStaticObjectField = GetStaticObjectField;
        nativeInterface->GetStaticBooleanField = GetStaticBooleanField;
        nativeInterface->GetStaticByteField = GetStaticByteField;
        nativeInterface->GetStaticCharField = GetStaticCharField;
        nativeInterface->GetStaticShortField = GetStaticShortField;
        nativeInterface->GetStaticIntField = GetStaticIntField;
        nativeInterface->GetStaticLongField = GetStaticLongField;
        nativeInterface->GetStaticFloatField = GetStaticFloatField;
        nativeInterface->GetStaticDoubleField = GetStaticDoubleField;
        nativeInterface->SetStaticObjectField = SetStaticObjectField;
        nativeInterface->SetStaticBooleanField = SetStaticBooleanField;
        nativeInterface->SetStaticByteField = SetStaticByteField;
        nativeInterface->SetStaticCharField = SetStaticCharField;
        nativeInterface->SetStaticShortField = SetStaticShortField;
        nativeInterface->SetStaticIntField = SetStaticIntField;
        nativeInterface->SetStaticLongField = SetStaticLongField;
        nativeInterface->SetStaticFloatField = SetStaticFloatField;
        nativeInterface->SetStaticDoubleField = SetStaticDoubleField;
        nativeInterface->NewString = NewString;
        nativeInterface->GetStringLength = GetStringLength;
        nativeInterface->GetStringChars = GetStringChars;
        nativeInterface->ReleaseStringChars = ReleaseStringChars;
        nativeInterface->NewStringUTF = NewStringUTF;
        nativeInterface->GetStringUTFLength = GetStringUTFLength;
        nativeInterface->GetStringUTFChars = GetStringUTFChars;
        nativeInterface->ReleaseStringUTFChars = ReleaseStringUTFChars;
        nativeInterface->GetArrayLength = GetArrayLength;
        nativeInterface->NewObjectArray = NewObjectArray;
        nativeInterface->GetObjectArrayElement = GetObjectArrayElement;
        nativeInterface->SetObjectArrayElement = SetObjectArrayElement;
        nativeInterface->NewBooleanArray = NewBooleanArray;
        nativeInterface->NewByteArray = NewByteArray;
        nativeInterface->NewCharArray = NewCharArray;
        nativeInterface->NewShortArray = NewShortArray;
        nativeInterface->NewIntArray = NewIntArray;
        nativeInterface->NewLongArray = NewLongArray;
        nativeInterface->NewFloatArray = NewFloatArray;
        nativeInterface->NewDoubleArray = NewDoubleArray;
        nativeInterface->GetBooleanArrayElements = GetBooleanArrayElements;
        nativeInterface->GetByteArrayElements = GetByteArrayElements;
        nativeInterface->GetCharArrayElements = GetCharArrayElements;
        nativeInterface->GetShortArrayElements = GetShortArrayElements;
        nativeInterface->GetIntArrayElements = GetIntArrayElements;
        nativeInterface->GetLongArrayElements = GetLongArrayElements;
        nativeInterface->GetFloatArrayElements = GetFloatArrayElements;
        nativeInterface->GetDoubleArrayElements = GetDoubleArrayElements;
        nativeInterface->ReleaseBooleanArrayElements = ReleaseBooleanArrayElements;
        nativeInterface->ReleaseByteArrayElements = ReleaseByteArrayElements;
        nativeInterface->ReleaseCharArrayElements = ReleaseCharArrayElements;
        nativeInterface->ReleaseShortArrayElements = ReleaseShortArrayElements;
        nativeInterface->ReleaseIntArrayElements = ReleaseIntArrayElements;
        nativeInterface->ReleaseLongArrayElements = ReleaseLongArrayElements;
        nativeInterface->ReleaseFloatArrayElements = ReleaseFloatArrayElements;
        nativeInterface->ReleaseDoubleArrayElements = ReleaseDoubleArrayElements;
        nativeInterface->GetBooleanArrayRegion = GetBooleanArrayRegion;
        nativeInterface->GetByteArrayRegion = GetByteArrayRegion;
        nativeInterface->GetCharArrayRegion = GetCharArrayRegion;
        nativeInterface->GetShortArrayRegion = GetShortArrayRegion;
        nativeInterface->GetIntArrayRegion = GetIntArrayRegion;
        nativeInterface->GetLongArrayRegion = GetLongArrayRegion;
        nativeInterface->GetFloatArrayRegion = GetFloatArrayRegion;
        nativeInterface->GetDoubleArrayRegion = GetDoubleArrayRegion;
        nativeInterface->SetBooleanArrayRegion = SetBooleanArrayRegion;
        nativeInterface->SetByteArrayRegion = SetByteArrayRegion;
        nativeInterface->SetCharArrayRegion = SetCharArrayRegion;
        nativeInterface->SetShortArrayRegion = SetShortArrayRegion;
        nativeInterface->SetIntArrayRegion = SetIntArrayRegion;
        nativeInterface->SetLongArrayRegion = SetLongArrayRegion;
        nativeInterface->SetFloatArrayRegion = SetFloatArrayRegion;
        nativeInterface->SetDoubleArrayRegion = SetDoubleArrayRegion;
        nativeInterface->RegisterNatives = RegisterNatives;
        nativeInterface->UnregisterNatives = UnregisterNatives;
        nativeInterface->MonitorEnter = MonitorEnter;
        nativeInterface->MonitorExit = MonitorExit;
        nativeInterface->GetJavaVM = GetJavaVM;
        nativeInterface->GetStringRegion = GetStringRegion;
        nativeInterface->GetStringUTFRegion = GetStringUTFRegion;
        nativeInterface->GetPrimitiveArrayCritical = GetPrimitiveArrayCritical;
        nativeInterface->ReleasePrimitiveArrayCritical = ReleasePrimitiveArrayCritical;
        nativeInterface->GetStringCritical = GetStringCritical;
        nativeInterface->ReleaseStringCritical = ReleaseStringCritical;
        nativeInterface->NewWeakGlobalRef = NewWeakGlobalRef;
        nativeInterface->DeleteWeakGlobalRef = DeleteWeakGlobalRef;
        nativeInterface->ExceptionCheck = ExceptionCheck;
        nativeInterface->NewDirectByteBuffer = NewDirectByteBuffer;
        nativeInterface->GetDirectBufferAddress = GetDirectBufferAddress;
        nativeInterface->GetDirectBufferCapacity = GetDirectBufferCapacity;
        nativeInterface->GetObjectRefType = GetObjectRefType;

        _jni_interface = new JNIEnv;
        _jni_interface->functions = nativeInterface;
    }


    void VirtualMachine::release_java_vm()
    {
        if (_vm_interface != 0)
        {
            DELETE_OBJECT(_vm_interface->functions)
        }

        DELETE_OBJECT(_vm_interface)
    }


    void VirtualMachine::release_jni_env()
    {
        if (_jni_interface != 0)
        {
            DELETE_OBJECT(_jni_interface->functions)
        }

        DELETE_OBJECT(_jni_interface)
    }

}

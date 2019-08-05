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

#include <jvm/Environment.hpp>

#if defined(JDK_OPENJDK)

    #include <zip.h>

    #include <jvm/Global.hpp>
    #include <jvm/VirtualMachine.hpp>

    #define METHOD_COUNT(methods) (sizeof(methods) / sizeof(methods[0]))

namespace coldspot
{

  error_t OpenJDKHandler::initialize()
  {
    setupProperties();

    loadBindingLibrary();

    int err = 0;
    auto lib = zip_open("C:/openjdk-1.7.0-u80/jre/lib/rt.jar", 0,
      &err);

    _javaLibrary.addBack(lib);
    _javaLibrary.addBack(
      zip_open("C:/openjdk-1.7.0-u80/jre/lib/charsets.jar", 0, 0));
    _javaLibrary.addBack(
      zip_open("C:/openjdk-1.7.0-u80/jre/lib/jce.jar", 0, 0));
    _javaLibrary.addBack(
      zip_open("C:/openjdk-1.7.0-u80/jre/lib/jsse.jar", 0, 0));
    _javaLibrary.addBack(
      zip_open("C:/openjdk-1.7.0-u80/jre/lib/management-agent.jar",
        0, 0));
    _javaLibrary.addBack(
      zip_open("C:/openjdk-1.7.0-u80/jre/lib/resources.jar", 0, 0));

    /*_javaLibrary.addBack(
      zip_open("C:/openjdk-1.7.0-u80/jre/lib/charsets.jar", 0, 0));
    _javaLibrary.addBack(
      zip_open("C:/openjdk-1.7.0-u80/jre/lib/jce.jar", 0, 0));
    _javaLibrary.addBack(
      zip_open("C:/openjdk-1.7.0-u80/jre/lib/jsse.jar", 0, 0));
    _javaLibrary.addBack(
      zip_open("C:/openjdk-1.7.0-u80/jre/lib/management-agent.jar", 0, 0));
    _javaLibrary.addBack(
      zip_open("C:/openjdk-1.7.0-u80/jre/lib/resources.jar", 0, 0));
    _javaLibrary.addBack(zip_open("C:/openjdk-1.7.0-u80/jre/lib/rt.jar", 0, 0));*/

    // Creates a new vm-thread, attaches it to the current native-thread
    // and binds a new java-thread to it
    error_t errorValue = createInitialThread();
    if (errorValue != RETURN_OK)
    {
      return -1;
    }

    return RETURN_OK;
  }


  error_t OpenJDKHandler::release()
  {

    for (void *library : _javaLibrary)
    {
      zip_close((zip *) library);
    }

    return RETURN_OK;
  }


  List<void *> &OpenJDKHandler::library()
  {

    return _javaLibrary;
  }


  void OpenJDKHandler::loadBindingLibrary()
  {
    Library_t lib = _vm->library_binder()->load_lib(
      "C:/openjdk-1.7.0-u80/jre/lib/java.dll");
    if (lib == 0)
    {
      EXIT_FATAL("failed to load jdk native library")
    }
  }


  error_t OpenJDKHandler::createInitialThread()
  {
    // Create vm-thread and attach it to the native-thread
    VMThread *vmThread = new VMThread(THREADSTATE_RUNNABLE);
    error_t errorValue = _vm->attach_thread(vmThread, false);
    RETURN_ON_FAIL(errorValue)

    errorValue = _vm->register_classes();
    RETURN_ON_FAIL(errorValue)

    errorValue = _vm->class_loader()->initialize_class(_vm->builtin.classClass);
    RETURN_ON_FAIL(errorValue)

    Method *classConstructor;
    errorValue = _vm->builtin.classClass->get_method(
      Signature("(Ljava/lang/ClassLoader;)V", METHODNAME_CONSTRUCTOR),
      &classConstructor);
    RETURN_ON_FAIL(errorValue);

    auto &loadedClasses = _vm->class_loader()->loaded_classes();
    auto begin = loadedClasses.begin();
    auto end = loadedClasses.end();

    while (begin != end)
    {
      Value result;
      Value parameter((Object *) 0);
      classConstructor->invoke(begin->value->object, &parameter, &result);
      RETURN_ON_FAIL(errorValue);

      ++begin;
    }

    // Create system-thread-group
    Object *systemThreadGroup;
    java_lang_ThreadGroup::newSystemThreadGroup(&systemThreadGroup);
    RETURN_ON_FAIL(errorValue)

    // Create main-thread-group-name
    Object *mainString;
    errorValue = java_lang_String::new_(u"main", &mainString);
    RETURN_ON_FAIL(errorValue);

    // Create main-thread-group
    Object *mainThreadGroup;
    errorValue = java_lang_ThreadGroup::newThreadGroup(systemThreadGroup,
      mainString, &mainThreadGroup);
    RETURN_ON_FAIL(errorValue)

    // Load thread class
    Class *threadClass = _vm->builtin.threadClass;

    // Load thread-constructor
    Method *threadConstructor;
    errorValue = threadClass->get_method(
      Signature("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V",
        METHODNAME_CONSTRUCTOR), &threadConstructor);
    RETURN_ON_FAIL(errorValue);

    // Initialize thread-class
    errorValue = _vm->class_loader()->initialize_class(threadClass);
    RETURN_ON_FAIL(errorValue);

    // Allocate object
    Object *initialThread;
    errorValue = _vm->memory_manager()->allocate_object(threadClass,
      &initialThread);
    RETURN_ON_FAIL(errorValue);

    // Bind java-thread to vm-thread
    vmThread->bind(initialThread);

    // Set default priority
    errorValue = java_lang_Thread::setPriority(initialThread, 5);
    RETURN_ON_FAIL(errorValue);

    // Invoke constructor
    Value value;
    Value parameters[2];
    parameters[0] = mainThreadGroup;
    parameters[1] = mainString;
    errorValue = threadConstructor->invoke(initialThread, parameters, &value);
    RETURN_ON_FAIL(errorValue);

    // Registering native methods
    registerMethods();

    // Initialize system class
    errorValue = java_lang_System::initializeSystemClass();
    RETURN_ON_FAIL(errorValue)

    return RETURN_OK;
  }


  void OpenJDKHandler::setupProperties()
  {
    Options *options = _vm->options();

    options->set_property("java.ext.dirs", ".");
    options->set_property("java.io.tmpdir", ".");
    options->set_property("java.home", "C:/openjdk-1.7.0-u80/");
    options->set_property("java.library.path",
      "C:/openjdk-1.7.0-u80/jre/bin/");
    options->set_property("sun.boot.library.path",
      "C:/openjdk-1.7.0-u80/jre/bin/");
    options->set_property("gnu.cpu.endian", "little");
  }


  // See http://code.metager.de/source/xref/openjdk/jdk7/jdk/src/share/classes/sun/misc/Unsafe.java
  // and http://mishadoff.com/blog/java-magic-part-4-sun-dot-misc-dot-unsafe/
  void OpenJDKHandler::registerMethods()
  {
    JNIEnv *env = _vm->jni_interface();

    // Methods of sun/misc/Unsafe
    jclass unsafeClass = env->FindClass("sun/misc/Unsafe");
    JNINativeMethod unsafeMethods[] = {{(char *) "registerNatives",        (char *) "()V",                                                                                             (void *) &Unsafe_registerNatives},

                                       {(char *) "getObject",              (char *) "(Ljava/lang/Object;J)Ljava/lang/Object;",                                                         (void *) &Unsafe_getObject_object},
                                       {(char *) "getBoolean",             (char *) "(Ljava/lang/Object;J)Z",                                                                          (void *) &Unsafe_getBoolean_object},
                                       {(char *) "getByte",                (char *) "(Ljava/lang/Object;J)B",                                                                          (void *) &Unsafe_getByte_object},
                                       {(char *) "getShort",               (char *) "(Ljava/lang/Object;J)S",                                                                          (void *) &Unsafe_getShort_object},
                                       {(char *) "getChar",                (char *) "(Ljava/lang/Object;J)C",                                                                          (void *) &Unsafe_getChar_object},
                                       {(char *) "getInt",                 (char *) "(Ljava/lang/Object;J)I",                                                                          (void *) &Unsafe_getInt_object},
                                       {(char *) "getLong",                (char *) "(Ljava/lang/Object;J)J",                                                                          (void *) &Unsafe_getLong_object},
                                       {(char *) "getFloat",               (char *) "(Ljava/lang/Object;J)F",                                                                          (void *) &Unsafe_getFloat_object},
                                       {(char *) "getDouble",              (char *) "(Ljava/lang/Object;J)D",                                                                          (void *) &Unsafe_getDouble_object},
                                       {(char *) "putObject",              (char *) "(Ljava/lang/Object;JLjava/lang/Object;)V",                                                        (void *) &Unsafe_putObject_object},
                                       {(char *) "putBoolean",             (char *) "(Ljava/lang/Object;JZ)V",                                                                         (void *) &Unsafe_putBoolean_object},
                                       {(char *) "putByte",                (char *) "(Ljava/lang/Object;JB)V",                                                                         (void *) &Unsafe_putByte_object},
                                       {(char *) "putShort",               (char *) "(Ljava/lang/Object;JS)V",                                                                         (void *) &Unsafe_putShort_object},
                                       {(char *) "putChar",                (char *) "(Ljava/lang/Object;JC)V",                                                                         (void *) &Unsafe_putChar_object},
                                       {(char *) "putInt",                 (char *) "(Ljava/lang/Object;JI)V",                                                                         (void *) &Unsafe_putInt_object},
                                       {(char *) "putLong",                (char *) "(Ljava/lang/Object;JJ)V",                                                                         (void *) &Unsafe_putLong_object},
                                       {(char *) "putFloat",               (char *) "(Ljava/lang/Object;JF)V",                                                                         (void *) &Unsafe_putFloat_object},
                                       {(char *) "putDouble",              (char *) "(Ljava/lang/Object;JD)V",                                                                         (void *) &Unsafe_putDouble_object},

                                       {(char *) "getByte",                (char *) "(J)B",                                                                                            (void *) &Unsafe_getByte_address},
                                       {(char *) "getShort",               (char *) "(J)S",                                                                                            (void *) &Unsafe_getShort_address},
                                       {(char *) "getChar",                (char *) "(J)C",                                                                                            (void *) &Unsafe_getChar_address},
                                       {(char *) "getInt",                 (char *) "(J)I",                                                                                            (void *) &Unsafe_getInt_address},
                                       {(char *) "getLong",                (char *) "(J)J",                                                                                            (void *) &Unsafe_getLong_address},
                                       {(char *) "getFloat",               (char *) "(J)F",                                                                                            (void *) &Unsafe_getFloat_address},
                                       {(char *) "getDouble",              (char *) "(J)D",                                                                                            (void *) &Unsafe_getDouble_address},
                                       {(char *) "getAddress",             (char *) "(J)J",                                                                                            (void *) &Unsafe_getAddress_address},
                                       {(char *) "putByte",                (char *) "(JB)V",                                                                                           (void *) &Unsafe_putByte_address},
                                       {(char *) "putShort",               (char *) "(JS)V",                                                                                           (void *) &Unsafe_putShort_address},
                                       {(char *) "putChar",                (char *) "(JC)V",                                                                                           (void *) &Unsafe_putChar_address},
                                       {(char *) "putInt",                 (char *) "(JI)V",                                                                                           (void *) &Unsafe_putInt_address},
                                       {(char *) "putLong",                (char *) "(JJ)V",                                                                                           (void *) &Unsafe_putLong_address},
                                       {(char *) "putFloat",               (char *) "(JF)V",                                                                                           (void *) &Unsafe_putFloat_address},
                                       {(char *) "putDouble",              (char *) "(JD)V",                                                                                           (void *) &Unsafe_putDouble_address},
                                       {(char *) "putAddress",             (char *) "(JJ)V",                                                                                           (void *) &Unsafe_putAddress_address},

                                       {(char *) "addressSize",            (char *) "()I",                                                                                             (void *) &Unsafe_addressSize},
                                       {(char *) "pageSize",               (char *) "()I",                                                                                             (void *) 0},

                                       {(char *) "allocateMemory",         (char *) "(J)J",                                                                                            (void *) &Unsafe_allocateMemory},
                                       {(char *) "reallocateMemory",       (char *) "(JJ)J",                                                                                           (void *) &Unsafe_reallocateMemory},
                                       {(char *) "freeMemory",             (char *) "(J)V",                                                                                            (void *) &Unsafe_freeMemory},
                                       {(char *) "setMemory",              (char *) "(Ljava/lang/Object;JJB)V",                                                                        (void *) 0},
                                       {(char *) "copyMemory",             (char *) "(Ljava/lang/Object;JLjava/lang/Object;JJ)V",                                                      (void *) 0},

                                       {(char *) "arrayBaseOffset",        (char *) "(Ljava/lang/Class;)I",                                                                            (void *) &Unsafe_arrayBaseOffset},
                                       {(char *) "arrayIndexScale",        (char *) "(Ljava/lang/Class;)I",                                                                            (void *) &Unsafe_arrayIndexScale},

                                       {(char *) "staticFieldBase",        (char *) "(Ljava/lang/reflect/Field;)Ljava/lang/Object;",                                                   (void *) &Unsafe_staticFieldBase},
                                       {(char *) "staticFieldOffset",      (char *) "(Ljava/lang/reflect/Field;)J",                                                                    (void *) &Unsafe_staticFieldOffset},

                                       {(char *) "objectFieldOffset",      (char *) "(Ljava/lang/reflect/Field;)J",                                                                    (void *) &Unsafe_objectFieldOffset},

                                       {(char *) "allocateInstance",       (char *) "(Ljava/lang/Class;)Ljava/lang/Object;",                                                           (void *) &Unsafe_allocateInstance},
                                       {(char *) "defineClass",            (char *) "(Ljava/lang/String;[BIILjava/lang/ClassLoader;Ljava/security/ProtectionDomain)Ljava/lang/Class;", (void *) 0},
                                       {(char *) "defineClass",            (char *) "(Ljava/lang/String;[BII)Ljava/lang/Class;",                                                       (void *) 0},
                                       {(char *) "defineAnonymousClass",   (char *) "(Ljava/lang/Class;[B[Ljava/lang/Object;)Ljava/lang/Class;",                                       (void *) 0},
                                       {(char *) "ensureClassInitialized", (char *) "(Ljava/lang/Class;)V",                                                                            (void *) &Unsafe_ensureClassInitialized},

                                       {(char *) "monitorEnter",           (char *) "(Ljava/lang/Object;)V",                                                                           (void *) &Unsafe_monitorEnter},
                                       {(char *) "monitorExit",            (char *) "(Ljava/lang/Object;)V",                                                                           (void *) &Unsafe_monitorExit},
                                       {(char *) "tryMonitorEnter",        (char *) "(Ljava/lang/Object;)Z",                                                                           (void *) &Unsafe_tryMonitorEnter},

                                       {(char *) "throwException",         (char *) "(Ljava/lang/Throwable;)V",                                                                        (void *) &Unsafe_throwException},

                                       {(char *) "compareAndSwapObject",   (char *) "(Ljava/lang/Object;JLjava/lang/Object;Ljava/lang/Object;)Z",                                      (void *) &Unsafe_compareAndSwapObject},
                                       {(char *) "compareAndSwapInt",      (char *) "(Ljava/lang/Object;JII)Z",                                                                        (void *) &Unsafe_compareAndSwapInt},
                                       {(char *) "compareAndSwapLong",     (char *) "(Ljava/lang/Object;JJJ)Z",                                                                        (void *) &Unsafe_compareAndSwapLong},

                                       {(char *) "getObjectVolatile",      (char *) "(Ljava/lang/Object;J)Ljava/lang/Object;",                                                         (void *) &Unsafe_getObject_object},
                                       {(char *) "getBooleanVolatile",     (char *) "(Ljava/lang/Object;J)Z",                                                                          (void *) &Unsafe_getBoolean_object},
                                       {(char *) "getByteVolatile",        (char *) "(Ljava/lang/Object;J)B",                                                                          (void *) &Unsafe_getByte_object},
                                       {(char *) "getShortVolatile",       (char *) "(Ljava/lang/Object;J)S",                                                                          (void *) &Unsafe_getShort_object},
                                       {(char *) "getCharVolatile",        (char *) "(Ljava/lang/Object;J)C",                                                                          (void *) &Unsafe_getChar_object},
                                       {(char *) "getIntVolatile",         (char *) "(Ljava/lang/Object;J)I",                                                                          (void *) &Unsafe_getInt_object},
                                       {(char *) "getLongVolatile",        (char *) "(Ljava/lang/Object;J)J",                                                                          (void *) &Unsafe_getLong_object},
                                       {(char *) "getFloatVolatile",       (char *) "(Ljava/lang/Object;J)F",                                                                          (void *) &Unsafe_getFloat_object},
                                       {(char *) "getDoubleVolatile",      (char *) "(Ljava/lang/Object;J)D",                                                                          (void *) &Unsafe_getDouble_object},
                                       {(char *) "putObjectVolatile",      (char *) "(Ljava/lang/Object;JLjava/lang/Object;)V",                                                        (void *) &Unsafe_putObject_object},
                                       {(char *) "putBooleanVolatile",     (char *) "(Ljava/lang/Object;JZ)V",                                                                         (void *) &Unsafe_putBoolean_object},
                                       {(char *) "putByteVolatile",        (char *) "(Ljava/lang/Object;JB)V",                                                                         (void *) &Unsafe_putByte_object},
                                       {(char *) "putShortVolatile",       (char *) "(Ljava/lang/Object;JS)V",                                                                         (void *) &Unsafe_putShort_object},
                                       {(char *) "putCharVolatile",        (char *) "(Ljava/lang/Object;JC)V",                                                                         (void *) &Unsafe_putChar_object},
                                       {(char *) "putIntVolatile",         (char *) "(Ljava/lang/Object;JI)V",                                                                         (void *) &Unsafe_putInt_object},
                                       {(char *) "putLongVolatile",        (char *) "(Ljava/lang/Object;JJ)V",                                                                         (void *) &Unsafe_putLong_object},
                                       {(char *) "putFloatVolatile",       (char *) "(Ljava/lang/Object;JF)V",                                                                         (void *) &Unsafe_putFloat_object},
                                       {(char *) "putDoubleVolatile",      (char *) "(Ljava/lang/Object;JD)V",                                                                         (void *) &Unsafe_putDouble_object},

                                       {(char *) "putOrderedObject",       (char *) "(Ljava/lang/Object;JLjava/lang/Object;)V",                                                        (void *) &Unsafe_putObject_object},
                                       {(char *) "putOrderedInt",          (char *) "(Ljava/lang/Object;JI)V",                                                                         (void *) &Unsafe_putInt_object},
                                       {(char *) "putOrderedLong",         (char *) "(Ljava/lang/Object;JJ)V",                                                                         (void *) &Unsafe_putLong_object},

                                       {(char *) "unpark",                 (char *) "(Ljava/lang/Object;)V",                                                                           (void *) 0},
                                       {(char *) "park",                   (char *) "(ZJ)V",                                                                                           (void *) 0},
                                       {(char *) "getLoadAverage",         (char *) "([DI)I",                                                                                          (void *) 0},};
    env->RegisterNatives(unsafeClass, unsafeMethods,
      METHOD_COUNT(unsafeMethods));
  }

}

#endif

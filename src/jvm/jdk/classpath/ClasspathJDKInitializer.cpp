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

#if defined(JDK_CLASSPATH)

    #include <zip.h>

    #include <jvm/Global.hpp>

    #define METHOD_COUNT(methods) (sizeof(methods) / sizeof(methods[0]))

namespace coldspot {

error_t ClasspathJDKHandler::initialize() {

  setupProperties();

  _javaLibrary = zip_open("/usr/local/classpath/share/classpath/glibj.zip", 0,
                          0);
  if (_javaLibrary == 0) {
    EXIT_FATAL("could not load java-library");
  }

  // Creates a new vm-thread, attaches it to the current native-thread
  // and binds a new java-thread to it
  error_t errorValue = createInitialThread();
  RETURN_ON_FAIL(errorValue)

  return RETURN_OK;
}


error_t ClasspathJDKHandler::release() {

  if (_javaLibrary != 0) {
    zip_close((zip*) _javaLibrary);
  }

  return RETURN_OK;
}


void* ClasspathJDKHandler::getJavaLibrary() {

  return _javaLibrary;
}


error_t ClasspathJDKHandler::createInitialThread() {

  // Create vm-thread and attach it to the native-thread
  VMThread* vmThread = new VMThread(THREADSTATE_RUNNABLE);
  error_t errorValue = _vm->attachThread(vmThread, false);
  RETURN_ON_FAIL(errorValue);

  // Java-code needs functions like arraycopy etc.
  // Therefore we have to register the vm-methods to execute java-code.
  registerVMMethods();

  // Load thread-class
  Class* threadClass;
  errorValue = _vm->getClassLoader()->loadClass(CLASSNAME_THREAD,
               &threadClass);
  RETURN_ON_FAIL(errorValue);

  // Load thread-constructor
  Method* threadConstructor;
  errorValue = threadClass->getMethod(Signature(
                                        "(Ljava/lang/VMThread;Ljava/lang/String;IZ)V", METHODNAME_CONSTRUCTOR), &threadConstructor);
  RETURN_ON_FAIL(errorValue);

  // Create thread-name
  Object* threadName;
  errorValue = Object::newString(u"main", &threadName);
  RETURN_ON_FAIL(errorValue);

  // Create thread
  Object* thread;
  Value parameters[4];
  parameters[0] = (Object*) 0;
  parameters[1] = threadName;
  parameters[2] = (jint) 5;
  parameters[3] = false;
  errorValue = Object::newObject(threadConstructor, parameters, &thread);
  RETURN_ON_FAIL(errorValue);

  // Bind java-thread to vm-thread
  vmThread->bind(thread);

  // Load thread-group-class
  Class* threadGroupClass;
  errorValue = _vm->getClassLoader()->loadClass("java/lang/ThreadGroup",
               &threadGroupClass);
  RETURN_ON_FAIL(errorValue);

  // Initialize thread-group-class
  errorValue = _vm->getClassLoader()->initializeClass(threadGroupClass);
  RETURN_ON_FAIL(errorValue);

  // Get root-field from thread-group
  Field* rootField;
  errorValue = threadGroupClass->getField(Signature("Ljava/lang/ThreadGroup;",
                                          "root"), &rootField);
  RETURN_ON_FAIL(errorValue);

  // Load root-thread-group
  Object* rootThreadGroup = rootField->getStaticValue().asObject();

  // Load group-field
  Field* groupField;
  errorValue = threadClass->getField(Signature("Ljava/lang/ThreadGroup;",
                                     "group"), &groupField);
  RETURN_ON_FAIL(errorValue);

  // Set group-field
  Value value(rootThreadGroup);
  groupField->setValue(thread, value);

  // Load addThread-method
  Method* addThreadMethod;
  errorValue = threadGroupClass->getMethod(Signature("(Ljava/lang/Thread;)V",
               "addThread"), &addThreadMethod);
  RETURN_ON_FAIL(errorValue);

  // Call addThread-method
  Value addThreadParameter = rootThreadGroup;
  errorValue = addThreadMethod->invoke(rootThreadGroup, &addThreadParameter, &value);
  RETURN_ON_FAIL(errorValue);

  return RETURN_OK;
}


void ClasspathJDKHandler::registerVMMethods() {

  JNIEnv* env = _vm->getJNIEnv();

  // Methods of gnu.classpath.VMStackWalker
  jclass stackWalkerClass = env->FindClass("gnu/classpath/VMStackWalker");
  JNINativeMethod stackWalkerMethods[] = {
    {
      (char*) "getClassContext",
      (char*) "()[Ljava/lang/Class;",
      (void*) &VMStackWalker_getClassContext
    },
    {
      (char*) "getClassLoader",
      (char*) "(Ljava/lang/Class;)Ljava/lang/ClassLoader;",
      (void*) &VMStackWalker_getClassLoader
    },
  };
  env->RegisterNatives(stackWalkerClass, stackWalkerMethods, METHOD_COUNT(stackWalkerMethods));

  // Methods of gnu.classpath.VMSystemProperties
  jclass sysPropsClass = env->FindClass("gnu/classpath/VMSystemProperties");
  JNINativeMethod sysPropsMethods[] = {
    {
      (char*) "preInit",
      (char*) "(Ljava/util/Properties;)V",
      (void*) &VMSystemProperties_preInit
    },
  };
  env->RegisterNatives(sysPropsClass, sysPropsMethods, METHOD_COUNT(sysPropsMethods));

  // Methods of java.lang.reflect.VMConstructor
  jclass ctorClass = env->FindClass("java/lang/reflect/VMConstructor");
  JNINativeMethod ctorMethods[] = {
    {
      (char*) "construct",
      (char*) "([Ljava/lang/Object;)Ljava/lang/Object;",
      (void*) &VMConstructor_construct
    },
    {
      (char*) "getModifiersInternal",
      (char*) "()I",
      (void*) &VMConstructor_getModifiersInternal
    },
    {
      (char*) "getParameterTypes",
      (char*) "()[Ljava/lang/Class;",
      (void*) &VMConstructor_getParameterTypes
    },
  };
  env->RegisterNatives(ctorClass, ctorMethods, METHOD_COUNT(ctorMethods));

  // Methods of java.lang.VMClass
  jclass classClass = env->FindClass("java/lang/VMClass");
  JNINativeMethod classMethods[] = {
    {
      (char*) "forName",
      (char*) "(Ljava/lang/String;ZLjava/lang/ClassLoader;)Ljava/lang/Class;",
      (void*) &VMClass_forName
    },
    {
      (char*) "isAssignableFrom",
      (char*) "(Ljava/lang/Class;Ljava/lang/Class;)Z",
      (void*) &VMClass_isAssignableFrom
    },
    {
      (char*) "isInstance",
      (char*) "(Ljava/lang/Class;Ljava/lang/Object;)Z",
      (void*) &VMClass_isInstance
    },
    {
      (char*) "isInterface",
      (char*) "(Ljava/lang/Class;)Z",
      (void*) &VMClass_isInterface
    },
    {
      (char*) "isPrimitive",
      (char*) "(Ljava/lang/Class;)Z",
      (void*) &VMClass_isPrimitive
    },
    {
      (char*) "getClassLoader",
      (char*) "(Ljava/lang/Class;)Ljava/lang/ClassLoader;",
      (void*) &VMClass_getClassLoader
    },
    {
      (char*) "getDeclaredConstructors",
      (char*) "(Ljava/lang/Class;Z)[Ljava/lang/reflect/Constructor;",
      (void*) &VMClass_getDeclaredConstructors
    },
    {
      (char*) "getModifiers",
      (char*) "(Ljava/lang/Class;Z)I",
      (void*) &VMClass_getModifiers
    },
    {
      (char*) "getName",
      (char*) "(Ljava/lang/Class;)Ljava/lang/String;",
      (void*) &VMClass_getName
    },
  };
  env->RegisterNatives(classClass, classMethods, METHOD_COUNT(classMethods));

  // Methods of java.lang.VMClassLoader
  jclass classLoaderClass = env->FindClass("java/lang/VMClassLoader");
  JNINativeMethod classLoaderMethods[] = {
    {
      (char*) "defineClass",
      (char*) "(Ljava/lang/ClassLoader;Ljava/lang/String;[BIILjava/security/ProtectionDomain;)Ljava/lang/Class;",
      (void*) &VMClassLoader_defineClass
    },
    {
      (char*) "findLoadedClass",
      (char*) "(Ljava/lang/ClassLoader;Ljava/lang/String;)Ljava/lang/Class;",
      (void*) &VMClassLoader_findLoadedClass
    },
    {
      (char*) "getPrimitiveClass",
      (char*) "(C)Ljava/lang/Class;",
      (void*) &VMClassLoader_getPrimitiveClass
    },
    {
      (char*) "loadClass",
      (char*) "(Ljava/lang/String;Z)Ljava/lang/Class;",
      (void*) &VMClassLoader_loadClass
    },
  };
  env->RegisterNatives(classLoaderClass, classLoaderMethods, METHOD_COUNT(classLoaderMethods));

  // Methods of java.lang.VMObject
  jclass objectClass = env->FindClass("java/lang/VMObject");
  JNINativeMethod objectMethods[] = {
    {
      (char*) "clone",
      (char*) "(Ljava/lang/Cloneable;)Ljava/lang/Object;",
      (void*) &VMObject_clone
    },
    {
      (char*) "getClass",
      (char*) "(Ljava/lang/Object;)Ljava/lang/Class;",
      (void*) &VMObject_getClass
    },
    {
      (char*) "notify",
      (char*) "(Ljava/lang/Object;)V",
      (void*) &VMObject_notify
    },
    {
      (char*) "notifyAll",
      (char*) "(Ljava/lang/Object;)V",
      (void*) &VMObject_notifyAll
    },
    {
      (char*) "wait",
      (char*) "(Ljava/lang/Object;JI)V",
      (void*) &VMObject_wait
    },
  };
  env->RegisterNatives(objectClass, objectMethods, METHOD_COUNT(objectMethods));

  // Methods of java.lang.VMRuntime
  jclass runtimeClass = env->FindClass("java/lang/VMRuntime");
  JNINativeMethod runtimeMethods[] = {
    {
      (char*) "mapLibraryName",
      (char*) "(Ljava/lang/String;)Ljava/lang/String;",
      (void*) &VMRuntime_mapLibraryName
    },
    {
      (char*) "nativeLoad",
      (char*) "(Ljava/lang/String;Ljava/lang/ClassLoader;)I",
      (void*) &VMRuntime_nativeLoad
    },
  };
  env->RegisterNatives(runtimeClass, runtimeMethods, METHOD_COUNT(runtimeMethods));

  // Methods of java.lang.VMSystem
  jclass systemClass = env->FindClass("java/lang/VMSystem");
  JNINativeMethod systemMethods[] = {
    {
      (char*) "arraycopy",
      (char*) "(Ljava/lang/Object;ILjava/lang/Object;II)V",
      (void*) &VMSystem_arraycopy
    },
    {
      (char*) "currentTimeMillis",
      (char*) "()J",
      (void*) &VMSystem_currentTimeMillis
    },
    {
      (char*) "identityHashCode",
      (char*) "(Ljava/lang/Object;)I",
      (void*) &VMSystem_identityHashCode
    },
    {
      (char*) "nanoTime",
      (char*) "()J",
      (void*) &VMSystem_nanoTime
    },
  };
  env->RegisterNatives(systemClass, systemMethods, METHOD_COUNT(systemMethods));

  // Methods of java.lang.VMThread
  jclass threadClass = env->FindClass("java/lang/VMThread");
  JNINativeMethod threadMethods[] = {
    {
      (char*) "currentThread",
      (char*) "()Ljava/lang/Thread;",
      (void*) &VMThread_currentThread
    },
    {
      (char*) "start",
      (char*) "(J)V",
      (void*) &VMThread_start
    },
  };
  env->RegisterNatives(threadClass, threadMethods, METHOD_COUNT(threadMethods));

  // Methods of java.lang.VMThrowable
  jclass throwableClass = env->FindClass("java/lang/VMThrowable");
  JNINativeMethod throwableMethods[] = {
    {
      (char*) "fillInStackTrace",
      (char*) "(Ljava/lang/Throwable;)Ljava/lang/VMThrowable;",
      (void*) &VMThrowable_fillInStackTrace
    },
    {
      (char*) "getStackTrace",
      (char*) "(Ljava/lang/Throwable;)[Ljava/lang/StackTraceElement;",
      (void*) &VMThrowable_getStackTrace
    },
  };
  env->RegisterNatives(throwableClass, throwableMethods, METHOD_COUNT(throwableMethods));
}


void ClasspathJDKHandler::setupProperties() {

  Options* options = _vm->getOptions();

  options->setPropertyIfEmpty("java.ext.dirs", ".");
  options->setPropertyIfEmpty("java.io.tmpdir", ".");
  options->setPropertyIfEmpty("java.library.path",
                              "/usr/local/classpath/lib/classpath");  // TODO mac
  options->setPropertyIfEmpty("gnu.cpu.endian", "little");
}

}

#endif

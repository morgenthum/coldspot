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

    #include <jvm/Global.hpp>

using namespace coldspot;


jobjectArray VMStackWalker_getClassContext(JNIEnv* env, jclass clazz) {

  auto& frames = _currentExecutor->get_frames();
  jint size = frames.size(); // TODO size - 2
  jclass componentClass = env->FindClass("java/lang/Class");
  jobjectArray array = env->NewObjectArray(size, componentClass, 0);

  int i = 0;
  auto frameIter = frames.begin();
  while (frameIter != frames.end()) {
    Frame* frame = (Frame*) *frameIter;
    jobject clazz = _vm->addLocalReference(frame->clazz->javaClass);
    env->SetObjectArrayElement(array, i++, clazz);
    ++frameIter;
  }
  return array;
}


jobject VMStackWalker_getClassLoader(JNIEnv* env, jclass classObj,
                                     jclass checkClassObj) {

  Class* checkClass = Class::fromClassObject(checkClassObj);
  Object* checkClassLoader = checkClass->classLoader;

  if (checkClassLoader != 0) {
    return _vm->addLocalReference(checkClassLoader);
  }

  return 0;
}


void VMSystemProperties_preInit(JNIEnv* env, jclass classObj, jobject properties) {

  jclass propertiesClass = env->GetObjectClass(properties);
  jmethodID method = env->GetMethodID(
                       propertiesClass, "setProperty",
                       "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");

  auto& systemProperties = _vm->getOptions()->systemProperties;

  auto iterator = systemProperties.begin();
  while (iterator != systemProperties.end()) {
    String key(iterator->key);
    String value(iterator->value);
    env->CallVoidMethod(properties, method, env->NewStringUTF(key.toCString()),
                        env->NewStringUTF(value.toCString()));
    if (ExceptionCheck(env)) {
      return;
    }

    ++iterator;
  }
}


Method* getConstructorMethod(JNIEnv* env, jobject self) {

  jclass selfClass = env->GetObjectClass(self);
  jfieldID clazzField = env->GetFieldID(selfClass, "clazz",
                                        "Ljava/lang/Class;");
  jfieldID slotField = env->GetFieldID(selfClass, "slot", "I");

  jclass handlingClass = (jclass) env->GetObjectField(self, clazzField);
  jint slot = env->GetIntField(self, slotField);

  Class* internHandlingClass = Class::fromClassObject(handlingClass);
  List<Method*> internMethods;
  internHandlingClass->getDeclaredMethods(internMethods);

  Method* internHandlingConstructor = 0;

  for (auto internMethod : internMethods) {
    if (internMethod->getSlot() == slot) {
      internHandlingConstructor = internMethod;
      break;
    }
  }

  return internHandlingConstructor;
}


jobject VMConstructor_construct(JNIEnv* env, jobject self, jobjectArray args) {

  Method* internConstructor = getConstructorMethod(env, self);

  jclass selfClass = env->GetObjectClass(self);
  jfieldID clazzField = env->GetFieldID(selfClass, "clazz",
                                        "Ljava/lang/Class;");
  jclass handlingClass = (jclass) env->GetObjectField(self, clazzField);

  jobject object = env->AllocObject(handlingClass);

  Value value;
  jsize length = env->GetArrayLength(args);
  Value* parameters = (Value*) alloca(sizeof(Value) * length);
  for (jsize i = 0; i < length; ++i) {
    jobject arg = env->GetObjectArrayElement(args, i);
    Value valueArg(arg);
    parameters[i] = valueArg;
  }

  internConstructor->unboxParameters(parameters);

  internConstructor->invoke(object, parameters, &value);

  return object;
}


jint VMConstructor_getModifiersInternal(JNIEnv* env, jobject self) {

  return getConstructorMethod(env, self)->getMethodInfo()->accessFlags;
}


jarray VMConstructor_getParameterTypes(JNIEnv* env, jobject self) {

  jobjectArray parameterTypes = 0;

  Method* internHandlingConstructor = getConstructorMethod(env, self);

  jclass classClass = env->FindClass(CLASSNAME_CLASS);

  if (internHandlingConstructor == 0) {
    parameterTypes = env->NewObjectArray(0, classClass, 0);
  } else {
    List<TypeInfo> parameterTypeInfos = internHandlingConstructor
                                        ->getParameterTypes();
    parameterTypes = env->NewObjectArray(parameterTypeInfos.size(), classClass,
                                         0);
    uint32_t index = 0;
    for (auto parameterTypeInfo : parameterTypeInfos) {
      Class* parameterClass = 0;
      if (parameterTypeInfo.isPrimitive()) {
        const char* typeName = TYPE_NAMES[parameterTypeInfo.type];
        _vm->getClassLoader()->loadPrimitive(typeName, &parameterClass);
      } else {
        parameterClass = parameterTypeInfo.clazz;
      }
      jclass parameterTypeClass = (jclass) _vm->addLocalReference(
                                    parameterClass->javaClass);
      env->SetObjectArrayElement(parameterTypes, index++, parameterTypeClass);
    }
  }

  return parameterTypes;
}


jobject VMClass_forName(JNIEnv* env, jclass clazz, jstring name, jboolean init,
                        jobject loader) {

  const jchar* nameChars = env->GetStringChars(name, 0);
  UTF16String utf16Name(nameChars);
  env->ReleaseStringChars(name, nameChars);

  String className = utf16Name;
  className.replace('.', '/');

  Class* loadedClass;
  error_t errorValue = _vm->getClassLoader()->loadClass(className, loader,
                       &loadedClass);
  if (errorValue != RETURN_OK) {
    return 0;
  }

  return _vm->addLocalReference(loadedClass->javaClass);
}


jboolean VMClass_isAssignableFrom(JNIEnv* env, jclass clazz,
                                  jclass callingClassObj, jclass checkClassObj) {

  Class* callingClass = Class::fromClassObject(callingClassObj);
  Class* checkClass = Class::fromClassObject(checkClassObj);

  return checkClass->isCastableTo(callingClass);
}


jboolean VMClass_isInstance(JNIEnv* env, jclass clazz, jclass callingClassObj,
                            jobject object) {

  if (object == 0) {
    return false;
  }

  Class* callingClass = Class::fromClassObject(callingClassObj);

  return object->getClass()->isCastableTo(callingClass);
}


jboolean VMClass_isInterface(JNIEnv* env, jclass clazz, jclass checkClassObj) {

  Class* checkClass = Class::fromClassObject(checkClassObj);

  if (checkClass == 0) {
    return false;
  }

  return checkClass->isInterface();
}


jboolean VMClass_isPrimitive(JNIEnv* env, jclass clazz, jclass checkClassObj) {

  Class* checkClass = Class::fromClassObject(checkClassObj);

  if (checkClass == 0) {
    return false;
  }

  return checkClass->isPrimitive();
}


jobject VMClass_getClassLoader(JNIEnv* env, jclass clazz, jclass callingClassObj) {

  return Class::fromClassObject(callingClassObj)->classLoader;
}


jobject VMClass_getDeclaredConstructors(JNIEnv* env, jclass clazz,
                                        jclass checkClazz,
                                        jboolean publicOnly) {

  Class* internClass = Class::fromClassObject(checkClazz);

  List<Method*> internConstructors;
  List<Method*> internMethods;

  internClass->getDeclaredMethods(internMethods);

  for (auto internMethod : internMethods) {
    if (internMethod->getSignature().name == METHODNAME_CONSTRUCTOR) {
      if (publicOnly ? internMethod->isPublic() : true) {
        internConstructors.addBack(internMethod);
      }
    }
  }

  jclass constructorClass = env->FindClass("java/lang/reflect/Constructor");
  jclass vmConstructorClass = env->FindClass(
                                "java/lang/reflect/VMConstructor");

  jobjectArray declaredConstructors = env->NewObjectArray(
                                        internConstructors.size(), constructorClass, 0);

  jmethodID vmConstructorMethod = env->GetMethodID(vmConstructorClass, METHODNAME_CONSTRUCTOR,
                                  "(Ljava/lang/Class;I)V");
  jmethodID constructorMethod = env->GetMethodID(
                                  constructorClass, METHODNAME_CONSTRUCTOR, "(Ljava/lang/reflect/VMConstructor;)V");

  jint i = 0;
  for (auto internConstructor : internConstructors) {
    jobject vmConstructor = env->NewObject(vmConstructorClass,
                                           vmConstructorMethod, checkClazz,
                                           internConstructor->getSlot());
    jobject constructor = env->NewObject(constructorClass, constructorMethod,
                                         vmConstructor);
    env->SetObjectArrayElement(declaredConstructors, i++, constructor);
  }

  return declaredConstructors;
}


jint VMClass_getModifiers(JNIEnv* env, jclass clazz, jclass checkClazz,
                          jboolean ignoreInnerClassesAttrib) {

  Class* internClass = Class::fromClassObject(checkClazz);
  return internClass->classFile->accessFlags;
}


jstring VMClass_getName(JNIEnv* env, jclass clazz, jclass checkClassObj) {

  Class* checkClass = Class::fromClassObject(checkClassObj);

  if (checkClass != 0) {
    if (checkClass->primitive) {
      if (checkClass->name == "Z") {
        return env->NewStringUTF("boolean");
      } else if (checkClass->name == "B") {
        return env->NewStringUTF("byte");
      } else if (checkClass->name == "C") {
        return env->NewStringUTF("char");
      } else if (checkClass->name == "S") {
        return env->NewStringUTF("short");
      } else if (checkClass->name == "I") {
        return env->NewStringUTF("int");
      } else if (checkClass->name == "J") {
        return env->NewStringUTF("long");
      } else if (checkClass->name == "F") {
        return env->NewStringUTF("float");
      } else if (checkClass->name == "D") {
        return env->NewStringUTF("double");
      } else if (checkClass->name == "V") {
        return env->NewStringUTF("void");
      }
    } else {
      String name = checkClass->name;
      name.replace('/', '.');
      return env->NewStringUTF(name.toCString());
    }
  }

  return 0;
}


jclass VMClassLoader_defineClass(JNIEnv* env, jclass clazz, jclass classLoader,
                                 jstring name, jbyteArray data, jint offset,
                                 jint len, jobject protectionDomain) {

  const char* nameChars = 0;
  const jbyte* bytes = (jbyte*) data->getMemory();

  if (name != 0) {
    nameChars = env->GetStringUTFChars(name, 0);
  }

  return env->DefineClass(nameChars, classLoader, bytes, len);
}


jclass VMClassLoader_findLoadedClass(JNIEnv* env, jclass clazz,
                                     jobject classLoader, jstring name) {

  const jchar* nameChars = env->GetStringChars(name, 0);
  UTF16String utf16Name(nameChars);
  env->ReleaseStringChars(name, nameChars);

  auto& loadedClasses = _vm->getClassLoader()->getLoadedClasses();
  auto entry = loadedClasses.get(ClassIdentifier(classLoader, utf16Name));

  if (entry != 0) {
    return (jclass) _vm->addLocalReference(entry->value->javaClass);
  }

  return 0;
}


jclass VMClassLoader_getPrimitiveClass(JNIEnv* env, jclass clazz, jchar type) {

  StringBuilder builder;
  builder << (char) type;

  return env->FindClass(builder.toString().toCString());
}


jclass VMClassLoader_loadClass(JNIEnv* env, jclass clazz, jstring name,
                               jboolean resolve) {

  const jchar* nameChars = env->GetStringChars(name, 0);
  UTF16String utf16Name(nameChars);
  env->ReleaseStringChars(name, nameChars);

  String className = utf16Name;
  className.replace('.', '/');

  Class* internClass;
  error_t errorValue = _vm->getClassLoader()->loadClass(className,
                       &internClass);
  if (errorValue == RETURN_OK) {
    return (jclass) _vm->addLocalReference(internClass->javaClass);
  }

  return 0;
}


jobject VMObject_clone(JNIEnv* env, jclass clazz, jobject cloneable) {

  Object* clone;
  cloneable->clone(&clone);

  return _vm->addLocalReference(clone);
}


jobject VMObject_getClass(JNIEnv* env, jclass clazz, jobject object) {

  if (object == 0) {
    return 0;
  }

  Class* objectClass = object->getClass();
  return _vm->addLocalReference(objectClass->javaClass);
}


void VMObject_notify(JNIEnv* env, jclass clazz, jobject object) {

  object->getMonitor()->notify();
}


void VMObject_notifyAll(JNIEnv* env, jclass clazz, jobject object) {

  object->getMonitor()->notifyAll();
}


void VMObject_wait(JNIEnv* env, jclass clazz, jobject object, jlong ms, jint ns) {

  object->getMonitor()->wait(ms);
}


jstring VMRuntime_mapLibraryName(JNIEnv* env, jclass clazz, jstring libname) {

  const jchar* libnameChars = env->GetStringChars(libname, 0);
  UTF16String utf16Libname(libnameChars);
  env->ReleaseStringChars(libname, libnameChars);

  StringBuilder builder;
  builder << "lib" << String(utf16Libname) << ".dylib";

  UTF16String mappedLibname = builder.toString();
  return env->NewString(mappedLibname.toCString(), mappedLibname.length());
}


jint VMRuntime_nativeLoad(JNIEnv* env, jclass clazz, jstring filename,
                          jobject classLoader) {

  const jchar* filenameChars = env->GetStringChars(filename, 0);
  UTF16String utf16Filename(filenameChars);
  env->ReleaseStringChars(filename, filenameChars);

  Library_t library = _vm->getLibraryBinder()->loadLibrary(utf16Filename);
  return library == 0 ? 0 : 1;
}


void VMSystem_arraycopy(JNIEnv* env, jclass clazz, jobjectArray src,
                        jint srcStart, jobjectArray dest, jint destStart,
                        jint length) {

  Array::copyElements(src, dest, srcStart, destStart, length);
}


jlong VMSystem_currentTimeMillis(JNIEnv* env, jclass clazz) {

  return System::millis();
}


jint VMSystem_identityHashCode(JNIEnv* env, jclass clazz, jobject object) {

  return object == 0 ? 0 : object->identityHashCode();
}


jlong VMSystem_nanoTime(JNIEnv* env, jclass clazz) {

  return System::millis() * 1000000;
}


jobject VMThread_currentThread(JNIEnv* env, jclass clazz) {

  VMThread* current = static_cast<VMThread*>(_current_thread);
  Object* javaThread = current->getJavaThread();

  return _vm->addLocalReference(javaThread);
}


void VMThread_start(JNIEnv* env, jobject self, jlong stacksize) {

  // Load thread-field of VMThread
  Field* threadField;
  error_t errorValue = self->getClass()->getField(Signature("Ljava/lang/Thread;",
                       "thread"), &threadField);
  if (errorValue != RETURN_OK) {
    env->FatalError("Could not create native thread");
  }

  // Get value from thread-field
  Object* javaThread = threadField->getValue(self).asObject();

  // Create native thread
  VMThread* thread;
  errorValue = _vm->createVMThread(javaThread, &thread);
  if (errorValue != RETURN_OK) {
    env->FatalError("Could not create native thread");
  }

  // Load run-method from VMThread
  Method* runMethod;
  errorValue = self->getClass()->getMethod(Signature("()V", "run"), &runMethod);
  if (errorValue != RETURN_OK) {
    env->FatalError("Could not create native thread");
  }

  // Check if the thread is a daemon
  bool daemon;
  errorValue = Thread::isDaemon(self, &daemon);
  if (errorValue != RETURN_OK) {
    env->FatalError("Could not check if the thread is a daemon-thread");
  }

  // Start the new thread
  thread->start(daemon, runMethod, self);
}


jobject VMThrowable_fillInStackTrace(JNIEnv* env, jclass clazz,
                                     jobject throwable) {

  jmethodID vmThrowableCtor = env->GetMethodID(clazz, METHODNAME_CONSTRUCTOR, "()V");
  jobject vmThrowable = env->NewObject(clazz, vmThrowableCtor);
  Executor* executor = _currentExecutor;
  auto& frames = executor->getFrames();
  jclass elementClass = env->FindClass("java/lang/StackTraceElement");
  jmethodID constructor = env->GetMethodID(
                            elementClass, METHODNAME_CONSTRUCTOR,
                            "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Z)V");
  jobjectArray elements = env->NewObjectArray(frames.size(), elementClass, 0);

  int i = 0;
  auto frameIter = frames.begin();
  while (frameIter != frames.end()) {
    Frame* frame = (Frame*) *frameIter;
    UTF16String sourceFile = frame->clazz->sourceFile;
    jstring fileName = env->NewString(sourceFile.toCString(),
                                      sourceFile.length());
    jint lineNumber = 0;
    uint32_t programCounter = CURRENT_PC(frame);
    for (uint32_t i = programCounter; i <= programCounter; --i) {
      auto entry = frame->method->getLineMapping().get(i);
      if (entry != 0) {
        lineNumber = entry->value;
        break;
      }
    }
    UTF16String javaClassName = Class::toJavaClassName(frame->clazz->name);
    jstring className = env->NewString(javaClassName.toCString(),
                                       javaClassName.length());
    UTF16String utf16MethodName = frame->method->getSignature().name;
    jstring methodName = env->NewString(
                           utf16MethodName.toCString(),
                           utf16MethodName.length());
    jboolean native = frame->type == FrameType::FRAMETYPE_NATIVE;
    jobject element = env->NewObject(elementClass, constructor, fileName,
                                     lineNumber, className, methodName, native);
    env->SetObjectArrayElement(elements, i++, element);
    ++frameIter;
  }

  jfieldID vmdataField = env->GetFieldID(clazz, "vmdata", "Ljava/lang/Object;");
  env->SetObjectField(vmThrowable, vmdataField, elements);
  return vmThrowable;
}


jobject VMThrowable_getStackTrace(JNIEnv* env, jobject self,
                                  jobject throwable) {

  jclass clazz = env->GetObjectClass(self);
  jfieldID vmdataField = env->GetFieldID(clazz, "vmdata", "Ljava/lang/Object;");
  return env->GetObjectField(self, vmdataField);
}

#endif

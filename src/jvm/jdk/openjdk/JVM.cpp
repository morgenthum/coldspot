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

    #include <cmath>
    #include <unistd.h>

    #include <jvm/Global.hpp>

using namespace coldspot;


char *copyFromString(const String &string)
{
  char *copy = new char[string.length() + 1];

  memcpy(copy, string.c_str(), string.length());
  copy[string.length()] = 0;

  return copy;
}


JNIEXPORT void JNICALL JVM_SetNativeThreadName() { }


JNIEXPORT jclass JNICALL JVM_FindClassFromCaller(JNIEnv *env, const char *name,
  jboolean init, jobject loader, jclass caller)
{
  String string(name);
  UTF16String utf16String(string);

  Class *clazz;
  error_t errorValue = _vm->class_loader()->load_class(utf16String, loader,
    &clazz);
  RETURN_VALUE_ON_FAIL(errorValue, 0)

  if (init)
  {
    errorValue = _vm->class_loader()->initialize_class(clazz);
    RETURN_VALUE_ON_FAIL(errorValue, 0)
  }

  return clazz->object;
}


JNIEXPORT jint JNICALL JVM_GetInterfaceVersion()
{
  return JVM_INTERFACE_VERSION;
}


JNIEXPORT jint JNICALL JVM_IHashCode(JNIEnv *env, jobject obj)
{
  return obj ? obj->identity_hash_code() : 0;
}


JNIEXPORT void JNICALL JVM_MonitorWait(JNIEnv *env, jobject obj, jlong ms)
{
  if (obj)
  {
    obj->ensure_monitor()->wait(ms);
  }
}


JNIEXPORT void JNICALL JVM_MonitorNotify(JNIEnv *env, jobject obj)
{
  if (obj)
  {
    obj->ensure_monitor()->notify();
  }
}


JNIEXPORT void JNICALL JVM_MonitorNotifyAll(JNIEnv *env, jobject obj)
{
  if (obj)
  {
    obj->ensure_monitor()->notify_all();
  }
}


JNIEXPORT jobject JNICALL JVM_Clone(JNIEnv *env, jobject obj)
{
  if (obj)
  {
    Object *clone;
    error_t errorValue = obj->clone(&clone);
    RETURN_VALUE_ON_FAIL(errorValue, 0)

    return env->NewLocalRef(clone);
  }

  return 0;
}


JNIEXPORT jstring JNICALL JVM_InternString(JNIEnv *env, jstring str)
{
  if (!str)
  {
    return 0;
  }

  jchar *chars;
  jint length;
  error_t errorValue = java_lang_String::chars(str, &chars, &length);
  RETURN_VALUE_ON_FAIL(errorValue, 0)

  UTF16String utf16String(chars, length);

  Object *internString;
  errorValue = java_lang_String::intern(utf16String, &internString);
  RETURN_VALUE_ON_FAIL(errorValue, 0)

  return internString;
}


JNIEXPORT jlong JNICALL JVM_CurrentTimeMillis(JNIEnv *env, jclass ignored)
{
  return System::millis();
}


JNIEXPORT jlong JNICALL JVM_NanoTime(JNIEnv *env, jclass ignored)
{
  return System::millis() * 1000000;
}


JNIEXPORT void JNICALL JVM_ArrayCopy(JNIEnv *env, jclass ignored, jobject src,
  jint src_pos, jobject dst, jint dst_pos, jint length)
{
  Array::copy_elements((jarray) src, (jarray) dst, src_pos, dst_pos, length);
}


JNIEXPORT jobject JNICALL JVM_InitProperties(JNIEnv *env, jobject properties)
{
  LOG_INFO("JVM_InitProperties called")

  jclass propertiesClass = env->GetObjectClass(properties);
  jmethodID method = env->GetMethodID(propertiesClass, "setProperty",
    "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");

  auto begin = _vm->options()->systemProperties.begin();
  auto end = _vm->options()->systemProperties.end();

  while (begin != end)
  {
    env->CallObjectMethod(properties, method,
      env->NewStringUTF(begin->key.c_str()),
      env->NewStringUTF(begin->value.c_str()));
    if (ExceptionCheck(env))
    {
      LOG_ERROR("JVM_InitProperties failed");
    }

    ++begin;
  }

  return properties;
}


JNIEXPORT void JNICALL JVM_OnExit(void (*func)())
{
  LOG_ERROR("ignoring JVM_OnExit") // TODO
}


JNIEXPORT void JNICALL JVM_Exit(jint code)
{
  LOG_ERROR("ignoring JVM_Exit") // TODO
}


JNIEXPORT void JNICALL JVM_Halt(jint code)
{
  LOG_ERROR("ignoring JVM_Halt") // TODO
}


JNIEXPORT void JNICALL JVM_GC()
{
  LOG_ERROR("ignoring JVM_GC")
}


JNIEXPORT jlong JNICALL JVM_MaxObjectInspectionAge()
{
  LOG_ERROR("ignoring JVM_MaxObjectInspectionAge")
  return 0;
}


JNIEXPORT void JNICALL JVM_TraceInstructions(jboolean on)
{
  LOG_ERROR("ignoring JVM_TraceInstructions")
}


JNIEXPORT void JNICALL JVM_TraceMethodCalls(jboolean on)
{
  LOG_ERROR("ignoring JVM_TraceMethodCalls")
}


JNIEXPORT jlong JNICALL JVM_TotalMemory()
{
  LOG_ERROR("ignoring JVM_TotalMemory")
  return 0;
}


JNIEXPORT jlong JNICALL JVM_FreeMemory()
{
  LOG_ERROR("ignoring JVM_FreeMemory")
  return 0;
}


JNIEXPORT jlong JNICALL JVM_MaxMemory()
{
  LOG_ERROR("ignoring JVM_MaxMemory")
  return 0;
}


JNIEXPORT jint JNICALL JVM_ActiveProcessorCount()
{
  LOG_ERROR("ignoring JVM_ActiveProcessorCount")
  return 0;
}


JNIEXPORT void *JNICALL JVM_LoadLibrary(const char *name)
{
  return (void *) _vm->library_binder()->load_lib(name);
}


JNIEXPORT void JNICALL JVM_UnloadLibrary(void *handle)
{
  LOG_ERROR("ignoring JVM_UnloadLibrary")
}


JNIEXPORT void *JNICALL JVM_FindLibraryEntry(void *handle, const char *name)
{
  return (void *) _vm->library_binder()->get_function(name);
}


JNIEXPORT jboolean JNICALL JVM_IsSupportedJNIVersion(jint version)
{
  return _vm->jni_version_supported(version);
}


JNIEXPORT jboolean JNICALL JVM_IsNaN(jdouble d)
{
  return std::isnan(d);
}


JNIEXPORT void JNICALL JVM_FillInStackTrace(JNIEnv *env, jobject throwable)
{
  LOG_ERROR("ignoring JVM_FillInStackTrace")
}


JNIEXPORT void JNICALL JVM_PrintStackTrace(JNIEnv *env, jobject throwable,
  jobject printable)
{
  LOG_ERROR("ignoring JVM_PrintStackTrace")
}


JNIEXPORT jint JNICALL JVM_GetStackTraceDepth(JNIEnv *env, jobject throwable)
{
  LOG_ERROR("ignoring JVM_GetStackTraceDepth")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_GetStackTraceElement(JNIEnv *env,
  jobject throwable, jint index)
{
  LOG_ERROR("ignoring JVM_GetStackTraceElement")
  return 0;
}


JNIEXPORT void JNICALL JVM_InitializeCompiler(JNIEnv *env, jclass compCls)
{
  LOG_ERROR("ignoring JVM_InitializeCompiler")
}


JNIEXPORT jboolean JNICALL JVM_IsSilentCompiler(JNIEnv *env, jclass compCls)
{
  LOG_ERROR("ignoring JVM_IsSilentCompiler")
  return false;
}


JNIEXPORT jboolean JNICALL JVM_CompileClass(JNIEnv *env, jclass compCls,
  jclass cls)
{
  LOG_ERROR("ignoring JVM_CompileClass")
  return false;
}


JNIEXPORT jboolean JNICALL JVM_CompileClasses(JNIEnv *env, jclass cls,
  jstring jname)
{
  LOG_ERROR("ignoring JVM_CompileClasses")
  return false;
}


JNIEXPORT jobject JNICALL JVM_CompilerCommand(JNIEnv *env, jclass compCls,
  jobject arg)
{
  LOG_ERROR("ignoring JVM_CompilerCommand")
  return 0;
}


JNIEXPORT void JNICALL JVM_EnableCompiler(JNIEnv *env, jclass compCls)
{
  LOG_ERROR("ignoring JVM_EnableCompiler")
}


JNIEXPORT void JNICALL JVM_DisableCompiler(JNIEnv *env, jclass compCls)
{
  LOG_ERROR("ignoring JVM_DisableCompiler")
}


JNIEXPORT void JNICALL JVM_StartThread(JNIEnv *env, jobject javaThread)
{
  // Create native thread
  VMThread *thread;
  error_t errorValue = _vm->create_vm_thread(javaThread, &thread);
  if (errorValue != RETURN_OK)
  {
    env->FatalError("could not create native thread");
  }

  // Load run-method of Thread
  Method *runMethod;
  errorValue = javaThread->type()->get_method(Signature("()V", "run"),
    &runMethod);
  if (errorValue != RETURN_OK)
  {
    env->FatalError("could not load run-method of thread");
  }

  jboolean daemon;
  errorValue = java_lang_Thread::isDaemon(javaThread, &daemon);
  if (errorValue != RETURN_OK)
  {
    env->FatalError("could not check if the java-thread is a daemon");
  }

  // Start the new thread
  thread->start(daemon, runMethod, javaThread);
}


JNIEXPORT void JNICALL JVM_StopThread(JNIEnv *env, jobject thread,
  jobject exception)
{
  LOG_ERROR("ignoring JVM_StopThread")
}


JNIEXPORT jboolean JNICALL JVM_IsThreadAlive(JNIEnv *env, jobject threadObj)
{
  VMThread *thread = VMThread::from_object(threadObj);
  if (thread != 0)
  {
    return thread->is_alive();
  }

  return false;
}


JNIEXPORT void JNICALL JVM_SuspendThread(JNIEnv *env, jobject thread)
{
  LOG_ERROR("ignoring JVM_SuspendThread")
}


JNIEXPORT void JNICALL JVM_ResumeThread(JNIEnv *env, jobject thread)
{
  LOG_ERROR("ignoring JVM_ResumeThread")
}


JNIEXPORT void JNICALL JVM_SetThreadPriority(JNIEnv *env, jobject thread,
  jint priority)
{
  java_lang_Thread::setPriority(thread, priority);
}


JNIEXPORT void JNICALL JVM_Yield(JNIEnv *env, jclass threadClass)
{
  System::yield();
}


JNIEXPORT void JNICALL JVM_Sleep(JNIEnv *env, jclass threadClass, jlong millis)
{
  System::sleep(millis);
}


JNIEXPORT jobject JNICALL JVM_CurrentThread(JNIEnv *env, jclass threadClass)
{
  VMThread *current = (VMThread *) _current_thread;
  Object *javaThread = current->object();

  return env->NewLocalRef(javaThread);
}


JNIEXPORT jint JNICALL JVM_CountStackFrames(JNIEnv *env, jobject threadObj)
{
  VMThread *thread = VMThread::from_object(threadObj);
  if (thread)
  {
    return thread->executor()->frames().size();
  }

  return 0;
}


JNIEXPORT void JNICALL JVM_Interrupt(JNIEnv *env, jobject thread)
{
  LOG_ERROR("ignoring JVM_Interrupt")
}


JNIEXPORT jboolean JNICALL JVM_IsInterrupted(JNIEnv *env, jobject thread,
  jboolean clearInterrupted)
{
  LOG_ERROR("ignoring JVM_IsInterrupted")
  return false;
}


JNIEXPORT jboolean JNICALL JVM_HoldsLock(JNIEnv *env, jclass threadClass,
  jobject obj)
{
  LOG_ERROR("ignoring JVM_HoldsLock")
  return false;
}


JNIEXPORT void JNICALL JVM_DumpAllStacks(JNIEnv *env, jclass unused)
{
  LOG_ERROR("ignoring JVM_DumpAllStacks")
}


JNIEXPORT jobjectArray JNICALL JVM_GetAllThreads(JNIEnv *env, jclass dummy)
{
  LOG_ERROR("ignoring JVM_GetAllThreads")
  return 0;
}


JNIEXPORT jobjectArray JNICALL JVM_DumpThreads(JNIEnv *env, jclass threadClass,
  jobjectArray threads)
{
  LOG_ERROR("ignoring JVM_DumpThreads")
  return 0;
}


JNIEXPORT jclass JNICALL JVM_CurrentLoadedClass(JNIEnv *env)
{
  LOG_ERROR("ignoring JVM_CurrentLoadedClass")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_CurrentClassLoader(JNIEnv *env)
{
  LOG_ERROR("ignoring JVM_CurrentClassLoader")
  return 0;
}


JNIEXPORT jobjectArray JNICALL JVM_GetClassContext(JNIEnv *env)
{
  LOG_ERROR("ignoring JVM_GetClassContext")
  return 0;
}


JNIEXPORT jint JNICALL JVM_ClassDepth(JNIEnv *env, jstring name)
{
  LOG_ERROR("ignoring JVM_ClassDepth")
  return 0;
}


JNIEXPORT jint JNICALL JVM_ClassLoaderDepth(JNIEnv *env)
{
  LOG_ERROR("ignoring JVM_ClassLoaderDepth")
  return 0;
}


JNIEXPORT jstring JNICALL JVM_GetSystemPackage(JNIEnv *env, jstring name)
{
  LOG_ERROR("ignoring JVM_GetSystemPackage")
  return 0;
}


JNIEXPORT jobjectArray JNICALL JVM_GetSystemPackages(JNIEnv *env)
{
  LOG_ERROR("ignoring JVM_GetSystemPackages")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_AllocateNewObject(JNIEnv *env, jobject obj,
  jclass currClass, jclass initClass)
{
  LOG_ERROR("ignoring JVM_AllocateNewObject")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_AllocateNewArray(JNIEnv *env, jobject obj,
  jclass currClass, jint length)
{
  LOG_ERROR("ignoring JVM_AllocateNewArray")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_LatestUserDefinedLoader(JNIEnv *env)
{
  LOG_ERROR("ignoring JVM_LatestUserDefinedLoader")
  return 0;
}


JNIEXPORT jclass JNICALL JVM_LoadClass0(JNIEnv *env, jobject obj,
  jclass currClass, jstring currClassName)
{
  LOG_ERROR("ignoring JVM_LoadClass0")
  return 0;
}


JNIEXPORT jint JNICALL JVM_GetArrayLength(JNIEnv *env, jobject arr)
{
  return env->GetArrayLength((jarray) arr);
}


JNIEXPORT jobject JNICALL JVM_GetArrayElement(JNIEnv *env, jobject arr,
  jint index)
{
  return env->GetObjectArrayElement((jobjectArray) arr, index);
}


JNIEXPORT jvalue JNICALL JVM_GetPrimitiveArrayElement(JNIEnv *env, jobject arr,
  jint index, jint wCode)
{
  LOG_ERROR("ignoring JVM_GetPrimitiveArrayElement")
  jvalue v;
  return v;
}


JNIEXPORT void JNICALL JVM_SetArrayElement(JNIEnv *env, jobject arr, jint index,
  jobject val)
{
  env->SetObjectArrayElement((jobjectArray) arr, index, val);
}


JNIEXPORT void JNICALL JVM_SetPrimitiveArrayElement(JNIEnv *env, jobject arr,
  jint index, jvalue v, unsigned char vCode)
{
  LOG_ERROR("ignoring JVM_SetPrimitiveArrayElement")
}


JNIEXPORT jobject JNICALL JVM_NewArray(JNIEnv *env, jclass element_class_obj,
  jint length)
{
  Array *array;
  Class *element_class = Class::from_class_object(element_class_obj);
  error_t error_value = Array::new_array(element_class, length, &array);
  if (error_value != RETURN_OK)
  {
    return 0;
  }

  return env->NewLocalRef(array);
}


JNIEXPORT jobject JNICALL JVM_NewMultiArray(JNIEnv *env, jclass eltClass,
  jintArray dim)
{
  LOG_ERROR("ignoring JVM_NewMultiArray")
  return 0;
}


JNIEXPORT jclass JNICALL JVM_GetCallerClass(JNIEnv *env, int n)
{
  auto &frames = _current_executor->frames();

  auto begin = frames.begin();
  auto end = frames.end();

  while (begin != end && n-- != 0)
  {
    ++begin;
  }

  if (begin != end)
  {
    Frame *frame = (Frame *) *begin;
    return frame->clazz->object;
  }

  return 0;
}


JNIEXPORT jclass JNICALL JVM_FindPrimitiveClass(JNIEnv *env, const char *utf)
{
  String name(utf);

  if (name == "void")
  {
    return PrimitiveClass<void>::get()->object;
  }
  else if (name == "boolean")
  {
    return PrimitiveClass<jboolean>::get()->object;
  }
  else if (name == "byte")
  {
    return PrimitiveClass<jbyte>::get()->object;
  }
  else if (name == "char")
  {
    return PrimitiveClass<jchar>::get()->object;
  }
  else if (name == "short")
  {
    return PrimitiveClass<jshort>::get()->object;
  }
  else if (name == "int")
  {
    return PrimitiveClass<jint>::get()->object;
  }
  else if (name == "float")
  {
    return PrimitiveClass<jfloat>::get()->object;
  }
  else if (name == "long")
  {
    return PrimitiveClass<jlong>::get()->object;
  }
  else if (name == "double")
  {
    return PrimitiveClass<jdouble>::get()->object;
  }
  else
  {
    EXIT_FATAL("JVM_FindPrimitiveClass failed: " << name.c_str());
  }

  return 0;
}


JNIEXPORT void JNICALL JVM_ResolveClass(JNIEnv *env, jclass cls)
{
}


JNIEXPORT jclass JNICALL JVM_FindClassFromBootLoader(JNIEnv *env,
  const char *name)
{
  return env->FindClass(name);
}


JNIEXPORT jclass JNICALL JVM_FindClassFromClassLoader(JNIEnv *env,
  const char *name, jboolean init, jobject loader, jboolean throwError)
{
  LOG_ERROR("ignoring JVM_FindClassFromClassLoader")
  return 0;
}


JNIEXPORT jclass JNICALL JVM_FindClassFromClass(JNIEnv *env, const char *name,
  jboolean init, jclass from)
{
  LOG_ERROR("ignoring JVM_FindClassFromClass")
  return 0;
}


JNIEXPORT jclass JNICALL JVM_FindLoadedClass(JNIEnv *env, jobject loader,
  jstring name)
{
  LOG_ERROR("ignoring JVM_FindLoadedClass")
  return 0;
}


JNIEXPORT jclass JNICALL JVM_DefineClass(JNIEnv *env, const char *name,
  jobject loader, const jbyte *buf, jsize len, jobject pd)
{
  return env->DefineClass(name, loader, buf, len);
}


JNIEXPORT jclass JNICALL JVM_DefineClassWithSource(JNIEnv *env,
  const char *name, jobject loader, const jbyte *buf, jsize len, jobject pd,
  const char *source)
{
  return env->DefineClass(name, loader, buf, len);
}


JNIEXPORT jstring JNICALL JVM_GetClassName(JNIEnv *env, jclass cls)
{
  Object *className;
  Class *clazz = Class::from_class_object(cls);
  UTF16String utfClassName = Class::to_java_class_name(clazz->name);
  error_t errorValue = java_lang_String::new_(utfClassName, &className);
  if (errorValue != RETURN_OK)
  {
    return 0;
  }

  return env->NewLocalRef(className);
}


JNIEXPORT jobjectArray JNICALL JVM_GetClassInterfaces(JNIEnv *env, jclass cls)
{
  LOG_ERROR("ignoring JVM_GetClassInterfaces")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_GetClassLoader(JNIEnv *env, jclass cls)
{
  return Class::from_class_object(cls)->class_loader;
}


JNIEXPORT jboolean JNICALL JVM_IsInterface(JNIEnv *env, jclass cls)
{
  return Class::from_class_object(cls)->is_interface();
}


JNIEXPORT jobjectArray JNICALL JVM_GetClassSigners(JNIEnv *env, jclass cls)
{
  LOG_ERROR("ignoring JVM_GetClassSigners")
  return 0;
}


JNIEXPORT void JNICALL JVM_SetClassSigners(JNIEnv *env, jclass cls,
  jobjectArray signers)
{
  LOG_ERROR("ignoring JVM_SetClassSigners")
}


JNIEXPORT jobject JNICALL JVM_GetProtectionDomain(JNIEnv *env, jclass cls)
{
  LOG_ERROR("ignoring JVM_GetProtectionDomain")
  return 0;
}


JNIEXPORT void JNICALL JVM_SetProtectionDomain(JNIEnv *env, jclass cls,
  jobject protection_domain)
{
  LOG_ERROR("ignoring JVM_SetProtectionDomain")
}


JNIEXPORT jboolean JNICALL JVM_IsArrayClass(JNIEnv *env, jclass cls)
{
  return Class::from_class_object(cls)->is_array();
}


JNIEXPORT jboolean JNICALL JVM_IsPrimitiveClass(JNIEnv *env, jclass cls)
{
  return Class::from_class_object(cls)->is_primitive();
}


JNIEXPORT jclass JNICALL JVM_GetComponentType(JNIEnv *env, jclass cls)
{
  Class* clazz = Class::from_class_object(cls);
  return clazz->component_type->object;
}


JNIEXPORT jint JNICALL JVM_GetClassModifiers(JNIEnv *env, jclass cls)
{
  LOG_ERROR("ignoring JVM_GetClassModifiers")
  return 0;
}


JNIEXPORT jobjectArray JNICALL JVM_GetDeclaredClasses(JNIEnv *env,
  jclass ofClass)
{
  LOG_ERROR("ignoring JVM_GetDeclaredClasses")
  return 0;
}


JNIEXPORT jclass JNICALL JVM_GetDeclaringClass(JNIEnv *env, jclass ofClass)
{
  LOG_ERROR("ignoring JVM_GetDeclaringClass")
  return 0;
}


JNIEXPORT jstring JNICALL JVM_GetClassSignature(JNIEnv *env, jclass cls)
{
  Object *classSignature;
  Class *clazz = Class::from_class_object(cls);
  error_t errorValue = java_lang_String::new_(clazz->name, &classSignature);
  if (errorValue != RETURN_OK)
  {
    return 0;
  }

  return env->NewLocalRef(classSignature);
}


JNIEXPORT jbyteArray JNICALL JVM_GetClassAnnotations(JNIEnv *env, jclass cls)
{
  LOG_ERROR("ignoring JVM_GetClassAnnotations")
  return 0;
}


JNIEXPORT jobjectArray JNICALL JVM_GetClassDeclaredMethods(JNIEnv *env,
  jclass ofClass, jboolean publicOnly)
{
  LOG_ERROR("ignoring JVM_GetClassDeclaredMethods")
  return 0;
}


JNIEXPORT jobjectArray JNICALL JVM_GetClassDeclaredFields(JNIEnv *env,
  jclass ofClass, jboolean publicOnly)
{
  LOG_DEBUG("JVM_GetClassDeclaredFields")

  Class *clazz = Class::from_class_object(ofClass);

  List<Field *> fields;
  clazz->get_declared_fields(fields);

  jclass fieldClassObj = env->FindClass(CLASSNAME_FIELD);
  if (fieldClassObj == 0)
  {
    LOG_ERROR("could not find class java.lang.reflect.Field");
    return 0;
  }

  jmethodID constructor = env->GetMethodID(fieldClassObj,
    METHODNAME_CONSTRUCTOR,
    "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/Class;IILjava/lang/String;[B)V");
  if (constructor == 0)
  {
    LOG_ERROR("could not load constructor of java.lang.reflect.Field")
    return 0;
  }

  jobjectArray array = env->NewObjectArray(fields.size(), fieldClassObj, 0);
  if (array == 0)
  {
    LOG_ERROR("could not create object-array");
    return 0;
  }

  auto begin = fields.begin();
  auto end = fields.end();

  for (jint i = 0; begin != end; ++i)
  {
    Field *field = *begin;

    jclass declaringClass = ofClass;

    jstring name;
    error_t errorValue = java_lang_String::intern(field->signature().name,
      &name);
    RETURN_VALUE_ON_FAIL(errorValue, 0)

    jclass type = field->type()->object;
    jint modifiers = field->access_flags();
    jint slot = field->slot();

    jstring signature;
    errorValue = java_lang_String::intern(field->signature().descriptor,
      &signature);
    RETURN_VALUE_ON_FAIL(errorValue, 0)

    jbyteArray annotations = 0;
    jobject fieldObj = env->NewObject(fieldClassObj, constructor,
      declaringClass, name, type, modifiers, slot, signature, annotations);

    env->SetObjectArrayElement(array, i, fieldObj);

    ++begin;
  }

  return array;
}


JNIEXPORT jobjectArray JNICALL JVM_GetClassDeclaredConstructors(JNIEnv *env,
  jclass ofClass, jboolean publicOnly)
{
  LOG_DEBUG("JVM_GetClassDeclaredConstructors")

  if (ofClass == 0)
  {
    return 0;
  }

  Class *clazz = Class::from_class_object(ofClass);

  List<Method *> methods;
  clazz->get_declared_methods(methods);

  List<Method *> constructors;
  for (Method *method : methods)
  {
    if (method->signature().name == METHODNAME_CONSTRUCTOR) // TODO scheisse
    {
      constructors.addBack(method);
    }
  }

  jclass ctorClassObj = env->FindClass("java/lang/reflect/Constructor");
  jobjectArray array = env->NewObjectArray(constructors.size(), ctorClassObj,
    0);

  jsize i = 0;
  auto begin = constructors.begin();
  auto end = constructors.end();

  while (begin != end)
  {
    Method *method = *begin;

    Object *object;
    error_t errorValue = java_lang_reflect_Constructor::newInstance(method,
      &object);
    RETURN_VALUE_ON_FAIL(errorValue, 0)

    env->SetObjectArrayElement(array, i++, object);

    ++begin;
  }

  return array;
}


JNIEXPORT jint JNICALL JVM_GetClassAccessFlags(JNIEnv *env, jclass cls)
{
  return Class::from_class_object(cls)->class_file->accessFlags;
}


JNIEXPORT jobject JNICALL JVM_InvokeMethod(JNIEnv *env, jobject method,
  jobject obj, jobjectArray args0)
{
  LOG_ERROR("ignoring JVM_InvokeMethod")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_NewInstanceFromConstructor(JNIEnv *env, jobject c,
  jobjectArray args0)
{
  LOG_ERROR("ignoring JVM_NewInstanceFromConstructor")

  Class *ctor_class = c->type();

  Field *declared_class_field;
  error_t error_value = ctor_class->get_declared_field(
    Signature("Ljava/lang/Class;", "clazz"), &declared_class_field);
  RETURN_VALUE_ON_FAIL(error_value, 0)

  Field *slot_field;
  error_value = ctor_class->get_declared_field(Signature("I", "slot"),
    &slot_field);
  RETURN_VALUE_ON_FAIL(error_value, 0)

  Object *declared_class_obj = declared_class_field->get<Object *>(c);
  jint slot = slot_field->get<jint>(c);

  Class *declared_class = Class::from_class_object(declared_class_obj);
  Method *ctor = declared_class->get_declared_method(slot);

  dynarray<Value> params(args0 == 0 ? 0 : args0->length());
  if (args0 != 0)
  {
    for (jint i = 0; i < args0->length(); ++i)
    {
      Value param;
      error_value = args0->get_value(i, &param);
      RETURN_VALUE_ON_FAIL(error_value, 0)
      params[i] = param;
    }
  }

  Object *object = 0;
  Object::new_object(ctor, params, &object);

  return object;
}


JNIEXPORT jobject JNICALL JVM_GetClassConstantPool(JNIEnv *env, jclass cls)
{
  return cls;
}


JNIEXPORT jint JNICALL JVM_ConstantPoolGetSize(JNIEnv *env, jobject unused,
  jobject jcpool)
{
  return Class::from_class_object(jcpool)->class_file->constantPool.length();
}


JNIEXPORT jclass JNICALL JVM_ConstantPoolGetClassAt(JNIEnv *env, jobject unused,
  jobject jcpool, jint index)
{
  Class *clazz;
  Class *cpClass = Class::from_class_object(jcpool);
  error_t errorValue = cpClass->get_class_from_cp(index, &clazz);
  if (errorValue != RETURN_OK)
  {
    return 0;
  }

  return clazz->object;
}


JNIEXPORT jclass JNICALL JVM_ConstantPoolGetClassAtIfLoaded(JNIEnv *env,
  jobject unused, jobject jcpool, jint index)
{
  LOG_ERROR("ignoring JVM_ConstantPoolGetClassAtIfLoaded")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_ConstantPoolGetMethodAt(JNIEnv *env,
  jobject unused, jobject jcpool, jint index)
{
  LOG_ERROR("ignoring JVM_ConstantPoolGetMethodAt")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_ConstantPoolGetMethodAtIfLoaded(JNIEnv *env,
  jobject unused, jobject jcpool, jint index)
{
  LOG_ERROR("ignoring JVM_ConstantPoolGetMethodAtIfLoaded")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_ConstantPoolGetFieldAt(JNIEnv *env,
  jobject unused, jobject jcpool, jint index)
{
  LOG_ERROR("ignoring JVM_ConstantPoolGetFieldAt")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_ConstantPoolGetFieldAtIfLoaded(JNIEnv *env,
  jobject unused, jobject jcpool, jint index)
{
  LOG_ERROR("ignoring JVM_ConstantPoolGetFieldAtIfLoaded")
  return 0;
}


JNIEXPORT jobjectArray JNICALL JVM_ConstantPoolGetMemberRefInfoAt(JNIEnv *env,
  jobject unused, jobject jcpool, jint index)
{
  LOG_ERROR("ignoring JVM_ConstantPoolGetMemberRefInfoAt")
  return 0;
}


JNIEXPORT jint JNICALL JVM_ConstantPoolGetIntAt(JNIEnv *env, jobject unused,
  jobject jcpool, jint index)
{
  return Class::from_class_object(jcpool)->get_integer_from_cp(index);
}


JNIEXPORT jlong JNICALL JVM_ConstantPoolGetLongAt(JNIEnv *env, jobject unused,
  jobject jcpool, jint index)
{
  return Class::from_class_object(jcpool)->get_long_from_cp(index);
}


JNIEXPORT jfloat JNICALL JVM_ConstantPoolGetFloatAt(JNIEnv *env, jobject unused,
  jobject jcpool, jint index)
{
  return Class::from_class_object(jcpool)->get_float_from_cp(index);
}


JNIEXPORT jdouble JNICALL JVM_ConstantPoolGetDoubleAt(JNIEnv *env,
  jobject unused, jobject jcpool, jint index)
{
  return Class::from_class_object(jcpool)->get_double_from_cp(index);
}


JNIEXPORT jstring JNICALL JVM_ConstantPoolGetStringAt(JNIEnv *env,
  jobject unused, jobject jcpool, jint index)
{

  Object *string;
  Class *cpClass = Class::from_class_object(jcpool);
  error_t errorValue = cpClass->get_string_from_cp(index, &string);
  if (errorValue != RETURN_OK)
  {
    return 0;
  }

  return env->NewLocalRef(string);
}


JNIEXPORT jstring JNICALL JVM_ConstantPoolGetUTF8At(JNIEnv *env, jobject unused,
  jobject jcpool, jint index)
{
  Object *string;
  Class *cpClass = Class::from_class_object(jcpool);
  UTF16String utfString = cpClass->get_utf8_from_cp(index);
  error_t errorValue = java_lang_String::new_(utfString, &string);
  if (errorValue != RETURN_OK)
  {
    return 0;
  }

  return env->NewLocalRef(string);
}


JNIEXPORT jobject JNICALL JVM_DoPrivileged(JNIEnv *env, jclass cls,
  jobject action, jobject context, jboolean wrapException)
{
  Class *clazz = action->type();

  Method *method;
  error_t errorValue = clazz->get_method(
    Signature("()Ljava/lang/Object;", "run"), &method);
  RETURN_VALUE_ON_FAIL(errorValue, 0)

  Value value;
  errorValue = method->invoke(action, 0, &value);
  RETURN_VALUE_ON_FAIL(errorValue, 0)

  return env->NewLocalRef(value.as_object());
}


JNIEXPORT jobject JNICALL JVM_GetInheritedAccessControlContext(JNIEnv *env,
  jclass cls)
{

  LOG_ERROR("ignoring JVM_GetInheritedAccessControlContext")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_GetStackAccessControlContext(JNIEnv *env,
  jclass cls)
{

  LOG_ERROR("ignoring JVM_GetStackAccessControlContext")
  return 0;
}


JNIEXPORT void *JNICALL JVM_RegisterSignal(jint sig, void *handler)
{

  LOG_ERROR("ignoring JVM_RegisterSignal")
  return 0;
}


JNIEXPORT jboolean JNICALL JVM_RaiseSignal(jint sig)
{

  LOG_ERROR("ignoring JVM_RaiseSignal")
  return 0;
}


JNIEXPORT jint JNICALL JVM_FindSignal(const char *name)
{

  LOG_ERROR("ignoring JVM_FindSignal")
  return 0;
}


JNIEXPORT jboolean JNICALL JVM_DesiredAssertionStatus(JNIEnv *env,
  jclass unused, jclass cls)
{

  LOG_ERROR("ignoring JVM_DesiredAssertionStatus")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_AssertionStatusDirectives(JNIEnv *env,
  jclass unused)
{

  LOG_ERROR("ignoring JVM_AssertionStatusDirectives")
  return 0;
}


JNIEXPORT jboolean JNICALL JVM_SupportsCX8()
{

  LOG_ERROR("ignoring JVM_SupportsCX8")
  return 0;
}


JNIEXPORT jint JNICALL JVM_DTraceGetVersion(JNIEnv *env)
{

  LOG_ERROR("ignoring JVM_DTraceGetVersion")
  return 0;
}


JNIEXPORT jlong JNICALL JVM_DTraceActivate(JNIEnv *env, jint version,
  jstring module_name, jint providers_count, JVM_DTraceProvider *providers)
{

  LOG_ERROR("ignoring JVM_DTraceActivate")
  return 0;
}


JNIEXPORT jboolean JNICALL JVM_DTraceIsProbeEnabled(JNIEnv *env,
  jmethodID method)
{

  LOG_ERROR("ignoring JVM_DTraceIsProbeEnabled")
  return 0;
}


JNIEXPORT void JNICALL JVM_DTraceDispose(JNIEnv *env, jlong activation_handle)
{

  LOG_ERROR("ignoring JVM_DTraceDispose")
}


JNIEXPORT jboolean JNICALL JVM_DTraceIsSupported(JNIEnv *env)
{

  LOG_ERROR("ignoring JVM_DTraceIsSupported")
  return 0;
}


JNIEXPORT const char *JNICALL JVM_GetClassNameUTF(JNIEnv *env, jclass cb)
{

  Class *clazz = Class::from_class_object(cb);

  return clazz == 0 ? 0 : copyFromString(clazz->name);
}


JNIEXPORT void JNICALL JVM_GetClassCPTypes(JNIEnv *env, jclass cb,
  unsigned char *types)
{

  LOG_ERROR("ignoring JVM_GetClassCPTypes")
}


JNIEXPORT jint JNICALL JVM_GetClassCPEntriesCount(JNIEnv *env, jclass cb)
{

  LOG_ERROR("ignoring JVM_GetClassCPEntriesCount")
  return 0;
}


JNIEXPORT jint JNICALL JVM_GetClassFieldsCount(JNIEnv *env, jclass cb)
{

  LOG_ERROR("ignoring JVM_GetClassFieldsCount")
  return 0;
}


JNIEXPORT jint JNICALL JVM_GetClassMethodsCount(JNIEnv *env, jclass cb)
{

  LOG_ERROR("ignoring JVM_GetClassMethodsCount")
  return 0;
}


JNIEXPORT void JNICALL JVM_GetMethodIxExceptionIndexes(JNIEnv *env, jclass cb,
  jint method_index, unsigned short *exceptions)
{

  LOG_ERROR("ignoring JVM_GetMethodIxExceptionIndexes")
}


JNIEXPORT jint JNICALL JVM_GetMethodIxExceptionsCount(JNIEnv *env, jclass cb,
  jint method_index)
{

  LOG_ERROR("ignoring JVM_GetMethodIxExceptionsCount")
  return 0;
}


JNIEXPORT void JNICALL JVM_GetMethodIxByteCode(JNIEnv *env, jclass cb,
  jint method_index, unsigned char *code)
{

  LOG_ERROR("ignoring JVM_GetMethodIxByteCode")
}


JNIEXPORT jint JNICALL JVM_GetMethodIxByteCodeLength(JNIEnv *env, jclass cb,
  jint method_index)
{

  LOG_ERROR("ignoring JVM_GetMethodIxByteCodeLength")
  return 0;
}


JNIEXPORT void JNICALL JVM_GetMethodIxExceptionTableEntry(JNIEnv *env,
  jclass cb, jint method_index, jint entry_index,
  JVM_ExceptionTableEntryType *entry)
{

  LOG_ERROR("ignoring JVM_GetMethodIxExceptionTableEntry")
}


JNIEXPORT jint JNICALL JVM_GetMethodIxExceptionTableLength(JNIEnv *env,
  jclass cb, int index)
{

  LOG_ERROR("ignoring JVM_GetMethodIxExceptionTableLength")
  return 0;
}


JNIEXPORT jint JNICALL JVM_GetFieldIxModifiers(JNIEnv *env, jclass cb,
  int index)
{

  LOG_ERROR("ignoring JVM_GetFieldIxModifiers")
  return 0;
}


JNIEXPORT jint JNICALL JVM_GetMethodIxModifiers(JNIEnv *env, jclass cb,
  int index)
{

  LOG_ERROR("ignoring JVM_GetMethodIxModifiers")
  return 0;
}


JNIEXPORT jint JNICALL JVM_GetMethodIxLocalsCount(JNIEnv *env, jclass cb,
  int index)
{

  Method *method;
  Class *cpClass = Class::from_class_object(cb);
  error_t errorValue = cpClass->get_method_from_cp(index, &method);
  if (errorValue != RETURN_OK)
  {
    return 0;
  }

  return method->locals_count();
}


JNIEXPORT jint JNICALL JVM_GetMethodIxArgsSize(JNIEnv *env, jclass cb,
  int index)
{

  Method *method;
  Class *cpClass = Class::from_class_object(cb);
  error_t errorValue = cpClass->get_method_from_cp(index, &method);
  if (errorValue != RETURN_OK)
  {
    return 0;
  }

  return method->parameter_types().size();
}


JNIEXPORT jint JNICALL JVM_GetMethodIxMaxStack(JNIEnv *env, jclass cb,
  int index)
{

  Method *method;
  Class *cpClass = Class::from_class_object(cb);
  error_t errorValue = cpClass->get_method_from_cp(index, &method);
  if (errorValue != RETURN_OK)
  {
    return 0;
  }

  return method->operands_count();
}


JNIEXPORT jboolean JNICALL JVM_IsConstructorIx(JNIEnv *env, jclass cb,
  int index)
{

  Method *method;
  Class *cpClass = Class::from_class_object(cb);
  error_t errorValue = cpClass->get_method_from_cp(index, &method);
  if (errorValue != RETURN_OK)
  {
    return 0;
  }

  return method->signature().name == METHODNAME_CONSTRUCTOR;
}


JNIEXPORT const char *JNICALL JVM_GetMethodIxNameUTF(JNIEnv *env, jclass cb,
  jint index)
{

  Method *method;
  Class *cpClass = Class::from_class_object(cb);
  error_t errorValue = cpClass->get_method_from_cp(index, &method);
  if (errorValue != RETURN_OK)
  {
    return 0;
  }

  return copyFromString(method->signature().name);
}


JNIEXPORT const char *JNICALL JVM_GetMethodIxSignatureUTF(JNIEnv *env,
  jclass cb, jint index)
{

  Method *method;
  Class *cpClass = Class::from_class_object(cb);
  error_t errorValue = cpClass->get_method_from_cp(index, &method);
  if (errorValue != RETURN_OK)
  {
    return 0;
  }

  return copyFromString(method->signature().descriptor);
}


JNIEXPORT const char *JNICALL JVM_GetCPFieldNameUTF(JNIEnv *env, jclass cb,
  jint index)
{

  LOG_ERROR("ignoring JVM_GetCPFieldNameUTF")
  return 0;
}


JNIEXPORT const char *JNICALL JVM_GetCPMethodNameUTF(JNIEnv *env, jclass cb,
  jint index)
{

  LOG_ERROR("ignoring JVM_GetCPMethodNameUTF")
  return 0;
}


JNIEXPORT const char *JNICALL JVM_GetCPMethodSignatureUTF(JNIEnv *env,
  jclass cb, jint index)
{

  LOG_ERROR("ignoring JVM_GetCPMethodSignatureUTF")
  return 0;
}


JNIEXPORT const char *JNICALL JVM_GetCPFieldSignatureUTF(JNIEnv *env, jclass cb,
  jint index)
{

  LOG_ERROR("ignoring JVM_GetCPFieldSignatureUTF")
  return 0;
}


JNIEXPORT const char *JNICALL JVM_GetCPClassNameUTF(JNIEnv *env, jclass cb,
  jint index)
{

  LOG_ERROR("ignoring JVM_GetCPClassNameUTF")
  return 0;
}


JNIEXPORT const char *JNICALL JVM_GetCPFieldClassNameUTF(JNIEnv *env, jclass cb,
  jint index)
{

  LOG_ERROR("ignoring JVM_GetCPFieldClassNameUTF")
  return 0;
}


JNIEXPORT const char *JNICALL JVM_GetCPMethodClassNameUTF(JNIEnv *env,
  jclass cb, jint index)
{

  LOG_ERROR("ignoring JVM_GetCPMethodClassNameUTF")
  return 0;
}


JNIEXPORT jint JNICALL JVM_GetCPFieldModifiers(JNIEnv *env, jclass cb,
  int index, jclass calledClass)
{

  LOG_ERROR("ignoring JVM_GetCPFieldModifiers")
  return 0;
}


JNIEXPORT jint JNICALL JVM_GetCPMethodModifiers(JNIEnv *env, jclass cb,
  int index, jclass calledClass)
{

  LOG_ERROR("ignoring JVM_GetCPMethodModifiers")
  return 0;
}


JNIEXPORT void JNICALL JVM_ReleaseUTF(const char *utf)
{

  DELETE_ARRAY(utf);
}


JNIEXPORT jboolean JNICALL JVM_IsSameClassPackage(JNIEnv *env, jclass class1,
  jclass class2)
{

  LOG_ERROR("ignoring JVM_IsSameClassPackage")
  return 0;
}


JNIEXPORT jint JNICALL JVM_GetLastErrorString(char *buf, int len)
{

  LOG_ERROR("ignoring JVM_GetLastErrorString")
  return 0;
}


JNIEXPORT char *JNICALL JVM_NativePath(char *nativePath)
{
  LOG_WARN("JVM_NativePath may not correctly implemented")
  return nativePath;
}


JNIEXPORT jint JNICALL JVM_Open(const char *fname, jint flags, jint mode)
{

  LOG_ERROR("ignoring JVM_Open")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Close(jint fd)
{

  LOG_ERROR("ignoring JVM_Close")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Read(jint fd, char *buf, jint nbytes)
{
  LOG_ERROR("ignoring JVM_Read")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Write(jint fd, char *buf, jint nbytes)
{
  return write(fd, buf, nbytes);
}


JNIEXPORT jint JNICALL JVM_Available(jint fd, jlong *pbytes)
{

  LOG_ERROR("ignoring JVM_Available")
  return 0;
}


JNIEXPORT jlong JNICALL JVM_Lseek(jint fd, jlong offset, jint whence)
{

  LOG_ERROR("ignoring JVM_Lseek")
  return 0;
}


JNIEXPORT jint JNICALL JVM_SetLength(jint fd, jlong length)
{

  LOG_ERROR("ignoring JVM_SetLength")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Sync(jint fd)
{

  LOG_ERROR("ignoring JVM_Sync")
  return 0;
}


JNIEXPORT jint JNICALL JVM_InitializeSocketLibrary()
{

  LOG_ERROR("ignoring JVM_InitializeSocketLibrary")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Socket(jint domain, jint type, jint protocol)
{

  LOG_ERROR("ignoring JVM_Socket")
  return 0;
}


JNIEXPORT jint JNICALL JVM_SocketClose(jint fd)
{

  LOG_ERROR("ignoring JVM_SocketClose")
  return 0;
}


JNIEXPORT jint JNICALL JVM_SocketShutdown(jint fd, jint howto)
{

  LOG_ERROR("ignoring JVM_SocketShutdown")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Recv(jint fd, char *buf, jint nBytes, jint flags)
{

  LOG_ERROR("ignoring JVM_Recv")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Send(jint fd, char *buf, jint nBytes, jint flags)
{

  LOG_ERROR("ignoring JVM_Send")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Timeout(int fd, long timeout)
{

  LOG_ERROR("ignoring JVM_Timeout")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Listen(jint fd, jint count)
{

  LOG_ERROR("ignoring JVM_Listen")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Connect(jint fd, struct sockaddr *him, jint len)
{

  LOG_ERROR("ignoring JVM_Connect")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Bind(jint fd, struct sockaddr *him, jint len)
{

  LOG_ERROR("ignoring JVM_Bind")
  return 0;
}


JNIEXPORT jint JNICALL JVM_Accept(jint fd, struct sockaddr *him, jint *len)
{

  LOG_ERROR("ignoring JVM_Accept")
  return 0;
}


JNIEXPORT jint JNICALL JVM_RecvFrom(jint fd, char *buf, int nBytes, int flags,
  struct sockaddr *from, int *fromlen)
{

  LOG_ERROR("ignoring JVM_RecvFrom")
  return 0;
}


JNIEXPORT jint JNICALL JVM_SendTo(jint fd, char *buf, int len, int flags,
  struct sockaddr *to, int tolen)
{

  LOG_ERROR("ignoring JVM_SendTo")
  return 0;
}


JNIEXPORT jint JNICALL JVM_SocketAvailable(jint fd, jint *result)
{

  LOG_ERROR("ignoring JVM_SocketAvailable")
  return 0;
}


JNIEXPORT jint JNICALL JVM_GetSockName(jint fd, struct sockaddr *him, int *len)
{

  LOG_ERROR("ignoring JVM_GetSockName")
  return 0;
}


JNIEXPORT jint JNICALL JVM_GetSockOpt(jint fd, int level, int optname,
  char *optval, int *optlen)
{

  LOG_ERROR("ignoring JVM_GetSockOpt")
  return 0;
}


JNIEXPORT jint JNICALL JVM_SetSockOpt(jint fd, int level, int optname,
  const char *optval, int optlen)
{

  LOG_ERROR("ignoring JVM_SetSockOpt")
  return 0;
}


JNIEXPORT int JNICALL JVM_GetHostName(char *name, int namelen)
{

  LOG_ERROR("ignoring JVM_GetHostName")
  return 0;
}


int jio_vsnprintf(char *str, size_t count, const char *fmt, va_list args)
{

  LOG_ERROR("ignoring jio_vsnprintf")
  return 0;
}


int jio_snprintf(char *str, size_t count, const char *fmt, ...)
{

  LOG_ERROR("ignoring jio_snprintf")
  return 0;
}


int jio_fprintf(FILE *file, const char *fmt, ...)
{

  LOG_ERROR("ignoring jio_fprintf")
  return 0;
}


int jio_vfprintf(FILE *file, const char *fmt, va_list args)
{

  LOG_ERROR("ignoring jio_vfprintf")
  return 0;
}


JNIEXPORT void *JNICALL JVM_RawMonitorCreate()
{
  return (void *) new Monitor();
}


JNIEXPORT void JNICALL JVM_RawMonitorDestroy(void *mon)
{
  Monitor *monitor = (Monitor *) mon;
  DELETE_OBJECT(monitor)
}


JNIEXPORT jint JNICALL JVM_RawMonitorEnter(void *mon)
{
  ((Monitor *) mon)->enter();
  return 0;
}


JNIEXPORT void JNICALL JVM_RawMonitorExit(void *mon)
{
  ((Monitor *) mon)->exit();
}


JNIEXPORT void *JNICALL JVM_GetManagement(jint version)
{
  LOG_ERROR("ignoring JVM_GetManagement")
  return 0;
}


JNIEXPORT jobject JNICALL JVM_InitAgentProperties(JNIEnv *env,
  jobject agent_props)
{
  LOG_ERROR("ignoring JVM_InitAgentProperties")
  return 0;
}


JNIEXPORT jobjectArray JNICALL JVM_GetEnclosingMethodInfo(JNIEnv *env,
  jclass ofClass)
{
  LOG_ERROR("ignoring JVM_GetEnclosingMethodInfo")
  return 0;
}


JNIEXPORT jintArray JNICALL JVM_GetThreadStateValues(JNIEnv *env,
  jint javaThreadState)
{
  LOG_ERROR("ignoring JVM_GetThreadStateValues")
  return 0;
}


JNIEXPORT jobjectArray JNICALL JVM_GetThreadStateNames(JNIEnv *env,
  jint javaThreadState, jintArray values)
{
  LOG_ERROR("ignoring JVM_GetThreadStateNames")
  return 0;
}


JNIEXPORT void JNICALL JVM_GetVersionInfo(JNIEnv *env, jvm_version_info *info,
  size_t infoSize)
{
  LOG_ERROR("ignoring JVM_GetVersionInfo")
}

#endif

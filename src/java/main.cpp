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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <jvm/Global.hpp>

#define HANDLE_EXCEPTION(value) \
  if (value == 0) { \
    if (jniEnv->ExceptionCheck()) { \
      jniEnv->ExceptionDescribe(); \
    } \
    return 1; \
  }

using namespace coldspot;

int argIndex = 1;

void init_args(int argc, char **argv, JavaVMInitArgs *init_args)
{
    // Initialize init-arguments
    init_args->nOptions = 0;
    init_args->options = 0;

    bool multiTokenOption = false;

    // Iterate over arguments
    while (argIndex < argc)
    {
        // Current argument
        char *arg = argv[argIndex];

        // Break if it is not the second token of an option
        // and does not start with '-' (class-name reached)
        if (!multiTokenOption && arg[0] != '-')
        {
            break;
        }

        // Resize memory
        size_t newSize = ++init_args->nOptions * sizeof(JavaVMOption);
        void *newMemory = realloc((void *) init_args->options, newSize);
        init_args->options = (JavaVMOption *) newMemory;

        // Set option
        init_args->options[init_args->nOptions - 1].extraInfo = 0;
        init_args->options[init_args->nOptions - 1].optionString = arg;

        if (multiTokenOption)
        {
            multiTokenOption = false;
        }
        else if (strcmp(arg, "-cp") == 0 || strcmp(arg, "-classpath") == 0)
        {
            multiTokenOption = true;
        }

        // Next argument
        ++argIndex;
    }
}


const char *next_argument(int argc, char **argv)
{
    if (argIndex < argc)
    {
        return argv[argIndex++];
    }

    return 0;
}


void print_usage()
{
    LOG_ERROR(
        "usage: coldspot [-options] class [args...]\n")  // TODO load from call
    LOG_ERROR("options:\n")

    LOG_ERROR("\t-cp <pathes>\n")
    LOG_ERROR("\t-classpath <pathes>\n")
    LOG_ERROR(
        "\t\tColon separated pathes to directories to search for classes\n")

    LOG_ERROR("\t-D<name>=<value>\n")
    LOG_ERROR("\t\tSets a system property\n")

    LOG_ERROR("\t-verbose:[class|gc|jni]\n")
    LOG_ERROR("\t\tActivates verbose messages\n")

    LOG_ERROR("\t-Xverbose:[debug|execute]\n")
    LOG_ERROR("\t\tActivates non-standard verbose messages\n")

    fflush(stderr);
}


int main(int argc, char **argv)
{
    JavaVMInitArgs args;
    init_args(argc, argv, &args);

    // Get start class
    const char *startClassName = next_argument(argc, argv);
    if (startClassName == 0)
    {
        print_usage();
        return 1;
    }

    // Create and initialize the vm
    JavaVM *javaVM;
    JNIEnv *jniEnv;
    if (JNI_CreateJavaVM(&javaVM, (void **) &jniEnv, &args) == JNI_OK)
    {
        // Load start class
        jclass startClass = jniEnv->FindClass(startClassName);
        HANDLE_EXCEPTION(startClass)

        // Get handle to main-method of the start clazz
        jmethodID mainMethod = jniEnv->GetMethodID(startClass, "main",
            "([Ljava/lang/String;)V");
        HANDLE_EXCEPTION(mainMethod)

        // Create arguments
        jint argumentsCount = argc - argIndex;
        jclass stringClass = jniEnv->FindClass("java/lang/String");
        jobjectArray arguments = jniEnv->NewObjectArray(argumentsCount,
            stringClass, 0);
        HANDLE_EXCEPTION(arguments)

        // Fill arguments
        for (int i = 0; i < argumentsCount; ++i)
        {
            jstring argument = jniEnv->NewStringUTF(next_argument(argc, argv));
            jniEnv->SetObjectArrayElement(arguments, i, argument);
        }

        // Call main-method
        jniEnv->CallStaticVoidMethod(startClass, mainMethod, arguments);

        // Handle pending exception
        if (jniEnv->ExceptionCheck())
        {
            jniEnv->ExceptionDescribe();
        }

        // Detach thread
        javaVM->DetachCurrentThread();
    }

    return javaVM->DestroyJavaVM();
}

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

#if defined(OS_POSIX)

    #include <jvm/Global.hpp>

    #include <cstdlib>

    #include <chrono>

    #include <sys/time.h>
    #include <sys/types.h>
    #include <dlfcn.h>
    #include <pthread.h>
    #include <pwd.h>
    #include <unistd.h>

namespace coldspot
{

  using namespace std::chrono;


  String System::architecture()
  {
    return "x86_64";
  }


  Thread_t System::currentThread()
  {
    return pthread_self();
  }


  String System::environmentVariable(const String &variable)
  {
    return getenv(variable.c_str());
  }


  Function_t System::getFunction(Library_t library, const String &name)
  {
    return dlsym(library, name.c_str());
  }


  Library_t System::loadLibrary(const String &path)
  {
    Library_t lib = dlopen(path.c_str(), RTLD_LAZY);
    if (lib == 0)
    {
      LOG_WARN("loadLibrary: " << dlerror())
    }

    return lib;
  }


  jlong System::millis()
  {
    return duration_cast<milliseconds>(
      system_clock::now().time_since_epoch()).count();
  }


  String System::name()
  {
    return "Mac OS X";
  }


  void System::releaseLibrary(Library_t library)
  {
    dlclose(library);
  }


  void System::sleep(jlong ms)
  {
    usleep(ms * 1000);
  }


  String System::userhome()
  {
    struct passwd *pw = getpwuid(getuid());
    return pw->pw_dir;
  }


  String System::username()
  {
    char username[1024];
    memset(username, 0, 1024);
    getlogin_r(username, 1024);
    return username;
  }


  String System::version()
  {
    return "unknown";  // TODO
  }


  String System::workingDirectory()
  {
    char workingDirectory[4096];
    memset(workingDirectory, 0, 4096);
    return getcwd(workingDirectory, sizeof(workingDirectory));
  }


  void System::createThread(void *function, void *parameter)
  {
    using start_t = void *(*)(void *);

    pthread_t thread;
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    pthread_attr_setstacksize(&attributes, NATIVE_STACK_SIZE);
    pthread_create(&thread, &attributes, (start_t) function, parameter);
  }


  void System::stopThread(Thread_t thread)
  {
    pthread_cancel(thread);
  }


  void System::yield()
  {
    sched_yield();
  }


  void System::join(Thread_t thread)
  {
    pthread_join(thread, 0);
  }

}

#endif

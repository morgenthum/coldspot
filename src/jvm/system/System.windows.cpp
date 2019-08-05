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

#if defined(OS_WINDOWS) && !defined(OS_POSIX)

    #include <jvm/Global.hpp>

    #include <chrono>

    #include <shlobj.h>
    #include <unistd.h>

namespace coldspot {

  using namespace std::chrono;

  
  String System::architecture() {

    SYSTEM_INFO systemInfo;
    GetNativeSystemInfo(&systemInfo);

    switch (systemInfo.wProcessorArchitecture) {
      case 9: {
        return "x86_64";
      }
      case 6: {
        return "x86";
      }
    }

    return "unknown";
  }


  Thread_t System::currentThread() {

    return pthread_self();
  }


  String System::environmentVariable(const String &variable) {

    return getenv(variable.toCString());
  }


  Function_t System::getFunction(Library_t library,
                                 const String &name) {

    return GetProcAddress((HMODULE) library, String(name).toCString());
  }


  Library_t System::loadLibrary(const String &path) {

    return LoadLibraryA(path.toCString());
  }


  jlong System::millis() {

    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count();
  }


  String System::name() {

    return "unknown";
  }


  void System::releaseLibrary(Library_t library) {

    FreeLibrary(library);
  }


  void System::sleep(jlong ms) {

    Sleep(ms);
  }


  String System::userhome() {

    char path[4096];
    SHGetFolderPath(0, CSIDL_PROFILE, 0, 0, path);

    return path;
  }


  String System::username() {

    char username[1024];
    DWORD length = 1024;
    GetUserName(username, &length);

    return username;
  }


  String System::version() {

    return "unknown";
  }


  String System::workingDirectory() {

    char workingDirectory[4096];
    memset(workingDirectory, 0, 4096);
    char *answer = getcwd(workingDirectory, sizeof(workingDirectory));
    return String(answer);
  }


  void System::createThread(void *function, void *parameter) {

    CreateThread(0, 0, (LPTHREAD_START_ROUTINE) function, parameter, 0, 0);
  }


  void System::yield() {

    SwitchToThread();
  }


  void System::join(Thread_t thread) {

    pthread_join(thread, 0);
  }

}

#endif

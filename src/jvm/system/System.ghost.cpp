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

#if defined(OS_GHOST)

    #include <jvm/Global.hpp>

    #include <Ghost.hpp>
    #include <cstdlib>
    #include <unistd.h>

namespace coldspot {


String System::architecture() {

  // TODO
  return "unknown";
}


Thread_t System::currentThread() {

  return ggettid();
}


String System::environmentVariable(const String& variable) {

  // TODO
  return "";
}


void System::stopThread(Thread_t thread) {

  // TODO
}


Function_t System::getFunction(Library_t library,
                               const String& name) {

  // TODO
  return 0;
}


Library_t System::loadLibrary(const String& path) {

  // TODO
  return 0;
}


jlong System::millis() {

  return gmillis();
}


String System::name() {

  return "unknown";
}


void System::releaseLibrary(Library_t library) {

  // TODO
}


void System::sleep(jlong ms) {

  gsleep(ms);
}


String System::userhome() {

  // TODO
  return "/";
}


String System::username() {

  // TODO
  return "maxdev";
}


String System::version() {

  return "unknown";
}


String System::workingDirectory() {

  // TODO
  return "/";
}


void System::createThread(void* function, void* parameter) {

  gcreatethread(function, parameter);
}


void System::yield() {

  gyield();
}


void System::join(Thread_t thread) {

  gjoin(thread);
}

}

#endif

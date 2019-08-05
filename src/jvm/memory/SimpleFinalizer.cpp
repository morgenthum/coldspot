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

#include <jvm/Global.hpp>

namespace coldspot
{

    void SimpleFinalizer::finalize()
    {
        // Move incoming objects to processing objects
        _in_objects.lock();
        List<Object *>::move(*_in_objects, _current_objects);
        _in_objects.unlock();

#ifdef IS_LOG_LEVEL_DEBUG
        auto start_millis = System::millis();
#endif

        // Finalize objects
        for (auto object : _current_objects)
        {
            java_lang_Object::finalize(object);
        }

#ifdef IS_LOG_LEVEL_DEBUG
        auto needed_millis = System::millis() - start_millis;
        LOG_DEBUG_VERBOSE(GC,
          "finalized " << _current_objects.size() << " objects in " <<
          needed_millis << " ms")
#endif

        // Move finalized objects to outgoing objects
        _out_objects.lock();
        List<Object *>::move(_current_objects, *_out_objects);
        _out_objects.unlock();
    }

}

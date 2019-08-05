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

#ifndef COLDSPOT_JVM_MEMORY_SIMPLEGARBAGECOLLECTOR_HPP_
#define COLDSPOT_JVM_MEMORY_SIMPLEGARBAGECOLLECTOR_HPP_

#include "GarbageCollector.hpp"

namespace coldspot
{

    class SimpleGarbageCollector : public GarbageCollector
    {
    public:

        void collectGarbage() override;

        void collectGarbageForExit() override;

    private:

        // Deletes all threads that are terminated.
        void deleteTerminatedVMThreads();

        // Deletes all vm-threads
        void deleteVMThreads();

        void finalizeAllObjects();

        // Moves all unused objects from source to target.
        void moveUnusedObjects(heap<Object *> &source,
            Lockable <List<Object *>> &target);

        // Removes all finalized objects.
        void removeFinalizedObjects();
    };

}

#endif

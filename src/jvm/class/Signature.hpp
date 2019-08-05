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

#ifndef COLDSPOT_JVM_CLASS_SIGNATURE_HPP_
#define COLDSPOT_JVM_CLASS_SIGNATURE_HPP_

#include <cstdint>

#include <jvm/common/Hashable.hpp>
#include <jvm/common/String.hpp>

namespace coldspot
{

    class Signature : public Hashable
    {
    public:

        String descriptor;
        String name;

        Signature()
        {
        }

        Signature(const String &descriptor, const String &name) : descriptor(
            descriptor), name(name)
        {
        }

        uint64_t hashCode() const override
        {
            return descriptor.hashCode() + name.hashCode();
        }

        friend inline bool operator==(const Signature &lhs,
            const Signature &rhs)
        {
            return lhs.descriptor == rhs.descriptor && lhs.name == rhs.name;
        }
    };

}

#endif

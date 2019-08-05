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

    /**
     *
     */
    Annotation::~Annotation()
    {

        for (uint16_t i = 0; i < elementValuePairs.length(); ++i)
        {
            DELETE_OBJECT(elementValuePairs[i])
        }
    }

    /**
     *
     */
    AnnotationValue::~AnnotationValue()
    {

        DELETE_OBJECT(value);
    }

    /**
     *
     */
    ArrayValue::~ArrayValue()
    {

        for (uint16_t i = 0; i < values.length(); ++i)
        {
            DELETE_OBJECT(values[i])
        }
    }

    /**
     *
     */
    ElementValuePair::~ElementValuePair()
    {

        DELETE_OBJECT(value);
    }

    /**
     *
     */
    ParameterAnnotation::~ParameterAnnotation()
    {

        for (uint16_t i = 0; i < annotations.length(); ++i)
        {
            DELETE_OBJECT(annotations[i])
        }
    }

    /**
     *
     */
    SameLocals1StackItemFrame::~SameLocals1StackItemFrame()
    {

        DELETE_OBJECT(stackElement);
    }

    /**
     *
     */
    SameLocals1StackItemFrameExtended::~SameLocals1StackItemFrameExtended()
    {

        DELETE_OBJECT(stackElement);
    }

    /**
     *
     */
    AppendFrame::~AppendFrame()
    {

        for (uint16_t i = 0; i < locals.length(); ++i)
        {
            DELETE_OBJECT(locals[i])
        }
    }

    /**
     *
     */
    FullFrame::~FullFrame()
    {

        for (uint16_t i = 0; i < locals.length(); ++i)
        {
            DELETE_OBJECT(locals[i])
        }

        for (uint16_t i = 0; i < stack.length(); ++i)
        {
            DELETE_OBJECT(stack[i])
        }
    }

}

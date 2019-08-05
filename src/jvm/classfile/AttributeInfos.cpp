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
    AnnotationDefaultAttribute::~AnnotationDefaultAttribute()
    {

        DELETE_OBJECT(defaultValue);
    }

    /**
     *
     */
    BootstrapMethodsAttribute::~BootstrapMethodsAttribute()
    {

        for (uint16_t i = 0; i < bootstrapMethods.length(); ++i)
        {
            DELETE_OBJECT(bootstrapMethods[i])
        }
    }

    /**
     *
     */
    CodeAttribute::~CodeAttribute()
    {
        DELETE_ARRAY(code);

        for (uint16_t i = 0; i < exceptionTable.length(); ++i)
        {
            DELETE_OBJECT(exceptionTable[i])
        }

        for (uint16_t i = 0; i < attributes.length(); ++i)
        {
            DELETE_OBJECT(attributes[i])
        }
    }

    /**
     *
     */
    InnerClassesAttribute::~InnerClassesAttribute()
    {

        for (uint16_t i = 0; i < classes.length(); ++i)
        {
            DELETE_OBJECT(classes[i])
        }
    }

    /**
     *
     */
    LineNumberTableAttribute::~LineNumberTableAttribute()
    {

        for (uint16_t i = 0; i < lineNumberTable.length(); ++i)
        {
            DELETE_OBJECT(lineNumberTable[i])
        }
    }

    /**
     *
     */
    LocalVariableTableAttribute::~LocalVariableTableAttribute()
    {

        for (uint16_t i = 0; i < table.length(); ++i)
        {
            DELETE_OBJECT(table[i])
        }
    }

    /**
     *
     */
    LocalVariableTypeTableAttribute::~LocalVariableTypeTableAttribute()
    {

        for (uint16_t i = 0; i < table.length(); ++i)
        {
            DELETE_OBJECT(table[i])
        }
    }

    /**
     *
     */
    RuntimeInvisibleAnnotationsAttribute::~RuntimeInvisibleAnnotationsAttribute()
    {

        for (uint16_t i = 0; i < annotations.length(); ++i)
        {
            DELETE_OBJECT(annotations[i])
        }
    }

    /**
     *
     */
    RuntimeInvisibleParameterAnnotationsAttribute::~RuntimeInvisibleParameterAnnotationsAttribute()
    {

        for (uint16_t i = 0; i < parameterAnnotations.length(); ++i)
        {
            DELETE_OBJECT(parameterAnnotations[i])
        }
    }

    /**
     *
     */
    RuntimeVisibleAnnotationsAttribute::~RuntimeVisibleAnnotationsAttribute()
    {

        for (uint16_t i = 0; i < annotations.length(); ++i)
        {
            DELETE_OBJECT(annotations[i])
        }
    }

    /**
     *
     */
    RuntimeVisibleParameterAnnotationsAttribute::~RuntimeVisibleParameterAnnotationsAttribute()
    {

        for (uint16_t i = 0; i < parameterAnnotations.length(); ++i)
        {
            DELETE_OBJECT(parameterAnnotations[i])
        }
    }

    /**
     *
     */
    SourceDebugExtensionAttribute::~SourceDebugExtensionAttribute()
    {

        DELETE_ARRAY(debugExtension);
    }

    /**
     *
     */
    StackMapTableAttribute::~StackMapTableAttribute()
    {

        for (uint16_t i = 0; i < entries.length(); ++i)
        {
            DELETE_OBJECT(entries[i])
        }
    }

}

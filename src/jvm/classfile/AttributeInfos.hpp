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

#ifndef COLDSPOT_JVM_CLASSFILE_ATTRIBUTEINFOS_HPP_
#define COLDSPOT_JVM_CLASSFILE_ATTRIBUTEINFOS_HPP_

#include <cstdint>

#include <jvm/common/SmartArray.hpp>

namespace coldspot
{

    /**
     *
     */
    class AttributeInfo
    {
    public:

        uint16_t nameIndex;
        uint32_t length;

        virtual ~AttributeInfo()
        {
        }
    };

    class ElementValue;

    /**
     *
     */
    class AnnotationDefaultAttribute : public AttributeInfo
    {
    public:

        ElementValue *defaultValue;

        AnnotationDefaultAttribute() : defaultValue(0)
        {
        }

        ~AnnotationDefaultAttribute();
    };

    class BootstrapMethod;

    /**
     *
     */
    class BootstrapMethodsAttribute : public AttributeInfo
    {
    public:

        SmartArray<BootstrapMethod *, uint16_t> bootstrapMethods;

        ~BootstrapMethodsAttribute();
    };

    class ExceptionTableEntry;

    /**
     *
     */
    class CodeAttribute : public AttributeInfo
    {
    public:

        uint16_t maxStack;
        uint16_t maxLocals;
        uint32_t codeLength;
        uint8_t *code;
        SmartArray<ExceptionTableEntry *, uint16_t> exceptionTable;
        SmartArray<AttributeInfo *, uint16_t> attributes;

        CodeAttribute() : maxStack(0), maxLocals(0), codeLength(0), code(0)
        {
        }

        ~CodeAttribute();
    };

    /**
     *
     */
    class ConstantValueAttribute : public AttributeInfo
    {
    public:

        uint16_t valueIndex;
    };

    /**
     *
     */
    class DeprecatedAttribute : public AttributeInfo
    {
    };

    /**
     *
     */
    class EnclosingMethodAttribute : public AttributeInfo
    {
    public:

        uint16_t classIndex;
        uint16_t methodIndex;
    };

    /**
     *
     */
    class ExceptionsAttribute : public AttributeInfo
    {
    public:

        SmartArray <uint16_t, uint16_t> exceptionIndexTable;
    };

    class InnerClassesInfo;

    /**
     *
     */
    class InnerClassesAttribute : public AttributeInfo
    {
    public:

        SmartArray<InnerClassesInfo *, uint16_t> classes;

        ~InnerClassesAttribute();
    };

    class LineNumberTableEntry;

    /**
     *
     */
    class LineNumberTableAttribute : public AttributeInfo
    {
    public:

        SmartArray<LineNumberTableEntry *, uint16_t> lineNumberTable;

        ~LineNumberTableAttribute();
    };

    class LocalVariableTableEntry;

    /**
     *
     */
    class LocalVariableTableAttribute : public AttributeInfo
    {
    public:

        SmartArray<LocalVariableTableEntry *, uint16_t> table;

        ~LocalVariableTableAttribute();
    };

    class LocalVariableTypeTableEntry;

    /**
     *
     */
    class LocalVariableTypeTableAttribute : public AttributeInfo
    {
    public:

        SmartArray<LocalVariableTypeTableEntry *, uint16_t> table;

        ~LocalVariableTypeTableAttribute();
    };

    class Annotation;

    /**
     *
     */
    class RuntimeInvisibleAnnotationsAttribute : public AttributeInfo
    {
    public:

        SmartArray<Annotation *, uint16_t> annotations;

        ~RuntimeInvisibleAnnotationsAttribute();
    };

    class ParameterAnnotation;

    /**
     *
     */
    class RuntimeInvisibleParameterAnnotationsAttribute : public AttributeInfo
    {
    public:

        SmartArray<ParameterAnnotation *, uint8_t> parameterAnnotations;

        ~RuntimeInvisibleParameterAnnotationsAttribute();
    };

    class Annotation;

    /**
     *
     */
    class RuntimeVisibleAnnotationsAttribute : public AttributeInfo
    {
    public:

        SmartArray<Annotation *, uint16_t> annotations;

        ~RuntimeVisibleAnnotationsAttribute();
    };

    class ParameterAnnotation;

    /**
     *
     */
    class RuntimeVisibleParameterAnnotationsAttribute : public AttributeInfo
    {
    public:

        SmartArray<ParameterAnnotation *, uint8_t> parameterAnnotations;

        ~RuntimeVisibleParameterAnnotationsAttribute();
    };

    /**
     *
     */
    class SignatureAttribute : public AttributeInfo
    {
    public:

        uint16_t signatureIndex;
    };

    /**
     *
     */
    class SourceDebugExtensionAttribute : public AttributeInfo
    {
    public:

        uint8_t *debugExtension;

        SourceDebugExtensionAttribute() : debugExtension(0)
        {
        }

        ~SourceDebugExtensionAttribute();
    };

    /**
     *
     */
    class SourceFileAttribute : public AttributeInfo
    {
    public:

        uint16_t sourcefileIndex;
    };

    class StackMapFrame;

    /**
     *
     */
    class StackMapTableAttribute : public AttributeInfo
    {
    public:

        SmartArray<StackMapFrame *, uint16_t> entries;

        ~StackMapTableAttribute();
    };

    /**
     *
     */
    class SyntheticAttribute : public AttributeInfo
    {
    };

}

#endif

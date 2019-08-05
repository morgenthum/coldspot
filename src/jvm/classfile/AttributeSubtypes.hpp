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

#ifndef COLDSPOT_JVM_CLASSFILE_ATTRIBUTESUBTYPES_HPP_
#define COLDSPOT_JVM_CLASSFILE_ATTRIBUTESUBTYPES_HPP_

#include <cstdint>

#include <jvm/common/SmartArray.hpp>

namespace coldspot
{

    class ElementValuePair;

    /**
     *
     */
    class Annotation
    {
    public:

        uint16_t typeIndex;
        SmartArray<ElementValuePair *, uint16_t> elementValuePairs;

        ~Annotation();
    };

    /**
     *
     */
    class BootstrapMethod
    {
    public:
        uint16_t bootstrapMethodRef;

        SmartArray <uint16_t, uint16_t> bootstrapArguments;
    };

    /**
     *
     */
    class ElementValue
    {
    public:

        uint8_t tag;
    };

    /**
     *
     */
    class ConstValue : public ElementValue
    {
    public:

        uint16_t constValueIndex;
    };

    /**
     *
     */
    class EnumConstValue : public ElementValue
    {
    public:

        uint16_t typeNameIndex;
        uint16_t constNameIndex;
    };

    /**
     *
     */
    class ClassInfoValue : public ElementValue
    {
    public:

        uint16_t classInfoIndex;
    };

    class Annotation;

    /**
     *
     */
    class AnnotationValue : public ElementValue
    {
    public:

        Annotation *value;

        AnnotationValue() : value(0)
        {
        }

        ~AnnotationValue();
    };

    class ElementValue;

    /**
     *
     */
    class ArrayValue : public ElementValue
    {
    public:

        SmartArray<ElementValue *, uint16_t> values;

        ~ArrayValue();
    };

    class ElementValue;

    /**
     *
     */
    class ElementValuePair
    {
    public:

        uint16_t elementNameIndex;
        ElementValue *value;

        ElementValuePair() : elementNameIndex(0), value(0)
        {
        }

        ~ElementValuePair();
    };

    /**
     *
     */
    class ExceptionTableEntry
    {
    public:

        uint16_t startPc;
        uint16_t endPc;
        uint16_t handlerPc;
        uint16_t catchType;
    };

    /**
     *
     */
    class InnerClassesInfo
    {
    public:

        uint16_t innerClassInfoIndex;
        uint16_t outerClassInfoIndex;
        uint16_t innerNameIndex;
        uint16_t innerClassAccessFlags;
    };

    /**
     *
     */
    class LineNumberTableEntry
    {
    public:

        uint16_t startPc;
        uint16_t lineNumber;
    };

    /**
     *
     */
    class LocalVariableTableEntry
    {
    public:

        uint16_t startPc;
        uint16_t length;
        uint16_t nameIndex;
        uint16_t descriptorIndex;
        uint16_t index;
    };

    /**
     *
     */
    class LocalVariableTypeTableEntry
    {
    public:

        uint16_t startPc;
        uint16_t length;
        uint16_t nameIndex;
        uint16_t signatureIndex;
        uint16_t index;
    };

    class Annotation;

    /**
     *
     */
    class ParameterAnnotation
    {
    public:

        SmartArray<Annotation *, uint16_t> annotations;

        ~ParameterAnnotation();
    };

    /**
     *
     */
    class StackMapFrame
    {
    public:

        virtual ~StackMapFrame()
        {
        }
    };

    /**
     *
     */
    class SameFrame : public StackMapFrame
    {
    public:

        uint8_t frameType;
    };

    class VerificationTypeInfo;

    /**
     *
     */
    class SameLocals1StackItemFrame : public StackMapFrame
    {
    public:

        uint8_t frameType;
        VerificationTypeInfo *stackElement;

        SameLocals1StackItemFrame() : stackElement(0)
        {
        }

        ~SameLocals1StackItemFrame();
    };

    class VerificationTypeInfo;

    /**
     *
     */
    class SameLocals1StackItemFrameExtended : public StackMapFrame
    {
    public:

        uint8_t frameType;
        uint16_t offsetDelta;
        VerificationTypeInfo *stackElement;

        SameLocals1StackItemFrameExtended() : stackElement(0)
        {
        }

        ~SameLocals1StackItemFrameExtended();
    };

    /**
     *
     */
    class ChopFrame : public StackMapFrame
    {
    public:

        uint8_t frameType;
        uint16_t offsetDelta;
    };

    /**
     *
     */
    class SameFrameExtended : public StackMapFrame
    {
    public:

        uint8_t frameType;
        uint16_t offsetDelta;
    };

    class VerificationTypeInfo;

    /**
     *
     */
    class AppendFrame : public StackMapFrame
    {
    public:

        uint8_t frameType;
        uint16_t offsetDelta;
        SmartArray<VerificationTypeInfo *, uint16_t> locals;

        ~AppendFrame();
    };

    class VerificationTypeInfo;

    /**
     *
     */
    class FullFrame : public StackMapFrame
    {
    public:

        uint8_t frameType;
        uint16_t offsetDelta;
        SmartArray<VerificationTypeInfo *, uint16_t> locals;
        SmartArray<VerificationTypeInfo *, uint16_t> stack;

        ~FullFrame();
    };

    /**
     *
     */
    class VerificationTypeInfo
    {
    public:

        uint8_t tag;
    };

    /**
     *
     */
    class TopVariableInfo : public VerificationTypeInfo
    {
    };

    /**
     *
     */
    class IntegerVariableInfo : public VerificationTypeInfo
    {
    };

    /**
     *
     */
    class FloatVariableInfo : public VerificationTypeInfo
    {
    };

    /**
     *
     */
    class LongVariableInfo : public VerificationTypeInfo
    {
    };

    /**
     *
     */
    class DoubleVariableInfo : public VerificationTypeInfo
    {
    };

    /**
     *
     */
    class NullVariableInfo : public VerificationTypeInfo
    {
    };

    /**
     *
     */
    class UninitializedThisVariableInfo : public VerificationTypeInfo
    {
    };

    /**
     *
     */
    class ObjectVariableInfo : public VerificationTypeInfo
    {
    public:

        uint16_t cpoolIndex;
    };

    /**
     *
     */
    class UninitializedVariableInfo : public VerificationTypeInfo
    {
    public:

        uint16_t offset;
    };

}

#endif

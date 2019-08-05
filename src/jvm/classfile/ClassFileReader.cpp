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

    error_t ClassFileReader::init(const String &className)
    {
        if (_input_stream->load(className))
        {
            return RETURN_OK;
        }

        if (className == CLASSNAME_CLASSNOTFOUNDEXCEPTION)
        {
            EXIT_FATAL("failed to load " << className.c_str())
        }

        _current_executor->throw_exception(CLASSNAME_CLASSNOTFOUNDEXCEPTION,
            className.c_str());

        return RETURN_EXCEPTION;
    }


    error_t ClassFileReader::read_class()
    {
        uint32_t magic;
        read(&magic);
        if (magic != 0xCAFEBABE)
        {
            _current_executor->throw_exception(CLASSNAME_LINKAGEERROR);
            return RETURN_EXCEPTION;
        }

        uint16_t minorVersion;
        read(&minorVersion);
        uint16_t majorVersion;
        read(&majorVersion);

        uint16_t constantPoolCount;
        read(&constantPoolCount);
        _class_file->constantPool.init(constantPoolCount);
        error_t errorValue = read_constant_pool();
        RETURN_ON_FAIL(errorValue);

        read(&_class_file->accessFlags);
        read(&_class_file->thisClass);
        read(&_class_file->superClass);

        uint16_t interfacesCount;
        read(&interfacesCount);
        _class_file->interfaces.init(interfacesCount);
        read_interfaces();

        uint16_t fieldsCount;
        read(&fieldsCount);
        _class_file->fields.init(fieldsCount);
        errorValue = read_fields();
        RETURN_ON_FAIL(errorValue);

        uint16_t methodsCount;
        read(&methodsCount);
        _class_file->methods.init(methodsCount);
        errorValue = read_methods();
        RETURN_ON_FAIL(errorValue);

        uint16_t attributesCount;
        read(&attributesCount);
        _class_file->attributes.init(attributesCount);
        return read_attributes(_class_file);
    }


    error_t ClassFileReader::read_constant_pool()
    {
        auto &constantPool = _class_file->constantPool;
        constantPool[0] = 0;

        for (uint16_t i = 1; i < constantPool.length(); ++i)
        {
            ConstantPoolEntry *entry = 0;
            bool increment = false;
            uint8_t tag;

            read(&tag);

            switch (tag)
            {
                case CP_CLASS:
                {
                    ClassInfoEntry *temp = new ClassInfoEntry;
                    read(&temp->nameIndex);
                    entry = temp;
                    break;
                }

                case CP_FIELDREF:
                {
                    FieldrefInfoEntry *temp = new FieldrefInfoEntry;
                    read(&temp->classIndex);
                    read(&temp->nameAndTypeIndex);
                    entry = temp;
                    break;
                }

                case CP_METHODREF:
                {
                    MethodrefInfoEntry *temp = new MethodrefInfoEntry;
                    read(&temp->classIndex);
                    read(&temp->nameAndTypeIndex);
                    entry = temp;
                    break;
                }

                case CP_INTERFACEMETHODREF:
                {
                    InterfaceMethodrefInfoEntry *temp = new InterfaceMethodrefInfoEntry;
                    read(&temp->classIndex);
                    read(&temp->nameAndTypeIndex);
                    entry = temp;
                    break;
                }

                case CP_STRING:
                {
                    StringInfoEntry *temp = new StringInfoEntry;
                    read(&temp->stringIndex);
                    entry = temp;
                    break;
                }

                case CP_INTEGER:
                {
                    IntegerInfoEntry *temp = new IntegerInfoEntry;
                    read(&temp->bytes);
                    entry = temp;
                    break;
                }

                case CP_FLOAT:
                {
                    FloatInfoEntry *temp = new FloatInfoEntry;
                    read(&temp->bytes);
                    entry = temp;
                    break;
                }

                case CP_LONG:
                {
                    LongInfoEntry *temp = new LongInfoEntry;
                    read(&temp->highBytes);
                    read(&temp->lowBytes);
                    entry = temp;
                    increment = true;
                    break;
                }

                case CP_DOUBLE:
                {
                    DoubleInfoEntry *temp = new DoubleInfoEntry;
                    read(&temp->highBytes);
                    read(&temp->lowBytes);
                    entry = temp;
                    increment = true;
                    break;
                }

                case CP_NAMEANDTYPE:
                {
                    NameAndTypeInfoEntry *temp = new NameAndTypeInfoEntry;
                    read(&temp->nameIndex);
                    read(&temp->descriptorIndex);
                    entry = temp;
                    break;
                }

                case CP_UTF8:
                {
                    Utf8InfoEntry *temp = new Utf8InfoEntry;
                    read(&temp->length);
                    temp->bytes = new uint8_t[temp->length];
                    for (uint16_t i = 0; i < temp->length; ++i)
                    {
                        read(temp->bytes + i);
                    }
                    entry = temp;
                    break;
                }

                case CP_METHODHANDLE:
                {
                    MethodHandleInfoEntry *temp = new MethodHandleInfoEntry;
                    read(&temp->referenceKind);
                    read(&temp->referenceIndex);
                    entry = temp;
                    break;
                }

                case CP_METHODTYPE:
                {
                    MethodTypeInfoEntry *temp = new MethodTypeInfoEntry;
                    read(&temp->descriptorIndex);
                    entry = temp;
                    break;
                }

                case CP_INVOKEDYNAMIC:
                {
                    InvokeDynamicInfoEntry *temp = new InvokeDynamicInfoEntry;
                    read(&temp->bootstrapMethodAttributeIndex);
                    read(&temp->nameAndTypeIndex);
                    entry = temp;
                    break;
                }
            }

            if (entry == 0)
            {
                _current_executor->throw_exception(CLASSNAME_LINKAGEERROR);
                return RETURN_EXCEPTION;
            }

            entry->tag = tag;

            constantPool[i] = entry;

            if (increment)
            {
                constantPool[++i] = 0;
            }
        }

        return RETURN_OK;
    }

    /**
     *
     */
    void ClassFileReader::read_interfaces()
    {

        auto &interfaces = _class_file->interfaces;

        for (uint16_t i = 0; i < interfaces.length(); ++i)
        {
            uint16_t interface;
            read(&interface);

            interfaces[i] = interface;
        }
    }

    /**
     *
     */
    error_t ClassFileReader::read_fields()
    {

        auto &fields = _class_file->fields;

        for (uint16_t i = 0; i < fields.length(); ++i)
        {
            fields[i] = new FieldInfo;

            read(&fields[i]->accessFlags);
            read(&fields[i]->nameIndex);
            read(&fields[i]->descriptorIndex);

            uint16_t attributesCount;
            read(&attributesCount);

            fields[i]->attributes.init(attributesCount);
            error_t errorValue = read_attributes(fields[i]);
            RETURN_ON_FAIL(errorValue);
        }

        return RETURN_OK;
    }

    /**
     *
     */
    error_t ClassFileReader::read_methods()
    {

        auto &methods = _class_file->methods;

        for (uint16_t i = 0; i < methods.length(); ++i)
        {
            methods[i] = new MethodInfo;

            read(&methods[i]->accessFlags);
            read(&methods[i]->nameIndex);
            read(&methods[i]->descriptorIndex);

            uint16_t attributesCount;
            read(&attributesCount);

            methods[i]->attributes.init(attributesCount);
            error_t errorValue = read_attributes(methods[i]);
            RETURN_ON_FAIL(errorValue);
        }

        return RETURN_OK;
    }

    /**
     *
     */
    template<typename T>
    error_t ClassFileReader::read_attributes(T *target)
    {

        auto &attributes = target->attributes;

        for (uint16_t i = 0; i < attributes.length(); ++i)
        {
            error_t errorValue = read_attribute(&attributes[i]);
            RETURN_ON_FAIL(errorValue);
        }

        return RETURN_OK;
    }

    /**
     *
     */
    error_t ClassFileReader::read_attribute(AttributeInfo **attribute)
    {

        uint16_t attributeNameIndex;
        uint32_t attributeLength;

        read(&attributeNameIndex);
        read(&attributeLength);

        Utf8InfoEntry *nameEntry = static_cast<Utf8InfoEntry *>(_class_file->constantPool[attributeNameIndex]);
        String name((char *) nameEntry->bytes, nameEntry->length);

        if (name == "ConstantValue")
        {
            ConstantValueAttribute *temp = new ConstantValueAttribute;
            read(&temp->valueIndex);
            *attribute = temp;
        }

        else if (name == "Code")
        {
            CodeAttribute *temp = new CodeAttribute;
            read(&temp->maxStack);
            read(&temp->maxLocals);
            read(&temp->codeLength);
            temp->code = new uint8_t[temp->codeLength];
            for (uint32_t i = 0; i < temp->codeLength; ++i)
            {
                read(&temp->code[i]);
            }

            uint16_t exceptionTableLength;
            read(&exceptionTableLength);

            temp->exceptionTable.init(exceptionTableLength);

            for (uint16_t i = 0; i < exceptionTableLength; ++i)
            {
                ExceptionTableEntry *entry = new ExceptionTableEntry;
                read(&entry->startPc);
                read(&entry->endPc);
                read(&entry->handlerPc);
                read(&entry->catchType);
                temp->exceptionTable[i] = entry;
            }

            uint16_t attributesCount;
            read(&attributesCount);

            temp->attributes.init(attributesCount);
            read_attributes(temp);
            *attribute = temp;
        }

        else if (name == "StackMapTable")
        {
            StackMapTableAttribute *temp = new StackMapTableAttribute;

            uint16_t entriesCount;
            read(&entriesCount);

            temp->entries.init(entriesCount);

            for (uint16_t i = 0; i < entriesCount; ++i)
            {
                uint8_t frameType;
                read(&frameType);
                if (frameType >= 0 && frameType <= 63)
                {
                    SameFrame *frame = new SameFrame;
                    frame->frameType = frameType;
                    temp->entries[i] = frame;
                }
                else if (frameType >= 64 && frameType <= 127)
                {
                    local <SameLocals1StackItemFrame> frame(
                        new SameLocals1StackItemFrame);
                    frame->frameType = frameType;
                    error_t errorValue = read_verification_type(
                        &frame->stackElement);
                    RETURN_ON_FAIL(errorValue);
                    temp->entries[i] = frame.release();
                }
                else if (frameType == 247)
                {
                    local <SameLocals1StackItemFrameExtended> frame(
                        new SameLocals1StackItemFrameExtended);
                    frame->frameType = frameType;
                    read(&frame->offsetDelta);
                    error_t errorValue = read_verification_type(
                        &frame->stackElement);
                    RETURN_ON_FAIL(errorValue);
                    temp->entries[i] = frame.release();
                }
                else if (frameType >= 248 && frameType <= 250)
                {
                    ChopFrame *frame = new ChopFrame;
                    frame->frameType = frameType;
                    read(&frame->offsetDelta);
                    temp->entries[i] = frame;
                }
                else if (frameType == 251)
                {
                    SameFrameExtended *frame = new SameFrameExtended;
                    frame->frameType = frameType;
                    read(&frame->offsetDelta);
                    temp->entries[i] = frame;
                }
                else if (frameType >= 252 && frameType <= 254)
                {
                    local <AppendFrame> frame(new AppendFrame);
                    frame->frameType = frameType;
                    read(&frame->offsetDelta);
                    uint8_t length = frameType - 251;

                    frame->locals.init(length);

                    for (uint8_t i = 0; i < length; ++i)
                    {
                        error_t errorValue = read_verification_type(
                            &frame->locals[i]);
                        RETURN_ON_FAIL(errorValue);
                    }

                    temp->entries[i] = frame.release();
                }
                else if (frameType == 255)
                {
                    local <FullFrame> frame(new FullFrame);
                    frame->frameType = frameType;
                    read(&frame->offsetDelta);

                    uint16_t localsCount;
                    read(&localsCount);

                    frame->locals.init(localsCount);

                    for (uint8_t i = 0; i < localsCount; ++i)
                    {
                        error_t errorValue = read_verification_type(
                            &frame->locals[i]);
                        RETURN_ON_FAIL(errorValue);
                    }

                    uint16_t stackItemsCount;
                    read(&stackItemsCount);

                    frame->stack.init(stackItemsCount);

                    for (uint8_t i = 0; i < stackItemsCount; ++i)
                    {
                        error_t errorValue = read_verification_type(
                            &frame->stack[i]);
                        RETURN_ON_FAIL(errorValue);
                    }

                    temp->entries[i] = frame.release();
                }
            }

            *attribute = temp;
        }

        else if (name == "Exceptions")
        {
            ExceptionsAttribute *temp = new ExceptionsAttribute;

            uint16_t exceptionIndexTableLength;
            read(&exceptionIndexTableLength);

            temp->exceptionIndexTable.init(exceptionIndexTableLength);

            for (uint16_t i = 0; i < exceptionIndexTableLength; ++i)
            {
                read(&temp->exceptionIndexTable[i]);
            }

            *attribute = temp;
        }

        else if (name == "InnerClasses")
        {
            InnerClassesAttribute *temp = new InnerClassesAttribute;

            uint16_t classesCount;
            read(&classesCount);

            temp->classes.init(classesCount);

            for (uint16_t i = 0; i < classesCount; ++i)
            {
                InnerClassesInfo *info = new InnerClassesInfo;
                read(&info->innerClassInfoIndex);
                read(&info->outerClassInfoIndex);
                read(&info->innerNameIndex);
                read(&info->innerClassAccessFlags);
                temp->classes[i] = info;
            }

            *attribute = temp;
        }

        else if (name == "EnclosingMethod")
        {
            EnclosingMethodAttribute *temp = new EnclosingMethodAttribute;

            read(&temp->classIndex);
            read(&temp->methodIndex);

            *attribute = temp;
        }

        else if (name == "Synthetic")
        {
            *attribute = new SyntheticAttribute;
        }

        else if (name == "Signature")
        {
            SignatureAttribute *temp = new SignatureAttribute;

            read(&temp->signatureIndex);

            *attribute = temp;
        }

        else if (name == "SourceFile")
        {
            SourceFileAttribute *temp = new SourceFileAttribute;

            read(&temp->sourcefileIndex);

            *attribute = temp;
        }

        else if (name == "SourceDebugExtension")
        {
            SourceDebugExtensionAttribute *temp = new SourceDebugExtensionAttribute;

            temp->debugExtension = new uint8_t[attributeLength];

            for (uint32_t i = 0; i < attributeLength; ++i)
            {
                read(&temp->debugExtension[i]);
            }

            *attribute = temp;
        }

        else if (name == "LineNumberTable")
        {
            LineNumberTableAttribute *temp = new LineNumberTableAttribute;

            uint16_t lineNumberTableLength;
            read(&lineNumberTableLength);

            temp->lineNumberTable.init(lineNumberTableLength);

            for (uint16_t i = 0; i < lineNumberTableLength; ++i)
            {
                LineNumberTableEntry *entry = new LineNumberTableEntry;
                read(&entry->startPc);
                read(&entry->lineNumber);
                temp->lineNumberTable[i] = entry;
            }

            *attribute = temp;
        }

        else if (name == "LocalVariableTable")
        {
            LocalVariableTableAttribute *temp = new LocalVariableTableAttribute;

            uint16_t tableLength;
            read(&tableLength);

            temp->table.init(tableLength);

            for (uint16_t i = 0; i < tableLength; ++i)
            {
                LocalVariableTableEntry *entry = new LocalVariableTableEntry;
                read(&entry->startPc);
                read(&entry->length);
                read(&entry->nameIndex);
                read(&entry->descriptorIndex);
                read(&entry->index);
                temp->table[i] = entry;
            }

            *attribute = temp;
        }

        else if (name == "LocalVariableTypeTable")
        {
            LocalVariableTypeTableAttribute *temp = new LocalVariableTypeTableAttribute;

            uint16_t tableLength;
            read(&tableLength);

            temp->table.init(tableLength);

            for (uint16_t i = 0; i < tableLength; ++i)
            {
                LocalVariableTypeTableEntry *entry = new LocalVariableTypeTableEntry;
                read(&entry->startPc);
                read(&entry->length);
                read(&entry->nameIndex);
                read(&entry->signatureIndex);
                read(&entry->index);
                temp->table[i] = entry;
            }

            *attribute = temp;
        }

        else if (name == "Deprecated")
        {
            *attribute = new DeprecatedAttribute;
        }

        else if (name == "RuntimeVisibleAnnotations")
        {
            local <RuntimeVisibleAnnotationsAttribute> temp(
                new RuntimeVisibleAnnotationsAttribute);

            uint16_t annotationsCount;
            read(&annotationsCount);

            temp->annotations.init(annotationsCount);

            for (uint16_t i = 0; i < annotationsCount; ++i)
            {
                error_t errorValue = read_annotation(&temp->annotations[i]);
                RETURN_ON_FAIL(errorValue);
            }

            *attribute = temp.release();
        }

        else if (name == "RuntimeInvisibleAnnotations")
        {
            local <RuntimeInvisibleAnnotationsAttribute> temp(
                new RuntimeInvisibleAnnotationsAttribute);

            uint16_t annotationsCount;
            read(&annotationsCount);

            temp->annotations.init(annotationsCount);

            for (uint16_t i = 0; i < annotationsCount; ++i)
            {
                error_t errorValue = read_annotation(&temp->annotations[i]);
                RETURN_ON_FAIL(errorValue);
            }

            *attribute = temp.release();
        }

        else if (name == "RuntimeVisibleParameterAnnotations")
        {
            local <RuntimeVisibleParameterAnnotationsAttribute> temp(
                new RuntimeVisibleParameterAnnotationsAttribute);

            uint8_t parametersCount;
            read(&parametersCount);

            temp->parameterAnnotations.init(parametersCount);

            for (uint16_t i = 0; i < parametersCount; ++i)
            {
                ParameterAnnotation *parameter = new ParameterAnnotation;

                uint16_t annotationsCount;
                read(&annotationsCount);

                parameter->annotations.init(annotationsCount);

                for (uint16_t j = 0; j < annotationsCount; ++j)
                {
                    error_t errorValue = read_annotation(
                        &parameter->annotations[j]);
                    RETURN_ON_FAIL(errorValue);
                }

                temp->parameterAnnotations[i] = parameter;
            }

            *attribute = temp.release();
        }

        else if (name == "RuntimeInvisibleParameterAnnotations")
        {
            local <RuntimeInvisibleParameterAnnotationsAttribute> temp(
                new RuntimeInvisibleParameterAnnotationsAttribute);

            uint8_t parametersCount;
            read(&parametersCount);

            temp->parameterAnnotations.init(parametersCount);

            for (uint16_t i = 0; i < parametersCount; ++i)
            {
                ParameterAnnotation *parameter = new ParameterAnnotation;
                temp->parameterAnnotations[i] = parameter;

                uint16_t annotationsCount;
                read(&annotationsCount);

                parameter->annotations.init(annotationsCount);

                for (uint16_t j = 0; j < annotationsCount; ++j)
                {
                    error_t errorValue = read_annotation(
                        &parameter->annotations[j]);
                    RETURN_ON_FAIL(errorValue);
                }
            }

            *attribute = temp.release();
        }

        else if (name == "AnnotationDefault")
        {
            local <AnnotationDefaultAttribute> temp(
                new AnnotationDefaultAttribute);

            error_t errorValue = read_element_value(&temp->defaultValue);
            RETURN_ON_FAIL(errorValue);

            *attribute = temp.release();
        }

        else if (name == "BootstrapMethods")
        {
            BootstrapMethodsAttribute *temp = new BootstrapMethodsAttribute;

            uint16_t bootstrapMethodsCount;
            read(&bootstrapMethodsCount);

            temp->bootstrapMethods.init(bootstrapMethodsCount);

            for (uint16_t i = 0; i < bootstrapMethodsCount; ++i)
            {
                BootstrapMethod *method = new BootstrapMethod;
                read(&method->bootstrapMethodRef);

                uint16_t bootstrapArgumentsCount;
                read(&bootstrapArgumentsCount);

                method->bootstrapArguments.init(bootstrapArgumentsCount);

                for (uint16_t i = 0; i < bootstrapArgumentsCount; ++i)
                {
                    read(&method->bootstrapArguments[i]);
                }
            }

            *attribute = temp;
        }

        if (!*attribute)
        {
            _current_executor->throw_exception(CLASSNAME_LINKAGEERROR,
                name.c_str());
            return RETURN_EXCEPTION;
        }

        (*attribute)->nameIndex = attributeNameIndex;
        (*attribute)->length = attributeLength;

        return RETURN_OK;
    }

    /**
     *
     */
    error_t ClassFileReader::read_verification_type(VerificationTypeInfo **info)
    {

        uint8_t tag;
        read(&tag);

        if (tag == 0)
        {
            *info = new TopVariableInfo;
        }
        else if (tag == 1)
        {
            *info = new IntegerVariableInfo;
        }
        else if (tag == 2)
        {
            *info = new FloatVariableInfo;
        }
        else if (tag == 4)
        {
            *info = new LongVariableInfo;
        }
        else if (tag == 3)
        {
            *info = new DoubleVariableInfo;
        }
        else if (tag == 5)
        {
            *info = new NullVariableInfo;
        }
        else if (tag == 6)
        {
            *info = new UninitializedThisVariableInfo;
        }
        else if (tag == 7)
        {
            ObjectVariableInfo *temp = new ObjectVariableInfo;
            read(&temp->cpoolIndex);
            *info = temp;
        }
        else if (tag == 8)
        {
            UninitializedVariableInfo *temp = new UninitializedVariableInfo;
            read(&temp->offset);
            *info = temp;
        }
        else
        {
            _current_executor->throw_exception(CLASSNAME_LINKAGEERROR);
            return RETURN_EXCEPTION;
        }

        (*info)->tag = tag;

        return RETURN_OK;
    }

    /**
     *
     */
    error_t ClassFileReader::read_annotation(Annotation **annotation)
    {

        *annotation = new Annotation;

        read(&(*annotation)->typeIndex);

        uint16_t elementValuePairsCount;
        read(&elementValuePairsCount);

        (*annotation)->elementValuePairs.init(elementValuePairsCount);

        for (uint16_t i = 0; i < elementValuePairsCount; ++i)
        {
            local <ElementValuePair> pair(new ElementValuePair);

            read(&pair->elementNameIndex);
            error_t errorValue = read_element_value(&pair->value);
            RETURN_ON_FAIL(errorValue);

            (*annotation)->elementValuePairs[i] = pair.release();
        }

        return RETURN_OK;
    }

    /**
     *
     */
    error_t ClassFileReader::read_element_value(ElementValue **value)
    {

        uint8_t tag;
        read(&tag);

        switch (tag)
        {

            case 'B':
            case 'C':
            case 'D':
            case 'F':
            case 'I':
            case 'J':
            case 'S':
            case 'Z':
            case 's':
            {
                ConstValue *temp = new ConstValue;
                read(&temp->constValueIndex);
                *value = temp;
                break;
            }

            case 'e':
            {
                EnumConstValue *temp = new EnumConstValue;
                read(&temp->typeNameIndex);
                read(&temp->constNameIndex);
                *value = temp;
                break;
            }

            case 'c':
            {
                ClassInfoValue *temp = new ClassInfoValue;
                read(&temp->classInfoIndex);
                *value = temp;
                break;
            }

            case '@':
            {
                local <AnnotationValue> temp(new AnnotationValue);

                error_t errorValue = read_annotation(&temp->value);
                RETURN_ON_FAIL(errorValue);

                *value = temp.release();
                break;
            }

            case '[':
            {
                local <ArrayValue> temp(new ArrayValue);
                uint16_t valuesCount;
                read(&valuesCount);

                temp->values.init(valuesCount);

                for (uint16_t i = 0; i < valuesCount; ++i)
                {
                    error_t errorValue = read_element_value(&temp->values[i]);
                    RETURN_ON_FAIL(errorValue);
                }

                *value = temp.release();
                break;
            }
        }

        if (!value)
        {
            _current_executor->throw_exception(CLASSNAME_LINKAGEERROR);
            return RETURN_EXCEPTION;
        }

        (*value)->tag = tag;

        return RETURN_OK;
    }

    /**
     *
     */
    template<typename T>
    inline void ClassFileReader::read(T *target)
    {

        _input_stream->read(reinterpret_cast<uint8_t *>(target), sizeof(T));

        swap_endianness(target);
    }

    /**
     *
     */
    template<typename T>
    inline void ClassFileReader::swap_endianness(T *target)
    {

        uint8_t *start = reinterpret_cast<uint8_t *>(target);
        uint8_t *low = start;
        uint8_t *high = start + sizeof(T) - 1;

        while (low < high)
        {
            uint8_t swap = *low;
            *low++ = *high;
            *high-- = swap;
        }
    }

}

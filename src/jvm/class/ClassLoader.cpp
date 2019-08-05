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

    ClassLoader::~ClassLoader()
    {
        auto begin = _loaded_classes.begin();
        auto end = _loaded_classes.end();

        while (begin != end)
        {
            DELETE_OBJECT(begin->value);
            ++begin;
        }
    }


    error_t ClassLoader::load_class(const String &name)
    {
        Class *clazz;
        return load_class(name, &clazz);
    }


    error_t ClassLoader::load_class(const String &name, Class **clazz)
    {
        return load_class(name, 0, clazz);
    }


    error_t ClassLoader::load_class(const String &name, Object *classLoader,
        Class **clazz)
    {
        _load_mutex.lock();

        auto entry = _loaded_classes.get(ClassIdentifier(classLoader, name));
        if (entry != 0)
        {
            *clazz = entry->value;
            RETURN_UNLOCK(RETURN_OK, _load_mutex);
        }

        // Load class with bootstrap class loader
        if (classLoader == 0)
        {
            SystemClassFileInputStream inputStream;
            error_t errorValue = define_class(name, classLoader, &inputStream,
                clazz);
            RETURN_UNLOCK(errorValue, _load_mutex);
        }

        // Load class with the specified class loader
        Object *loadedClassObject;
        String javaName = Class::to_java_class_name(name);
        error_t errorValue = java_lang_ClassLoader::loadClass(classLoader,
            javaName, &loadedClassObject);
        if (errorValue != RETURN_OK)
        {
            auto executor = _current_executor;

            if (!executor->frames().empty())
            {
                Frame *topFrame = (Frame *) executor->frames().peek();
                Object *exception = topFrame->exception;

                // Convert java.lang.ClassNotFoundException
                // to java.lang.NoClassDefFoundError
                if (exception != 0 &&
                    exception->type()->name == CLASSNAME_CLASSNOTFOUNDEXCEPTION)
                {
                    topFrame->exception = 0;
                    executor->throw_exception(CLASSNAME_NOCLASSDEFFOUNDERROR,
                        javaName.c_str());
                }
            }
            RETURN_UNLOCK(RETURN_EXCEPTION, _load_mutex)
        }

        // Throw java/lang/NoClassDefFoundError if the loaded class is null
        if (loadedClassObject == 0)
        {
            _current_executor->throw_exception(CLASSNAME_NOCLASSDEFFOUNDERROR,
                javaName.c_str());
            RETURN_UNLOCK(RETURN_EXCEPTION, _load_mutex)
        }

        // Store loaded class
        *clazz = Class::from_class_object(loadedClassObject);

        RETURN_UNLOCK(RETURN_OK, _load_mutex);
    }


    error_t ClassLoader::load_array(const String &name, Object *classLoader,
        Class **clazz)
    {
        _load_mutex.lock();

        // Check if the class is already loaded
        auto entry = _loaded_classes.get(ClassIdentifier(classLoader, name));
        if (entry != 0)
        {
            *clazz = entry->value;
            RETURN_UNLOCK(RETURN_OK, _load_mutex)
        }

        // Logging
        LOG_DEBUG_VERBOSE(Class, "load array '" << name.c_str() << "'")

        // Create basics
        local <Class> localClass(new Class);
        localClass->initialized = true;
        localClass->class_loader = classLoader;
        localClass->name = name;
        localClass->type = TYPE_REFERENCE;
        localClass->type_size = sizeof(Array * );

        // Load component-type
        uint16_t index = 1;
        error_t errorValue = resolve_by_descriptor(name, index, classLoader,
            &localClass->component_type);
        RETURN_ON_FAIL_UNLOCK(errorValue, _load_mutex)

        // Load super-class
        errorValue = load_class(CLASSNAME_OBJECT, &localClass->super_class);
        RETURN_ON_FAIL_UNLOCK(errorValue, _load_mutex)

        // All array-classes are cloneable ...
        Class *cloneableClass;
        errorValue = load_class(CLASSNAME_CLONEABLE, &cloneableClass);
        RETURN_ON_FAIL_UNLOCK(errorValue, _load_mutex)
        localClass->interface_classes.put(CLASSNAME_CLONEABLE, cloneableClass);

        // ... and serializable
        Class *serializableClass;
        errorValue = load_class(CLASSNAME_SERIALIZABLE, &serializableClass);
        RETURN_ON_FAIL_UNLOCK(errorValue, _load_mutex)
        localClass->interface_classes.put(CLASSNAME_SERIALIZABLE,
            serializableClass);

        // Register class
        _loaded_classes.put(ClassIdentifier(classLoader, name),
            localClass.get());
        *clazz = localClass.release();

        // Create java.lang.Class instance
        errorValue = create_object(*clazz);
        RETURN_UNLOCK(errorValue, _load_mutex);
    }


    error_t ClassLoader::load_primitive(const String &name, Class **clazz)
    {
        _load_mutex.lock();

        // Check if the class is already loaded
        auto entry = _loaded_classes.get(ClassIdentifier(0, name));
        if (entry != 0)
        {
            *clazz = entry->value;
            RETURN_UNLOCK(RETURN_OK, _load_mutex);
        }

        // Logging
        LOG_DEBUG_VERBOSE(Class, "load primitive '" << name.c_str() << "'")

        // Create class
        local <Class> localClass(new Class);
        localClass->initialized = true;
        localClass->name = name;
        localClass->primitive = true;

        switch (name[0])
        {
            case 'V':
                localClass->type = TYPE_VOID;
                localClass->type_size = 0;
                break;
            case 'Z':
                localClass->type = TYPE_BOOLEAN;
                localClass->type_size = sizeof(jboolean);
                break;
            case 'B':
                localClass->type = TYPE_BYTE;
                localClass->type_size = sizeof(jbyte);
                break;
            case 'C':
                localClass->type = TYPE_CHAR;
                localClass->type_size = sizeof(jchar);
                break;
            case 'S':
                localClass->type = TYPE_SHORT;
                localClass->type_size = sizeof(jshort);
                break;
            case 'I':
                localClass->type = TYPE_INT;
                localClass->type_size = sizeof(jint);
                break;
            case 'F':
                localClass->type = TYPE_FLOAT;
                localClass->type_size = sizeof(jfloat);
                break;
            case 'J':
                localClass->type = TYPE_LONG;
                localClass->type_size = sizeof(jlong);
                break;
            case 'D':
                localClass->type = TYPE_DOUBLE;
                localClass->type_size = sizeof(jdouble);
                break;
            default:
                EXIT_FATAL("unknown primitive type");
        }

        // Object and type size are equal for non reference types (primitive)
        localClass->object_size = localClass->type_size;

        // Register class
        _loaded_classes.put(ClassIdentifier(0, name), localClass.get());
        *clazz = localClass.release();

        // Create java/lang/Class instance
        error_t errorValue = create_object(*clazz);
        RETURN_UNLOCK(errorValue, _load_mutex);
    }


    error_t ClassLoader::define_class(const String &name, Object *classLoader,
        ClassFileInputStream *inputStream, Class **clazz)
    {
        auto iterator = _loaded_classes.get(ClassIdentifier(classLoader, name));
        if (iterator != 0)
        {
            *clazz = iterator->value;
            return RETURN_OK;
        }

        // Logging
        LOG_DEBUG_VERBOSE(Class, "define class '" << name.c_str() << "'")

        // Create class
        local <Class> localClass(new Class);
        localClass->class_loader = classLoader;
        localClass->name = name;
        localClass->type = TYPE_REFERENCE;
        localClass->type_size = sizeof(Object * );

        // Read class file
        error_t errorValue = read_classfile(localClass.get(), inputStream);
        RETURN_ON_FAIL(errorValue);

        // Initialize runtime-contant-pool
        localClass->contant_pool.init(
            localClass->class_file->constantPool.length());

        String className = name;

        // Load the class-name from class-file if it isn't specified
        if (className.empty())
        {
            ClassFile *classFile = localClass->class_file;
            ConstantPoolEntry *entry = classFile->constantPool[classFile->thisClass];
            ClassInfoEntry *classInfoEntry = static_cast<ClassInfoEntry *>(entry);
            className = localClass->get_utf8_from_cp(classInfoEntry->nameIndex);
            localClass->name = className;
        }

        // Register class
        *clazz = localClass.release();
        _loaded_classes.put(ClassIdentifier(classLoader, className), *clazz);

        // Link class
        errorValue = link_class(*clazz);
        if (errorValue != RETURN_OK)
        {
            // TODO delete registered class
            LOG_WARN("failed to link class")
            return errorValue;
        }

        // If java/lang/Class is loaded, create class-objects
        // for all previously loaded classes
        if (name == CLASSNAME_CLASS)
        {
            auto begin = _loaded_classes.begin();
            auto end = _loaded_classes.end();

            while (begin != end)
            {
                create_object(begin->value);
                ++begin;
            }
        }
            // Create java/lang/Class instance
        else
        {
            return create_object(*clazz);
        }

        return RETURN_OK;
    }


    // TODO see https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-5.html#jvms-5.5
    error_t ClassLoader::initialize_class(Class *clazz)
    {
        _load_mutex.lock();

        // Nothing to do if the class is already initialized
        if (clazz->initialized)
        {
            RETURN_UNLOCK(RETURN_OK, _load_mutex)
        }

        // Mark as initialized
        clazz->initialized = true;

        // If the class has a super-class, initialize it first
        if (clazz->super_class != 0)
        {
            error_t errorValue = initialize_class(clazz->super_class);
            RETURN_ON_FAIL_UNLOCK(errorValue, _load_mutex)
        }

        LOG_DEBUG_VERBOSE(Class,
            "initialize class '" << clazz->name.c_str() << "'")

        // Find and invoke static initialization method if it exists
        Method *method;
        error_t errorValue = clazz->get_declared_method(
            Signature("()V", METHODNAME_STATICINIT), &method);
        if (errorValue == RETURN_OK)
        {
            Value value;
            errorValue = method->invoke(0, 0, &value);
            RETURN_ON_FAIL_UNLOCK(errorValue, _load_mutex)
        }

        RETURN_UNLOCK(RETURN_OK, _load_mutex)
    }


    error_t ClassLoader::read_classfile(Class *clazz,
        ClassFileInputStream *input_stream)
    {
        // Create class-file
        local <ClassFile> class_file(new ClassFile);

        // Initialize input-stream
        ClassFileReader reader(input_stream, class_file.get());
        error_t errorValue = reader.init(clazz->name);
        RETURN_ON_FAIL(errorValue);

        // Read class-file
        errorValue = reader.read_class();
        RETURN_ON_FAIL(errorValue);

        // Associate class-file with class-type
        clazz->class_file = class_file.release();

        return RETURN_OK;
    }


    error_t ClassLoader::create_object(Class *clazz)
    {
        // Load java/lang/Class class
        Class *classClass;
        error_t errorValue = load_class(CLASSNAME_CLASS, &classClass);
        RETURN_ON_FAIL(errorValue);

        if (classClass->resolved)
        {
            Object *javaClass;
            errorValue = _vm->memory_manager()->allocate_object(classClass,
                &javaClass);
            RETURN_ON_FAIL(errorValue);

            clazz->object = javaClass;
            _object_mapping.put(javaClass, clazz);

            if (classClass->initialized)
            {
                Method *method;
                errorValue = classClass->get_method(
                    Signature("(Ljava/lang/ClassLoader;)V",
                        METHODNAME_CONSTRUCTOR), &method);
                RETURN_ON_FAIL(errorValue);

                Value result;
                Value
                parameter((Object *) 0);
                method->invoke(javaClass, &parameter, &result);
                RETURN_ON_FAIL(errorValue);
            }
        }

        return RETURN_OK;
    }


    error_t ClassLoader::link_class(Class *clazz)
    {
        error_t errorValue = resolve_class(clazz);
        RETURN_ON_FAIL(errorValue);

        prepare_class(clazz);

        return RETURN_OK;
    }


    void ClassLoader::prepare_class(Class *clazz)
    {
        uint32_t memory_size = 0;

        for (uint16_t i = 0; i < clazz->declared_fields.length(); ++i)
        {
            Field *field = clazz->declared_fields[i];

            if (field->is_static())
            {
                field->set_offset(memory_size);
                memory_size += field->type()->type_size;
            }
        }

        if (memory_size != 0)
        {
            clazz->static_memory_size = memory_size;
            clazz->static_memory = (uint8_t *) calloc(1, memory_size);
        }
    }


    error_t ClassLoader::resolve_class(Class *clazz)
    {
        ClassFile *classFile = clazz->class_file;
        uint16_t superClassIndex = classFile->superClass;

        // Resolve super-class
        if (superClassIndex)
        {
            Class *superClass;
            error_t errorValue = clazz->get_class_from_cp(superClassIndex,
                &superClass);
            RETURN_ON_FAIL(errorValue);

            clazz->super_class = superClass;
            clazz->object_size += superClass->object_size;
        }

        // Resolve interface-classes
        for (uint16_t i = 0; i < classFile->interfaces.length(); ++i)
        {
            Class *interfaceClass;
            uint16_t interface = classFile->interfaces[i];
            error_t errorValue = clazz->get_class_from_cp(interface,
                &interfaceClass);
            RETURN_ON_FAIL(errorValue);

            clazz->interface_classes.put(interfaceClass->name, interfaceClass);
        }

        // Resolve Attributes
        for (uint16_t i = 0; i < classFile->attributes.length(); ++i)
        {
            AttributeInfo *attribute = classFile->attributes[i];
            String &name = clazz->get_utf8_from_cp(attribute->nameIndex);
            if (name == "SourceFile")
            {
                SourceFileAttribute *sourceAttribute = (SourceFileAttribute *) attribute;
                clazz->source_file = clazz->get_utf8_from_cp(
                    sourceAttribute->sourcefileIndex);
            }
        }

        // Resolve fields
        auto &fieldInfos = classFile->fields;
        clazz->declared_fields.init(fieldInfos.length());
        for (uint16_t i = 0; i < fieldInfos.length(); ++i)
        {
            FieldInfo *fieldInfo = fieldInfos[i];
            String &descriptor = clazz->get_utf8_from_cp(
                fieldInfo->descriptorIndex);
            String &name = clazz->get_utf8_from_cp(fieldInfo->nameIndex);
            error_t errorValue = resolve_field(clazz,
                Signature(descriptor, name));
            RETURN_ON_FAIL(errorValue);
        }

        // Resolve methods
        auto &methodInfos = classFile->methods;
        clazz->declared_methods.init(methodInfos.length());
        for (uint16_t i = 0; i < methodInfos.length(); ++i)
        {
            MethodInfo *methodInfo = methodInfos[i];
            String &descriptor = clazz->get_utf8_from_cp(
                methodInfo->descriptorIndex);
            String &name = clazz->get_utf8_from_cp(methodInfo->nameIndex);
            error_t errorValue = resolve_method(clazz,
                Signature(descriptor, name));
            RETURN_ON_FAIL(errorValue);
        }

        clazz->resolved = true;

        return RETURN_OK;
    }


    error_t ClassLoader::resolve_field(Class *clazz, const Signature &signature)
    {
        // Try to find Field in Class
        auto entry = clazz->declared_field_slots.get(signature);
        if (entry != 0)
        {
            return RETURN_OK;
        }

        // Resolve Field from FieldInfo and store it in Class
        ClassFile *classFile = clazz->class_file;
        auto &fieldInfos = classFile->fields;

        for (uint16_t i = 0; i < fieldInfos.length(); ++i)
        {
            FieldInfo *fieldInfo = fieldInfos[i];

            String &currentDescriptor = clazz->get_utf8_from_cp(
                fieldInfo->descriptorIndex);
            String &currentName = clazz->get_utf8_from_cp(fieldInfo->nameIndex);

            if (signature == Signature(currentDescriptor, currentName))
            {
                local <Field> field(new Field(clazz, signature));
                field->set_access_flags(fieldInfo->accessFlags);

                error_t errorValue = resolve_field_signature(clazz, fieldInfo,
                    field.get());
                RETURN_ON_FAIL(errorValue);

                if (!field->is_static())
                {
                    clazz->object_size += field->type()->type_size;
                }

                uint16_t slot = 0;
                for (uint16_t i = 0; i < clazz->declared_fields.length(); ++i)
                {
                    if (clazz->declared_fields[i] == 0)
                    {
                        slot = i;
                        break;
                    }
                }

                field->set_slot(slot);
                clazz->declared_fields[slot] = field.release();
                clazz->declared_field_slots.put(signature, slot);

                return RETURN_OK;
            }
        }

        _current_executor->throw_exception(CLASSNAME_NOSUCHFIELDERROR,
            signature.name.c_str());
        return RETURN_EXCEPTION;
    }


    error_t ClassLoader::resolve_method(Class *clazz,
        const Signature &signature)
    {
        // Try to find Method in Class
        auto entry = clazz->declared_method_slots.get(signature);
        if (entry != 0)
        {
            return RETURN_OK;
        }

        // Resolve Method from MethodInfo and store it in Class
        ClassFile *classFile = clazz->class_file;
        auto &methodInfos = classFile->methods;

        for (uint16_t i = 0; i < methodInfos.length(); ++i)
        {
            MethodInfo *methodInfo = methodInfos[i];

            String &currentDescriptor = clazz->get_utf8_from_cp(
                methodInfo->descriptorIndex);
            String &currentName = clazz->get_utf8_from_cp(
                methodInfo->nameIndex);

            if (signature == Signature(currentDescriptor, currentName))
            {
                local <Method> method(new Method(clazz, signature));
                method->set_access_flags(methodInfo->accessFlags);

                error_t errorValue = collect_method_attributes(clazz,
                    methodInfo, method.get());
                RETURN_ON_FAIL(errorValue);

                method->set_frame_size(Frame::size(method.get()));

                errorValue = resolve_method_signature(clazz, methodInfo,
                    method.get());
                RETURN_ON_FAIL(errorValue);

                uint16_t slot = 0;
                for (uint16_t i = 0; i < clazz->declared_methods.length(); ++i)
                {
                    if (clazz->declared_methods[i] == 0)
                    {
                        slot = i;
                        break;
                    }
                }

                method->set_slot(slot);
                clazz->declared_methods[slot] = method.release();
                clazz->declared_method_slots.put(signature, slot);

                return RETURN_OK;
            }
        }

        _current_executor->throw_exception(CLASSNAME_NOSUCHMETHODERROR,
            signature.name.c_str());
        return RETURN_EXCEPTION;
    }


    error_t ClassLoader::resolve_field_signature(Class *clazz, FieldInfo *info,
        Field *field)
    {
        uint16_t descriptorIndex = info->descriptorIndex;

        String &descriptor = clazz->get_utf8_from_cp(descriptorIndex);
        uint16_t index = 0;
        Class *type;
        error_t errorValue = resolve_by_descriptor(descriptor, index,
            clazz->class_loader, &type);
        RETURN_ON_FAIL(errorValue);

        field->set_type(type);

        return RETURN_OK;
    }


    error_t ClassLoader::resolve_method_signature(Class *clazz,
        MethodInfo *info, Method *method)
    {
        uint16_t descriptorIndex = info->descriptorIndex;

        String &descriptor = clazz->get_utf8_from_cp(descriptorIndex);
        uint16_t index = 1;
        error_t errorValue;

        for (; ;)
        {
            Class *parameterType;
            if ((errorValue = resolve_by_descriptor(descriptor, index,
                clazz->class_loader, &parameterType)) != RETURN_OK)
            {
                if (errorValue == RETURN_EXCEPTION)
                {
                    return errorValue;
                }
                break;
            }

            method->parameter_types().addBack(parameterType);
        }

        Class *returnType;
        errorValue = resolve_by_descriptor(descriptor, ++index,
            clazz->class_loader, &returnType);
        RETURN_ON_FAIL(errorValue);

        method->set_return_type(returnType);

        return RETURN_OK;
    }


    error_t ClassLoader::resolve_by_descriptor(const String &descriptor,
        uint16_t &index, Object *classLoader, Class **clazz)
    {
        char16_t first = descriptor[index];

        switch (first)
        {
            case 'B':
            {
                *clazz = PrimitiveClass<jbyte>::get();
                ++index;
                return RETURN_OK;
            }
            case 'C':
            {
                *clazz = PrimitiveClass<jchar>::get();
                ++index;
                return RETURN_OK;
            }
            case 'D':
            {
                *clazz = PrimitiveClass<jdouble>::get();
                ++index;
                return RETURN_OK;
            }
            case 'F':
            {
                *clazz = PrimitiveClass<jfloat>::get();
                ++index;
                return RETURN_OK;
            }
            case 'I':
            {
                *clazz = PrimitiveClass<jint>::get();
                ++index;
                return RETURN_OK;
            }
            case 'J':
            {
                *clazz = PrimitiveClass<jlong>::get();
                ++index;
                return RETURN_OK;
            }
            case 'S':
            {
                *clazz = PrimitiveClass<jshort>::get();
                ++index;
                return RETURN_OK;
            }
            case 'Z':
            {
                *clazz = PrimitiveClass<jboolean>::get();
                ++index;
                return RETURN_OK;
            }
            case 'V':
            {
                *clazz = PrimitiveClass<void>::get();
                ++index;
                return RETURN_OK;
            }
            case 'L':
            {
                StringBuilder builder;
                while (descriptor[++index] != ';')
                {
                    builder << descriptor[index];
                }
                ++index;

                return load_class(builder.str(), classLoader, clazz);
            }
            case '[':
            {
                StringBuilder builder;
                do
                {
                    builder << descriptor[index];
                } while (descriptor[++index] == '[');

                if (descriptor[index] == 'L')
                {
                    builder << descriptor[index];
                    while (descriptor[++index] != ';')
                    {
                        builder << descriptor[index];
                    }
                }
                builder << descriptor[index++];

                return load_array(builder.str(), classLoader, clazz);
            }
        }

        return RETURN_ERROR;
    }


    error_t ClassLoader::collect_method_attributes(Class *clazz,
        MethodInfo *info, Method *method)
    {
        for (uint16_t i = 0; i < info->attributes.length(); ++i)
        {
            AttributeInfo *attribute = info->attributes[i];
            String &name = clazz->get_utf8_from_cp(attribute->nameIndex);

            // Code-Attribute
            if (name == "Code")
            {
                CodeAttribute *codeAttribute = (CodeAttribute *) attribute;

                // Resolve basics
                method->set_code(codeAttribute->code);
                method->set_locals_count(codeAttribute->maxLocals);
                method->set_operands_count(codeAttribute->maxStack);

                // Resolve exception-handlers
                for (uint16_t i = 0;
                     i < codeAttribute->exceptionTable.length(); ++i)
                {
                    ExceptionTableEntry *entry = codeAttribute->exceptionTable[i];

                    local <ExceptionHandler> handler(new ExceptionHandler);
                    handler->startPc = entry->startPc;
                    handler->endPc = entry->endPc;
                    handler->handlerPc = entry->handlerPc;

                    if (entry->catchType != 0)
                    {
                        error_t errorValue = clazz->get_class_from_cp(
                            entry->catchType, &handler->catchClass);
                        RETURN_ON_FAIL(errorValue);
                    }

                    method->exception_handlers().addBack(handler.release());
                }

                local <MethodDebugInfos> debugInfos;

                // Child-attributes of Code-Attribute
                for (uint16_t i = 0;
                     i < codeAttribute->attributes.length(); ++i)
                {
                    AttributeInfo *subAttribute = codeAttribute->attributes[i];
                    String &subName = clazz->get_utf8_from_cp(
                        subAttribute->nameIndex);

                    // LineNumberTable-Attribute
                    if (subName == "LineNumberTable")
                    {
                        if (debugInfos.get() == 0)
                        {
                            debugInfos.set(new MethodDebugInfos);
                        }

                        LineNumberTableAttribute *lineAttribute = (LineNumberTableAttribute *) subAttribute;
                        for (uint16_t i = 0;
                             i < lineAttribute->lineNumberTable.length(); ++i)
                        {
                            LineNumberTableEntry *entry = lineAttribute->lineNumberTable[i];
                            debugInfos->lineMapping.put(entry->startPc,
                                entry->lineNumber);
                        }
                    }
                        // LocalVariableTable-Attribute
                    else if (subName == "LocalVariableTable")
                    {
                        if (debugInfos.get() == 0)
                        {
                            debugInfos.set(new MethodDebugInfos);
                        }

                        LocalVariableTableAttribute *localVarAttribute = (LocalVariableTableAttribute *) subAttribute;
                        for (uint16_t i = 0;
                             i < localVarAttribute->table.length(); ++i)
                        {
                            local <LocalVariableInfo> localVar(
                                new LocalVariableInfo);

                            LocalVariableTableEntry *entry = localVarAttribute->table[i];
                            localVar->index = entry->index;
                            localVar->startPc = entry->startPc;
                            localVar->endPc = entry->startPc + entry->length;
                            localVar->signature.descriptor = clazz->get_utf8_from_cp(
                                entry->descriptorIndex);
                            localVar->signature.name = clazz->get_utf8_from_cp(
                                entry->nameIndex);

                            debugInfos->localVariableInfos.addBack(
                                localVar.release());
                        }
                    }
                }

                method->set_debug_infos(debugInfos.release());

                break;
            }
        }

        return RETURN_OK;
    }

}

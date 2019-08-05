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

    Class *Class::from_class_object(Object *object)
    {
        return _vm->class_loader()->object_mapping().get(object)->value;
    }


    String Class::to_java_class_name(const String &internalName)
    {
        String javaName = internalName;
        javaName.replace('/', '.');
        return javaName;
    }


    Class::~Class()
    {
        DELETE_OBJECT(class_file)

        for (uint16_t i = 0; i < declared_fields.length(); ++i)
        {
            DELETE_OBJECT(declared_fields[i])
        }

        for (uint16_t i = 0; i < declared_methods.length(); ++i)
        {
            DELETE_OBJECT(declared_methods[i])
        }

        for (uint16_t i = 0; i < contant_pool.length(); ++i)
        {
            DELETE_OBJECT(contant_pool[i])
        }

        FREE_OBJECT(static_memory)
    }


    error_t Class::get_declared_field(const Signature &signature, Field **field)
    {
        auto entry = declared_field_slots.get(signature);
        if (entry == 0)
        {
            return RETURN_ERROR;
        }

        *field = get_declared_field(entry->value);

        return RETURN_OK;
    }


    Field *Class::get_declared_field(uint16_t slot)
    {
        return declared_fields[slot];
    }


    void Class::get_declared_fields(List < Field * > &fields)
    {
        for (uint16_t i = 0; i < declared_fields.length(); ++i)
        {
            fields.addBack(declared_fields[i]);
        }
    }


    error_t Class::get_declared_method(const Signature &signature,
        Method **method)
    {
        auto entry = declared_method_slots.get(signature);
        if (entry == 0)
        {
            return RETURN_ERROR;
        }

        *method = declared_methods[entry->value];

        return RETURN_OK;
    }


    Method *Class::get_declared_method(uint16_t slot)
    {
        return declared_methods[slot];
    }


    void Class::get_declared_methods(List < Method * > &methods)
    {
        for (uint16_t i = 0; i < declared_methods.length(); ++i)
        {
            methods.addBack(declared_methods[i]);
        }
    }


    error_t Class::get_field(const Signature &signature, Field **field)
    {
        auto entry = fields.get(signature);
        if (entry != 0)
        {
            *field = entry->value;
            return RETURN_OK;
        }

        error_t errorValue = lookup_field(signature, field);
        if (errorValue != RETURN_OK)
        {
            _current_executor->throw_exception(CLASSNAME_NOSUCHFIELDERROR);
            return RETURN_EXCEPTION;
        }

        fields.put(signature, *field);

        return RETURN_OK;
    }


    error_t Class::get_method(const Signature &signature, Method **method)
    {
        auto entry = methods.get(signature);
        if (entry != 0)
        {
            *method = entry->value;
            return RETURN_OK;
        }

        error_t errorValue = lookup_method(signature, method);
        if (errorValue != RETURN_OK && signature.name != METHODNAME_STATICINIT)
        {
            _current_executor->throw_exception(CLASSNAME_NOSUCHMETHODERROR,
                signature.name.c_str());
            return RETURN_EXCEPTION;
        }

        if (errorValue == RETURN_OK)
        {
            methods.put(signature, *method);
            return RETURN_OK;
        }

        return RETURN_ERROR;
    }


    bool Class::is_abstract()
    {
        return (class_file->accessFlags & ACCESS_FLAG_ABSTRACT) != 0;
    }


    bool Class::is_array()
    {
        return name[0] == '[';
    }


    bool Class::is_implementing(Class *clazz)
    {
        auto begin = interface_classes.begin();
        auto end = interface_classes.end();

        while (begin != end)
        {
            Class *interfaceClass = begin->value;
            if (interfaceClass == clazz ||
                interfaceClass->is_subclass_of(clazz))
            {
                return true;
            }

            ++begin;
        }

        if (super_class != 0)
        {
            return super_class->is_implementing(clazz);
        }

        return false;
    }


    bool Class::is_interface()
    {
        return class_file->accessFlags & ACCESS_FLAG_INTERFACE;
    }


    bool Class::is_primitive()
    {
        return primitive;
    }


    bool Class::is_subclass_of(Class *clazz)
    {
        Class *current = super_class;

        while (current != 0)
        {
            if (current == clazz)
            {
                return true;
            }
            current = current->super_class;
        }

        return false;
    }


    bool Class::is_castable_to(Class *clazz)
    {
        if (is_array())
        {
            // Array to array
            if (clazz->is_array())
            {
                Class *thisType = component_type;
                Class *checkType = clazz->component_type;
                if (thisType->is_primitive() && checkType->is_primitive())
                {
                    return thisType->type == checkType->type;
                }
                else if (!thisType->is_primitive() &&
                         !checkType->is_primitive())
                {
                    return thisType->is_castable_to(checkType);
                }
            }
            else
            {
                // Array to interface
                if (clazz->is_interface())
                {
                    if (clazz->name == CLASSNAME_CLONEABLE ||
                        clazz->name == CLASSNAME_SERIALIZABLE)
                    {
                        return true;
                    }
                }
                    // Array to class
                else if (clazz->name == CLASSNAME_OBJECT)
                {
                    return true;
                }
            }
        }
        else
        {
            // Check cast to interface
            if (this->is_interface())
            {
                if (!clazz->is_array())
                {
                    if (clazz->is_interface())
                    {
                        if (this == clazz || this->is_subclass_of(clazz))
                        {
                            return true;
                        }
                    }
                    else if (clazz->name == CLASSNAME_OBJECT)
                    {
                        return true;
                    }
                }
                // Check cast to class
            }
            else
            {
                if (!clazz->is_array())
                {
                    if (clazz->is_interface())
                    {
                        if (this->is_implementing(clazz))
                        {
                            return true;
                        }
                    }
                    else if (this == clazz || this->is_subclass_of(clazz))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }


    error_t Class::get_class_from_cp(uint16_t index, Class **clazz)
    {
        auto entry = contant_pool[index];
        if (entry != 0)
        {
            RunTimeClassInfoEntry *runTimeClassEntry = (RunTimeClassInfoEntry *) entry;
            *clazz = runTimeClassEntry->clazz;
            return RETURN_OK;
        }

        ClassInfoEntry *classEntry = (ClassInfoEntry *) class_file->constantPool[index];
        String &className = get_utf8_from_cp(classEntry->nameIndex);

        error_t errorValue;
        if (className[0] == '[')
        {
            errorValue = _vm->class_loader()->load_array(className,
                class_loader, clazz);
        }
        else
        {
            errorValue = _vm->class_loader()->load_class(className,
                class_loader, clazz);
        }
        RETURN_ON_FAIL(errorValue);

        RunTimeClassInfoEntry *runTimeClassEntry = new RunTimeClassInfoEntry;
        runTimeClassEntry->clazz = *clazz;
        contant_pool[index] = runTimeClassEntry;

        return RETURN_OK;
    }


    error_t Class::get_field_from_cp(uint16_t index, Field **field)
    {
        auto entry = contant_pool[index];
        if (entry != 0)
        {
            *field = ((RunTimeFieldrefInfoEntry *) entry)->field;
            return RETURN_OK;
        }

        FieldrefInfoEntry *fieldEntry = (FieldrefInfoEntry *) class_file->constantPool[index];

        Class *clazz;
        error_t errorValue = get_class_from_cp(fieldEntry->classIndex, &clazz);
        RETURN_ON_FAIL(errorValue);

        NameAndTypeInfoEntry *signatureEntry = (NameAndTypeInfoEntry *) class_file->constantPool[fieldEntry->nameAndTypeIndex];

        String &descriptor = get_utf8_from_cp(signatureEntry->descriptorIndex);
        String &name = get_utf8_from_cp(signatureEntry->nameIndex);

        errorValue = clazz->get_field(Signature(descriptor, name), field);
        RETURN_ON_FAIL(errorValue);

        RunTimeFieldrefInfoEntry *runTimeFieldEntry = new RunTimeFieldrefInfoEntry;
        runTimeFieldEntry->field = *field;
        contant_pool[index] = runTimeFieldEntry;

        return RETURN_OK;
    }


    error_t Class::get_method_from_cp(uint16_t index, Method **method)
    {
        auto entry = contant_pool[index];
        if (entry != 0)
        {
            *method = ((RunTimeMethodrefInfoEntry *) entry)->method;
            return RETURN_OK;
        }

        MethodrefInfoEntry *methodEntry = (MethodrefInfoEntry *) class_file->constantPool[index];

        Class *clazz;
        error_t errorValue = get_class_from_cp(methodEntry->classIndex, &clazz);
        RETURN_ON_FAIL(errorValue);

        NameAndTypeInfoEntry *signatureEntry = (NameAndTypeInfoEntry *) class_file->constantPool[methodEntry->nameAndTypeIndex];
        String &descriptor = get_utf8_from_cp(signatureEntry->descriptorIndex);
        String &name = get_utf8_from_cp(signatureEntry->nameIndex);

        errorValue = clazz->get_method(Signature(descriptor, name), method);
        RETURN_ON_FAIL(errorValue);

        RunTimeMethodrefInfoEntry *runTimeMethodEntry = new RunTimeMethodrefInfoEntry;
        runTimeMethodEntry->method = *method;
        contant_pool[index] = runTimeMethodEntry;

        return RETURN_OK;
    }


    error_t Class::get_string_from_cp(uint16_t index, Object **string)
    {
        StringInfoEntry *stringEntry = (StringInfoEntry *) class_file->constantPool[index];
        String &stringConstant = get_utf8_from_cp(stringEntry->stringIndex);

        auto entry = _vm->string_pool().get(stringConstant);
        if (entry != 0)
        {
            *string = entry->value;
            return RETURN_OK;
        }

        error_t errorValue = java_lang_String::new_(stringConstant, string);
        RETURN_ON_FAIL(errorValue);

        _vm->string_pool().put(stringConstant, *string);

        return RETURN_OK;
    }


    jint Class::get_integer_from_cp(uint16_t index)
    {
        auto entry = contant_pool[index];
        if (entry != 0)
        {
            return ((RunTimeIntegerInfoEntry *) entry)->value;
        }

        IntegerInfoEntry *integerEntry = (IntegerInfoEntry *) class_file->constantPool[index];

        RunTimeIntegerInfoEntry *runTimeIntegerEntry = new RunTimeIntegerInfoEntry;
        runTimeIntegerEntry->value = integerEntry->bytes;
        contant_pool[index] = runTimeIntegerEntry;

        return runTimeIntegerEntry->value;
    }


    jfloat Class::get_float_from_cp(uint16_t index)
    {
        auto entry = contant_pool[index];
        if (entry != 0)
        {
            return ((RunTimeFloatInfoEntry *) entry)->value;
        }

        FloatInfoEntry *floatEntry = (FloatInfoEntry *) class_file->constantPool[index];

        RunTimeFloatInfoEntry *runTimeFloatEntry = new RunTimeFloatInfoEntry;
        contant_pool[index] = runTimeFloatEntry;
        runTimeFloatEntry->value = Value(Type::TYPE_FLOAT,
            floatEntry->bytes).as_float();

        return runTimeFloatEntry->value;
    }


    jlong Class::get_long_from_cp(uint16_t index)
    {
        auto entry = contant_pool[index];
        if (entry != 0)
        {
            return ((RunTimeLongInfoEntry *) entry)->value;
        }

        LongInfoEntry *longEntry = (LongInfoEntry *) class_file->constantPool[index];
        uint64_t highBytes = longEntry->highBytes;
        uint64_t lowBytes = longEntry->lowBytes;

        RunTimeLongInfoEntry *runTimeLongEntry = new RunTimeLongInfoEntry;
        runTimeLongEntry->value = (highBytes << 32) | lowBytes;
        contant_pool[index] = runTimeLongEntry;

        return runTimeLongEntry->value;
    }


    jdouble Class::get_double_from_cp(uint16_t index)
    {
        auto entry = contant_pool[index];
        if (entry != 0)
        {
            return ((RunTimeDoubleInfoEntry *) entry)->value;
        }

        DoubleInfoEntry *doubleEntry = (DoubleInfoEntry *) class_file->constantPool[index];
        uint64_t highBytes = doubleEntry->highBytes;
        uint64_t lowBytes = doubleEntry->lowBytes;
        uint64_t value = (highBytes << 32) | lowBytes;

        RunTimeDoubleInfoEntry *runTimeDoubleEntry = new RunTimeDoubleInfoEntry;
        runTimeDoubleEntry->value = Value(Type::TYPE_DOUBLE, value).as_double();
        contant_pool[index] = runTimeDoubleEntry;

        return runTimeDoubleEntry->value;
    }


    String &Class::get_utf8_from_cp(uint16_t index)
    {
        auto entry = contant_pool[index];
        if (entry != 0)
        {
            return ((RunTimeUtf8InfoEntry *) entry)->value;
        }

        Utf8InfoEntry *utf8Entry = (Utf8InfoEntry *) class_file->constantPool[index];

        RunTimeUtf8InfoEntry *runTimeUtf8Entry = new RunTimeUtf8InfoEntry;
        runTimeUtf8Entry->value = String((char *) utf8Entry->bytes,
            utf8Entry->length);
        contant_pool[index] = runTimeUtf8Entry;

        return runTimeUtf8Entry->value;
    }


    template<typename T>
    T Class::get_value(uint32_t offset)
    {
        return *((T *) (static_memory + offset));
    }

#define EXPLICIT(type) template type Class::get_value<type>(uint32_t offset);
    EXPLICIT(Object *)
    EXPLICIT(jboolean)
    EXPLICIT(jbyte)
    EXPLICIT(jchar)
    EXPLICIT(jshort)
    EXPLICIT(jint)
    EXPLICIT(jfloat)
    EXPLICIT(jlong)
    EXPLICIT(jdouble)
#undef EXPLICIT


    template<typename T>
    void Class::set_value(uint32_t offset, T value)
    {
        *((T *) (static_memory + offset)) = value;
    }

#define EXPLICIT(type) template void Class::set_value<type>(uint32_t offset, type value);
    EXPLICIT(Object *)
    EXPLICIT(jboolean)
    EXPLICIT(jbyte)
    EXPLICIT(jchar)
    EXPLICIT(jshort)
    EXPLICIT(jint)
    EXPLICIT(jfloat)
    EXPLICIT(jlong)
    EXPLICIT(jdouble)
#undef EXPLICIT


    error_t Class::lookup_field(const Signature &signature, Field **field)
    {
        error_t errorValue = get_declared_field(signature, field);
        RETURN_ON_SUCCESS(errorValue)

        auto begin = interface_classes.begin();
        auto end = interface_classes.end();

        while (begin != end)
        {
            Class *interfaceClass = begin->value;

            errorValue = interfaceClass->lookup_field(signature, field);
            RETURN_ON_SUCCESS(errorValue)

                ++
            begin;
        }

        if (super_class != 0)
        {
            errorValue = super_class->lookup_field(signature, field);
            RETURN_ON_SUCCESS(errorValue)
        }

        return RETURN_ERROR;
    }


    error_t Class::lookup_method(const Signature &signature, Method **method)
    {
        error_t errorValue = get_declared_method(signature, method);
        RETURN_ON_SUCCESS(errorValue)

        if (super_class != 0)
        {
            errorValue = super_class->lookup_method(signature, method);
            RETURN_ON_SUCCESS(errorValue)
        }

        auto begin = interface_classes.begin();
        auto end = interface_classes.end();

        while (begin != end)
        {
            Class *interfaceClass = begin->value;

            errorValue = interfaceClass->lookup_method(signature, method);
            RETURN_ON_SUCCESS(errorValue)

                ++
            begin;
        }

        return RETURN_ERROR;
    }

}

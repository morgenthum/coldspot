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

#include <cmath>

#include <jvm/Global.hpp>

// Generic instructions
#define IF(operator) \
  uint8_t* currentCode = code; \
  int16_t offset = (*(++code) << 8) | *(++code); \
  if (frame->pop().as_int() operator 0) { \
    code = currentCode + offset; \
  } else { \
    ++code; \
  } \
  SAFEPOINT

#define IFXNULL(operator) \
  uint8_t* currentCode = code; \
  int16_t offset = (*(++code) << 8) | *(++code); \
  if (frame->pop().as_object() operator 0) { \
    code = currentCode + offset; \
  } else { \
    ++code; \
  } \
  SAFEPOINT

#define IF_ACMP(operator) \
  uint8_t* currentCode = code; \
  int16_t offset = (*(++code) << 8) | *(++code); \
  Object* object2 = frame->pop().as_object(); \
  Object* object1 = frame->pop().as_object(); \
  if (object1 operator object2) { \
    code = currentCode + offset; \
  } else { \
    ++code; \
  } \
  SAFEPOINT

#define IF_ICMP(operator) \
  uint8_t* currentCode = code; \
  int16_t offset = (*(++code) << 8) | *(++code); \
  jint value2 = frame->pop().as_int(); \
  jint value1 = frame->pop().as_int(); \
  if (value1 operator value2) { \
    code = currentCode + offset; \
  } else { \
    ++code; \
  } \
  SAFEPOINT

#define TADD(getter) \
  ++code; \
  frame->push(frame->pop().getter() + frame->pop().getter());

#define TRETURN \
  ++code; \
  if (frame->method != 0) { \
    if (frame->method->is_synchronized()) { \
      if (frame->method->isStatic()) { \
        frame->clazz->object->ensure_monitor()->exit(); \
      } else { \
        frame->localVariables[0].as_object()->ensure_monitor()->exit(); \
      } \
    } \
  } \
  *returnValue = frame->pop(); \
  _frames.pop(); \
  return RETURN_OK;

#define TSUB(type, getter) \
  ++code; \
  type value2 = frame->pop().getter(); \
  type value1 = frame->pop().getter(); \
  frame->push(value1 - value2);

#define TMUL(getter) \
  ++code; \
  frame->push(frame->pop().getter() * frame->pop().getter());

#define TASTORE_VALUE(value) \
  ++code; \
  Value storeValue = frame->pop(); \
  jint index = frame->pop().as_int(); \
  Array* array = frame->pop().as_array(); \
  if (!array) { \
    throw_exception(CLASSNAME_NULLPOINTEREXCEPTION); \
    break; \
  } \
  array->set_value(index, value);

#define TASTORE \
  TASTORE_VALUE(storeValue)

#define TCONST_V(type, value) \
  ++code; \
  frame->push((type) value);

#define TLOAD_N(index) \
  ++code; \
  frame->push(frame->localVariables[index]);

#define TSTORE_N(index) \
  ++code; \
  frame->localVariables[index] = frame->pop();

#define TLOAD \
  frame->push(frame->localVariables[*(++code)]); \
  ++code;

#define TSTORE \
  frame->localVariables[*(++code)] = frame->pop(); \
  ++code;

// Helper for invoke-instructions.
#define FILL_INVOKE_INFO \
  uint16_t methodIndex = (*(++code) << 8) | *(++code); \
  ++code; \
  error_t errorValue = frame->clazz->get_method_from_cp(methodIndex, &invokeMethod); \
  BREAK_ON_FAIL(errorValue);

// Helpers for exception-handling.
#define RETURN_ON_UNWIND \
  if (!frame->valid) { \
    return RETURN_EXCEPTION; \
  }

#define THROW_WITH_RETURN_ON_UNWIND(exception) \
  throw_exception(exception); \
  RETURN_ON_UNWIND

#define BREAK_ON_FAIL(errorValue) \
  if (errorValue != RETURN_OK) { \
    RETURN_ON_UNWIND \
    break; \
  }

namespace coldspot
{

    error_t Interpreter::execute(Frame *frame, Value *returnValue)
    {
        // If the stack is full, throw a StackOverflowError
        if (_frames.capacity() - _frames.position() < 1024)
        {
            THROW_WITH_RETURN_ON_UNWIND(_vm->stack_overflow_error());
            return RETURN_EXCEPTION;
        }

        // Frame info
        auto &code = frame->currentCode;

        // Interpreter loop
        for (; ;)
        {
#if IS_LOG_LEVEL_DEBUG
            auto& lineMapping = frame->method->getDebugInfos()->lineMapping;
            auto begin = lineMapping.begin();
            auto end = lineMapping.end();
            uint16_t pc = 0;
            while (begin != end) {
              if (begin->key <= CURRENT_PC(frame) && begin->key > pc) {
                pc = begin->key;
              }
              ++begin;
            }
            StringBuilder builder;
            builder << frame->clazz->name << " # ";
            builder << frame->method->getSignature().name << ": ";
            builder << lineMapping.get(pc)->value;
            if (frame->method->getSignature().name == "put") {

            }
            LOG_DEBUG(builder.str().c_str())
#endif
            // Execute instruction
            switch (*code)
            {
                case AALOAD:
                {
                    ++code;
                    jint index = frame->pop().as_int();
                    Array *array = frame->pop().as_array();
                    if (array == 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }
                    Object *value;
                    error_t errorValue = array->get_value<Object *>(index,
                        &value);
                    BREAK_ON_FAIL(errorValue);
                    frame->push(value);
                    break;
                }

                case AASTORE:
                {
                    TASTORE
                    break;
                }

                case ACONST_NULL:
                {
                    ++code;
                    frame->push((Object *) 0);
                    break;
                }

                case ALOAD:
                {
                    TLOAD
                    break;
                }

                case ALOAD_0:
                {
                    TLOAD_N(0)
                    break;
                }

                case ALOAD_1:
                {
                    TLOAD_N(1)
                    break;
                }

                case ALOAD_2:
                {
                    TLOAD_N(2)
                    break;
                }

                case ALOAD_3:
                {
                    TLOAD_N(3)
                    break;
                }

                case ANEWARRAY:
                {

                    uint8_t indexByte1 = *(++code);
                    uint8_t indexByte2 = *(++code);
                    ++code;

                    jint count = frame->pop().as_int();
                    if (count < 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NEGATIVEARRAYSIZEEXCEPTION);
                        break;
                    }

                    Class *componentType;
                    uint16_t index = (indexByte1 << 8) | indexByte2;
                    error_t errorValue = frame->clazz->get_class_from_cp(index,
                        &componentType);
                    BREAK_ON_FAIL(errorValue);

                    StringBuilder builder;
                    builder << "[L" << componentType->name << ';';

                    Class *clazz;
                    errorValue = _vm->class_loader()->load_array(builder.str(),
                        frame->clazz->class_loader, &clazz);
                    BREAK_ON_FAIL(errorValue);

                    Array *array;
                    errorValue = _vm->memory_manager()->allocate_array(clazz,
                        count, &array);
                    BREAK_ON_FAIL(errorValue);

                    frame->push(array);

                    break;
                }

                case ARETURN:
                {
                    TRETURN
                    break;
                }

                case ARRAYLENGTH:
                {
                    ++code;
                    Array *array = frame->pop().as_array();
                    if (!array)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }
                    frame->push(array->length());
                    break;
                }

                case ASTORE:
                {
                    TSTORE
                    break;
                }

                case ASTORE_0:
                {
                    TSTORE_N(0)
                    break;
                }

                case ASTORE_1:
                {
                    TSTORE_N(1)
                    break;
                }

                case ASTORE_2:
                {
                    TSTORE_N(2)
                    break;
                }

                case ASTORE_3:
                {
                    TSTORE_N(3)
                    break;
                }

                case ATHROW:
                {
                    ++code;
                    THROW_WITH_RETURN_ON_UNWIND(frame->pop().as_object());
                    SAFEPOINT
                    break;
                }

                case BALOAD:
                {
                    ++code;
                    jint index = frame->pop().as_int();
                    Array *array = frame->pop().as_array();
                    if (array == 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }
                    Value value;
                    Class *clazz = array->type();
                    error_t errorValue = array->get_value(index, &value);
                    BREAK_ON_FAIL(errorValue);
                    frame->push(Value(Type::TYPE_INT, value.value()));
                    break;
                }

                case BASTORE:
                {
                    TASTORE_VALUE(storeValue.as_byte())
                    break;
                }

                case BIPUSH:
                {
                    jint value = (jbyte) * (++code);
                    ++code;
                    frame->push(value);
                    break;
                }

                case CALOAD:
                {
                    ++code;
                    jint index = frame->pop().as_int();
                    Array *array = frame->pop().as_array();
                    if (array == 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }
                    jchar value;
                    error_t errorValue = array->get_value<jchar>(index, &value);
                    BREAK_ON_FAIL(errorValue);
                    frame->push((jint) value);
                    break;
                }

                case CASTORE:
                {
                    TASTORE_VALUE(storeValue.as_char())
                    break;
                }

                case CHECKCAST:
                {
                    uint16_t index = (*(++code) << 8) | *(++code);
                    ++code;
                    Class *checkType;
                    error_t errorValue = frame->clazz->get_class_from_cp(index,
                        &checkType);
                    BREAK_ON_FAIL(errorValue);

                    Object *object = frame->peek().as_object();
                    if (object == 0)
                    {
                        break;
                    }

                    Class *objectType = object->type();
                    if (!objectType->is_castable_to(checkType))
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_CLASSCASTEXCEPTION);
                        break;
                    }
                    break;
                }

                case D2F:
                {
                    ++code;
                    frame->push((jfloat) frame->pop().as_double());
                    break;
                }

                case D2I:
                {
                    ++code;
                    frame->push((jint) frame->pop().as_double());
                    break;
                }

                case D2L:
                {
                    ++code;
                    frame->push((jlong) frame->pop().as_double());
                    break;
                }

                case DADD:
                {
                    TADD(as_double)
                    break;
                }

                case DALOAD:
                {
                    ++code;
                    jint index = frame->pop().as_int();
                    Array *array = frame->pop().as_array();
                    if (array == 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }
                    jdouble value;
                    error_t errorValue = array->get_value<jdouble>(index,
                        &value);
                    BREAK_ON_FAIL(errorValue);
                    frame->push(value);
                    break;
                }

                case DASTORE:
                {
                    TASTORE
                    break;
                }

                case DCMPG:
                case DCMPL:
                {
                    ++code;
                    jdouble value2 = frame->pop().as_double();
                    jdouble value1 = frame->pop().as_double();
                    jint result;
                    // TODO see spec (isnan)
                    if (value2 < value1)
                    {
                        result = -1;
                    }
                    else if (value1 == value2)
                    {
                        result = 0;
                    }
                    else
                    {
                        result = 1;
                    }
                    frame->push(result);
                    break;
                }

                case DCONST_0:
                {
                    TCONST_V(jdouble, 0)
                    break;
                }

                case DCONST_1:
                {
                    TCONST_V(jdouble, 1)
                    break;
                }

                case DDIV:
                {
                    ++code;
                    jdouble value2 = frame->pop().as_double();
                    jdouble value1 = frame->pop().as_double();
                    frame->push(value1 / value2);
                    break;
                }

                case DLOAD:
                {
                    TLOAD
                    break;
                }

                case DLOAD_0:
                {
                    TLOAD_N(0)
                    break;
                }

                case DLOAD_1:
                {
                    TLOAD_N(1)
                    break;
                }

                case DLOAD_2:
                {
                    TLOAD_N(2)
                    break;
                }

                case DLOAD_3:
                {
                    TLOAD_N(3)
                    break;
                }

                case DMUL:
                {
                    TMUL(as_double)
                    break;
                }

                case DNEG:
                {
                    ++code;
                    frame->push(-frame->pop().as_double());
                    break;
                }

                case DREM:
                {
                    ++code;
                    jdouble value2 = frame->pop().as_double();
                    jdouble value1 = frame->pop().as_double();
                    frame->push(fmod(value1, value2));
                    break;
                }

                case DRETURN:
                {
                    TRETURN
                    break;
                }

                case DSTORE:
                {
                    TSTORE
                    break;
                }

                case DSTORE_0:
                {
                    TSTORE_N(0)
                    break;
                }

                case DSTORE_1:
                {
                    TSTORE_N(1)
                    break;
                }

                case DSTORE_2:
                {
                    TSTORE_N(2)
                    break;
                }

                case DSTORE_3:
                {
                    TSTORE_N(3)
                    break;
                }

                case DSUB:
                {
                    TSUB(jdouble, as_double)
                    break;
                }

                case DUP:
                {
                    ++code;
                    frame->push(frame->peek());
                    break;
                }

                case DUP_X1:
                {
                    ++code;
                    Value top1 = frame->pop();
                    Value top2 = frame->pop();
                    frame->push(top1);
                    frame->push(top2);
                    frame->push(top1);
                    break;
                }

                case DUP_X2:
                {
                    ++code;
                    Value top1 = frame->pop();
                    Value top2 = frame->pop();
                    if (top2.type() == Type::TYPE_LONG ||
                        top2.type() == Type::TYPE_DOUBLE)
                    {
                        frame->push(top1);
                        frame->push(top2);
                        frame->push(top1);
                    }
                    else
                    {
                        Value top3 = frame->pop();
                        frame->push(top1);
                        frame->push(top3);
                        frame->push(top2);
                        frame->push(top1);
                    }
                    break;
                }

                case DUP2:
                {
                    ++code;
                    Value top1 = frame->pop();
                    if (top1.type() == Type::TYPE_LONG ||
                        top1.type() == Type::TYPE_DOUBLE)
                    {
                        frame->push(top1);
                        frame->push(top1);
                    }
                    else
                    {
                        Value top2 = frame->pop();
                        frame->push(top2);
                        frame->push(top1);
                        frame->push(top2);
                        frame->push(top1);
                    }
                    break;
                }

                case DUP2_X1:
                {
                    ++code;
                    Value top1 = frame->pop();
                    Value top2 = frame->pop();
                    if ((top1.type() == Type::TYPE_LONG ||
                         top1.type() == Type::TYPE_DOUBLE) &&
                        top2.type() != Type::TYPE_LONG &&
                        top2.type() != Type::TYPE_DOUBLE)
                    {
                        frame->push(top1);
                        frame->push(top2);
                        frame->push(top1);
                    }
                    else
                    {
                        Value top3 = frame->pop();
                        frame->push(top2);
                        frame->push(top1);
                        frame->push(top3);
                        frame->push(top2);
                        frame->push(top1);
                    }
                    break;
                }

                case DUP2_X2:
                {
                    ++code;
                    Value top1 = frame->pop();
                    Value top2 = frame->pop();
                    if ((top1.type() == Type::TYPE_LONG ||
                         top1.type() == Type::TYPE_DOUBLE) &&
                        (top2.type() == Type::TYPE_LONG ||
                         top2.type() == Type::TYPE_DOUBLE))
                    {
                        frame->push(top1);
                        frame->push(top2);
                        frame->push(top1);
                    }
                    else
                    {
                        Value top3 = frame->pop();
                        if ((top1.type() != Type::TYPE_LONG &&
                             top1.type() != Type::TYPE_DOUBLE) &&
                            (top2.type() != Type::TYPE_LONG &&
                             top2.type() != Type::TYPE_DOUBLE) &&
                            (top3.type() == Type::TYPE_LONG ||
                             top3.type() == Type::TYPE_DOUBLE))
                        {
                            frame->push(top2);
                            frame->push(top1);
                            frame->push(top3);
                            frame->push(top2);
                            frame->push(top1);
                        }
                        else if ((top1.type() == Type::TYPE_LONG ||
                                  top1.type() == Type::TYPE_DOUBLE) &&
                                 (top2.type() != Type::TYPE_LONG &&
                                  top2.type() != Type::TYPE_DOUBLE) &&
                                 (top3.type() != Type::TYPE_LONG &&
                                  top3.type() != Type::TYPE_DOUBLE))
                        {
                            frame->push(top1);
                            frame->push(top3);
                            frame->push(top2);
                            frame->push(top1);
                        }
                        else
                        {
                            Value top4 = frame->pop();
                            if ((top1.type() != Type::TYPE_LONG &&
                                 top1.type() != Type::TYPE_DOUBLE) &&
                                (top2.type() != Type::TYPE_LONG &&
                                 top2.type() != Type::TYPE_DOUBLE) &&
                                (top3.type() != Type::TYPE_LONG &&
                                 top3.type() != Type::TYPE_DOUBLE) &&
                                (top4.type() != Type::TYPE_LONG &&
                                 top4.type() != Type::TYPE_DOUBLE))
                            {
                                frame->push(top2);
                                frame->push(top1);
                                frame->push(top4);
                                frame->push(top3);
                                frame->push(top2);
                                frame->push(top1);
                            }
                        }
                    }
                    break;
                }

                case F2D:
                {
                    ++code;
                    frame->push((jdouble) frame->pop().as_float());
                    break;
                }

                case F2I:
                {
                    ++code;
                    frame->push((jint) frame->pop().as_float());
                    break;
                }

                case F2L:
                {
                    ++code;
                    frame->push((jlong) frame->pop().as_float());
                    break;
                }

                case FADD:
                {
                    TADD(as_float)
                    break;
                }

                case FALOAD:
                {
                    ++code;
                    jint index = frame->pop().as_int();
                    Array *array = frame->pop().as_array();
                    if (array == 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }
                    Class *clazz = array->type();
                    jfloat value;
                    error_t errorValue = array->get_value<jfloat>(index,
                        &value);
                    BREAK_ON_FAIL(errorValue);
                    frame->push(value);
                    break;
                }

                case FASTORE:
                {
                    TASTORE
                    break;
                }

                case FCMPG:
                case FCMPL:
                {
                    ++code;
                    jfloat value2 = frame->pop().as_float();
                    jfloat value1 = frame->pop().as_float();
                    jint result;
                    if (value1 > value2)
                    {
                        result = 1;
                    }
                    else if (value1 == value2)
                    {
                        result = 0;
                    }
                    else
                    {
                        result = -1;
                    }
                    frame->push(result);
                    break;
                }

                case FCONST_0:
                {
                    TCONST_V(jfloat, 0)
                    break;
                }

                case FCONST_1:
                {
                    TCONST_V(jfloat, 1)
                    break;
                }

                case FCONST_2:
                {
                    TCONST_V(jfloat, 2)
                    break;
                }

                case FDIV:
                {
                    ++code;
                    jfloat value2 = frame->pop().as_float();
                    jfloat value1 = frame->pop().as_float();
                    frame->push(value1 / value2);
                    break;
                }

                case FLOAD:
                {
                    TLOAD
                    break;
                }

                case FLOAD_0:
                {
                    TLOAD_N(0)
                    break;
                }

                case FLOAD_1:
                {
                    TLOAD_N(1)
                    break;
                }

                case FLOAD_2:
                {
                    TLOAD_N(2)
                    break;
                }

                case FLOAD_3:
                {
                    TLOAD_N(3)
                    break;
                }

                case FMUL:
                {
                    TMUL(as_float)
                    break;
                }

                case FNEG:
                {
                    ++code;
                    frame->push(-frame->pop().as_float());
                    break;
                }

                case FREM:
                {
                    ++code;
                    jfloat value2 = frame->pop().as_float();
                    jfloat value1 = frame->pop().as_float();
                    frame->push((jfloat) fmod(value1, value2));
                    break;
                }

                case FRETURN:
                {
                    TRETURN
                    break;
                }

                case FSTORE:
                {
                    TSTORE
                    break;
                }

                case FSTORE_0:
                {
                    TSTORE_N(0)
                    break;
                }

                case FSTORE_1:
                {
                    TSTORE_N(1)
                    break;
                }

                case FSTORE_2:
                {
                    TSTORE_N(2)
                    break;
                }

                case FSTORE_3:
                {
                    TSTORE_N(3)
                    break;
                }

                case FSUB:
                {
                    TSUB(jfloat, as_float)
                    break;
                }

                case GETFIELD:
                {
                    uint64_t fieldIndex = (*(++code) << 8) | *(++code);
                    ++code;
                    Field *field;
                    error_t errorValue = frame->clazz->get_field_from_cp(
                        fieldIndex, &field);
                    BREAK_ON_FAIL(errorValue);
                    if (field->is_static())
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_INCOMPATIBLECLASSCHANGEERROR);
                        break;
                    }
                    Value objectValue = frame->pop();
                    Object *object = objectValue.as_object();
                    if (!object)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }
                    frame->push(field->get(object));
                    break;
                }

                case GETSTATIC:
                {
                    uint64_t fieldIndex = (*(++code) << 8) | *(++code);
                    ++code;
                    Field *field;
                    error_t errorValue = frame->clazz->get_field_from_cp(
                        fieldIndex, &field);
                    BREAK_ON_FAIL(errorValue);
                    errorValue = _vm->class_loader()->initialize_class(
                        field->declaring_class());
                    BREAK_ON_FAIL(errorValue);
                    if (!field->is_static())
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_INCOMPATIBLECLASSCHANGEERROR);
                        break;
                    }
                    frame->push(field->get_static());
                    break;
                }

                case GOTO:
                {
                    uint8_t *currentCode = code;
                    int16_t offset = (*(++code) << 8) | *(++code);
                    code = currentCode + offset;
                    SAFEPOINT
                    break;
                }

                case GOTO_W:
                {
                    uint8_t *currentCode = code;
                    int32_t offset = (*(++code) << 24) | (*(++code) << 16) |
                                     (*(++code) << 8) | *(++code);
                    code = currentCode + offset;
                    SAFEPOINT
                    break;
                }

                case I2B:
                {
                    ++code;
                    frame->push((jint) frame->pop().as_byte());
                    break;
                }

                case I2C:
                {
                    ++code;
                    jchar value = (jchar) frame->pop().as_int();
                    frame->push((jint) value);
                    break;
                }

                case I2D:
                {
                    ++code;
                    frame->push((jdouble) frame->pop().as_int());
                    break;
                }

                case I2F:
                {
                    ++code;
                    frame->push((jfloat) frame->pop().as_int());
                    break;
                }

                case I2L:
                {
                    ++code;
                    frame->push((jlong) frame->pop().as_int());
                    break;
                }

                case I2S:
                {
                    ++code;
                    jshort value = (jshort) frame->pop().as_int();
                    frame->push((jint) value);
                    break;
                }

                case IADD:
                {
                    TADD(as_int)
                    break;
                }

                case IALOAD:
                {
                    ++code;

                    jint index = frame->pop().as_int();
                    Array *array = frame->pop().as_array();
                    if (!array)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }
                    Class *clazz = array->type();
                    jint value;
                    error_t errorValue = array->get_value<jint>(index, &value);
                    BREAK_ON_FAIL(errorValue);
                    frame->push(value);
                    break;
                }

                case IAND:
                {
                    ++code;
                    jint value2 = frame->pop().as_int();
                    jint value1 = frame->pop().as_int();
                    frame->push(value1 & value2);
                    break;
                }

                case IASTORE:
                {
                    TASTORE
                    break;
                }

                case ICONST_M1:
                {
                    TCONST_V(jint, -1)
                    break;
                }

                case ICONST_0:
                {
                    TCONST_V(jint, 0)
                    break;
                }

                case ICONST_1:
                {
                    TCONST_V(jint, 1)
                    break;
                }

                case ICONST_2:
                {
                    TCONST_V(jint, 2)
                    break;
                }

                case ICONST_3:
                {
                    TCONST_V(jint, 3)
                    break;
                }

                case ICONST_4:
                {
                    TCONST_V(jint, 4)
                    break;
                }

                case ICONST_5:
                {
                    TCONST_V(jint, 5)
                    break;
                }

                case IDIV:
                {
                    ++code;
                    jint value2 = frame->pop().as_int();
                    jint value1 = frame->pop().as_int();
                    if (value2 == 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_ARITHMETICEXCEPTION);
                        break;
                    }
                    frame->push(value1 / value2);
                    break;
                }

                case IF_ACMPEQ:
                {
                    IF_ACMP(==)
                    break;
                }

                case IF_ACMPNE:
                {
                    IF_ACMP(!=)
                    break;
                }

                case IF_ICMPEQ:
                {
                    IF_ICMP(==)
                    break;
                }

                case IF_ICMPNE:
                {
                    IF_ICMP(!=)
                    break;
                }

                case IF_ICMPLT:
                {
                    IF_ICMP(<)
                    break;
                }

                case IF_ICMPGE:
                {
                    IF_ICMP(>=)
                    break;
                }

                case IF_ICMPGT:
                {
                    IF_ICMP(>)
                    break;
                }

                case IF_ICMPLE:
                {
                    IF_ICMP(<=)
                    break;
                }

                case IFEQ:
                {
                    IF(==)
                    break;
                }

                case IFNE:
                {
                    IF(!=)
                    break;
                }

                case IFLT:
                {
                    IF(<)
                    break;
                }

                case IFGE:
                {
                    IF(>=)
                    break;
                }

                case IFGT:
                {
                    IF(>)
                    break;
                }

                case IFLE:
                {
                    IF(<=)
                    break;
                }

                case IFNONNULL:
                {
                    IFXNULL(!=)
                    break;
                }

                case IFNULL:
                {
                    IFXNULL(==)
                    break;
                }

                case IINC:
                {
                    Value &localVariable = frame->localVariables[*(++code)];
                    jint value = (jbyte) * (++code);
                    ++code;
                    localVariable.set_value(localVariable.as_int() + value);
                    break;
                }

                case ILOAD:
                {
                    TLOAD
                    break;
                }

                case ILOAD_0:
                {
                    TLOAD_N(0)
                    break;
                }

                case ILOAD_1:
                {
                    TLOAD_N(1)
                    break;
                }

                case ILOAD_2:
                {
                    TLOAD_N(2)
                    break;
                }

                case ILOAD_3:
                {
                    TLOAD_N(3)
                    break;
                }

                case IMUL:
                {
                    TMUL(as_int)
                    break;
                }

                case INEG:
                {
                    ++code;
                    frame->push(-frame->pop().as_int());
                    break;
                }

                case INSTANCEOF:
                {
                    uint16_t index = (*(++code) << 8) | *(++code);
                    ++code;

                    Class *checkClass;
                    error_t errorValue = frame->clazz->get_class_from_cp(index,
                        &checkClass);
                    BREAK_ON_FAIL(errorValue);

                    Object *object = frame->pop().as_object();
                    if (object == 0)
                    {
                        frame->push((jint) 0);
                        break;
                    }

                    Class *objectClass = object->type();
                    if (objectClass->is_castable_to(checkClass))
                    {
                        frame->push((jint) 1);
                    }
                    else
                    {
                        frame->push((jint) 0);
                    }
                    break;
                }

                case INVOKEDYNAMIC:
                {
                    EXIT_FATAL("unimplemented instruction invokedynamic");
                    // TODO implement
                    return RETURN_ERROR;
                }

                case INVOKEINTERFACE:
                {

                    Class *invokeClass;
                    Method *invokeMethod;
                    FILL_INVOKE_INFO

                    invokeClass = invokeMethod->declaring_class();

                    code += 2;

                    Object *object;

                    dynarray <Value> parameters(
                        invokeMethod->parameter_types().size());
                    errorValue = fill_parameters(frame, invokeMethod, &object,
                        parameters);
                    BREAK_ON_FAIL(errorValue);

                    errorValue = Method::lookupInterface(&invokeClass,
                        &invokeMethod, object);
                    BREAK_ON_FAIL(errorValue);

                    Value value;
                    if (invokeMethod->isNative())
                    {
                        errorValue = invokeMethod->invokeNative(object,
                            parameters, &value);
                    }
                    else
                    {
                        errorValue = invokeMethod->invokeJava(object,
                            parameters, &value);
                    }
                    BREAK_ON_FAIL(errorValue);
                    if (invokeMethod->return_type()->type != Type::TYPE_VOID)
                    {
                        frame->push(value);
                    }

                    SAFEPOINT
                    break;
                }

                case INVOKESPECIAL:
                {

                    Class *invokeClass;
                    Method *invokeMethod;
                    FILL_INVOKE_INFO

                    invokeClass = invokeMethod->declaring_class();

                    Object *object;
                    dynarray <Value> parameters(
                        invokeMethod->parameter_types().size());
                    errorValue = fill_parameters(frame, invokeMethod, &object,
                        parameters);
                    BREAK_ON_FAIL(errorValue);

                    errorValue = Method::lookupSpecial(&invokeClass,
                        &invokeMethod);
                    BREAK_ON_FAIL(errorValue);

                    Value value;
                    if (invokeMethod->isNative())
                    {
                        errorValue = invokeMethod->invokeNative(object,
                            parameters, &value);
                    }
                    else
                    {
                        errorValue = invokeMethod->invokeJava(object,
                            parameters, &value);
                    }
                    BREAK_ON_FAIL(errorValue);
                    if (invokeMethod->return_type()->type != Type::TYPE_VOID)
                    {
                        frame->push(value);
                    }

                    SAFEPOINT
                    break;
                }

                case INVOKESTATIC:
                {

                    Class *invokeClass;
                    Method *invokeMethod;
                    FILL_INVOKE_INFO

                    invokeClass = invokeMethod->declaring_class();
                    errorValue = _vm->class_loader()->initialize_class(
                        invokeClass);
                    BREAK_ON_FAIL(errorValue);

                    Object *object;
                    dynarray <Value> parameters(
                        invokeMethod->parameter_types().size());
                    errorValue = fill_parameters(frame, invokeMethod, &object,
                        parameters);
                    BREAK_ON_FAIL(errorValue);

                    Value value;
                    if (invokeMethod->isNative())
                    {
                        errorValue = invokeMethod->invokeNative(object,
                            parameters, &value);
                    }
                    else
                    {
                        errorValue = invokeMethod->invokeJava(object,
                            parameters, &value);
                    }
                    BREAK_ON_FAIL(errorValue);

                    if (invokeMethod->return_type()->type != Type::TYPE_VOID)
                    {
                        frame->push(value);
                    }

                    SAFEPOINT
                    break;
                }

                case INVOKEVIRTUAL:
                {

                    Class *invokeClass;
                    Method *invokeMethod;
                    FILL_INVOKE_INFO

                    invokeClass = invokeMethod->declaring_class();

                    Object *object;
                    dynarray <Value> parameters(
                        invokeMethod->parameter_types().size());
                    errorValue = fill_parameters(frame, invokeMethod, &object,
                        parameters);
                    BREAK_ON_FAIL(errorValue);

                    errorValue = Method::lookupVirtual(&invokeClass,
                        &invokeMethod, object);
                    BREAK_ON_FAIL(errorValue);

                    Value value;
                    if (invokeMethod->isNative())
                    {
                        errorValue = invokeMethod->invokeNative(object,
                            parameters, &value);
                    }
                    else
                    {
                        errorValue = invokeMethod->invokeJava(object,
                            parameters, &value);
                    }
                    BREAK_ON_FAIL(errorValue);

                    if (invokeMethod->return_type()->type != Type::TYPE_VOID)
                    {
                        frame->push(value);
                    }

                    SAFEPOINT
                    break;
                }

                case IOR:
                {
                    ++code;
                    jint value2 = frame->pop().as_int();
                    jint value1 = frame->pop().as_int();
                    frame->push(value1 | value2);
                    break;
                }

                case IREM:
                {
                    ++code;
                    jint value2 = frame->pop().as_int();
                    jint value1 = frame->pop().as_int();
                    if (value2 == 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_ARITHMETICEXCEPTION);
                        break;
                    }
                    frame->push(value1 % value2);
                    break;
                }

                case IRETURN:
                {
                    TRETURN
                    break;
                }

                case ISHL:
                {
                    ++code;
                    jint value2 = frame->pop().as_int();
                    jint value1 = frame->pop().as_int();
                    frame->push(value1 << value2);
                    break;
                }

                case ISHR:
                {
                    ++code;
                    jint value2 = frame->pop().as_int();
                    jint value1 = frame->pop().as_int();
                    frame->push(value1 >> value2);
                    break;
                }

                case ISTORE:
                {
                    TSTORE
                    break;
                }

                case ISTORE_0:
                {
                    TSTORE_N(0)
                    break;
                }

                case ISTORE_1:
                {
                    TSTORE_N(1)
                    break;
                }

                case ISTORE_2:
                {
                    TSTORE_N(2)
                    break;
                }

                case ISTORE_3:
                {
                    TSTORE_N(3)
                    break;
                }

                case ISUB:
                {
                    TSUB(jint, as_int)
                    break;
                }

                case IUSHR:
                {
                    ++code;
                    jint value2 = frame->pop().as_int();
                    jint value1 = frame->pop().as_int();
                    frame->push((jint)(((uint32_t) value1) >> value2));
                    break;
                }

                case IXOR:
                {
                    ++code;
                    jint value2 = frame->pop().as_int();
                    jint value1 = frame->pop().as_int();
                    jint result = value1 ^value2;
                    frame->push(result);
                    break;
                }

                case JSR:
                {
                    uint8_t *currentCode = code;
                    uint8_t index1 = *(++code);
                    uint8_t index2 = *(++code);
                    ++code;
                    int16_t offset = (index1 << 8) | index2;
                    frame->push(
                        Value(Type::TYPE_RETURNADDRESS, CURRENT_PC(frame)));
                    code = currentCode + offset;
                    SAFEPOINT
                    break;
                }

                case JSR_W:
                {
                    uint8_t *currentCode = code;
                    int32_t offset = (*(++code) << 24) | (*(++code) << 16) |
                                     (*(++code) << 8) | *(++code);
                    ++code;
                    frame->push(
                        Value(Type::TYPE_RETURNADDRESS, CURRENT_PC(frame)));
                    code = currentCode + offset;
                    SAFEPOINT
                    break;
                }

                case L2D:
                {
                    ++code;
                    frame->push((double) frame->pop().as_long());
                    break;
                }

                case L2F:
                {
                    ++code;
                    frame->push((float) frame->pop().as_long());
                    break;
                }

                case L2I:
                {
                    ++code;
                    frame->push((jint) frame->pop().as_long());
                    break;
                }

                case LADD:
                {
                    TADD(as_long)
                    break;
                }

                case LALOAD:
                {
                    ++code;
                    jint index = frame->pop().as_int();
                    Array *array = frame->pop().as_array();
                    if (!array)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }
                    Class *clazz = array->type();
                    jlong value;
                    error_t errorValue = array->get_value<jlong>(index, &value);
                    BREAK_ON_FAIL(errorValue);
                    frame->push(value);
                    break;
                }

                case LAND:
                {
                    ++code;
                    jlong value2 = frame->pop().as_long();
                    jlong value1 = frame->pop().as_long();
                    frame->push(value1 & value2);
                    break;
                }

                case LASTORE:
                {
                    TASTORE
                    break;
                }

                case LCMP:
                {
                    ++code;
                    jlong value2 = frame->pop().as_long();
                    jlong value1 = frame->pop().as_long();
                    jint result;
                    if (value1 > value2)
                    {
                        result = 1;
                    }
                    else if (value1 == value2)
                    {
                        result = 0;
                    }
                    else
                    {
                        result = -1;
                    }
                    frame->push(result);
                    break;
                }

                case LCONST_0:
                {
                    TCONST_V(jlong, 0)
                    break;
                }

                case LCONST_1:
                {
                    TCONST_V(jlong, 1)
                    break;
                }

                case LDC:
                {
                    uint8_t index = *(++code);
                    ++code;

                    ConstantPoolEntry *entry = frame->clazz->class_file->constantPool[index];
                    switch (entry->tag)
                    {
                        case CP_INTEGER:
                        {
                            frame->push(
                                frame->clazz->get_integer_from_cp(index));
                            break;
                        }
                        case CP_FLOAT:
                        {
                            frame->push(frame->clazz->get_float_from_cp(index));
                            break;
                        }
                        case CP_STRING:
                        {
                            Object *string;
                            error_t errorValue = frame->clazz->get_string_from_cp(
                                index, &string);
                            BREAK_ON_FAIL(errorValue);
                            frame->push(string);
                            break;
                        }
                        case CP_CLASS:
                        {
                            Class *clazz;
                            error_t errorValue = frame->clazz->get_class_from_cp(
                                index, &clazz);
                            BREAK_ON_FAIL(errorValue);
                            frame->push(clazz->object);
                            break;
                        }
                        default:
                        {
                            EXIT_FATAL(
                                "unimplemented constant-pool-entry in ldc");
                        }
                    }
                    break;
                }

                case LDC_W:
                {
                    uint16_t index = (*(++code) << 8) | *(++code);
                    ++code;

                    ConstantPoolEntry *entry = frame->clazz->class_file->constantPool[index];
                    switch (entry->tag)
                    {
                        case CP_INTEGER:
                        {
                            frame->push(
                                frame->clazz->get_integer_from_cp(index));
                            break;
                        }
                        case CP_FLOAT:
                        {
                            frame->push(frame->clazz->get_float_from_cp(index));
                            break;
                        }
                        case CP_STRING:
                        {
                            Object *string;
                            error_t errorValue = frame->clazz->get_string_from_cp(
                                index, &string);
                            BREAK_ON_FAIL(errorValue);
                            frame->push(string);
                            break;
                        }
                        case CP_CLASS:
                        {
                            Class *clazz;
                            error_t errorValue = frame->clazz->get_class_from_cp(
                                index, &clazz);
                            BREAK_ON_FAIL(errorValue);
                            frame->push(clazz->object);
                            break;
                        }
                        default:
                        {
                            EXIT_FATAL(
                                "unimplemented constant-pool-entry in ldc_w");
                        }
                    }
                    break;
                }

                case LDC2_W:
                {
                    uint16_t index = (*(++code) << 8) | *(++code);
                    ++code;

                    ConstantPoolEntry *entry = frame->clazz->class_file->constantPool[index];
                    switch (entry->tag)
                    {
                        case CP_LONG:
                        {
                            frame->push(frame->clazz->get_long_from_cp(index));
                            break;
                        }
                        case CP_DOUBLE:
                        {
                            frame->push(
                                frame->clazz->get_double_from_cp(index));
                            break;
                        }
                        default:
                        {
                            EXIT_FATAL(
                                "unimplemented constant-pool-entry in ldc2_w");
                        }
                    }
                    break;
                }

                case LDIV:
                {
                    ++code;
                    jlong value2 = frame->pop().as_long();
                    jlong value1 = frame->pop().as_long();
                    if (value2 == 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_ARITHMETICEXCEPTION);
                        break;
                    }
                    frame->push(value1 / value2);
                    break;
                }

                case LLOAD:
                {
                    TLOAD
                    break;
                }

                case LLOAD_0:
                {
                    TLOAD_N(0)
                    break;
                }

                case LLOAD_1:
                {
                    TLOAD_N(1)
                    break;
                }

                case LLOAD_2:
                {
                    TLOAD_N(2)
                    break;
                }

                case LLOAD_3:
                {
                    TLOAD_N(3)
                    break;
                }

                case LMUL:
                {
                    TMUL(as_long)
                    break;
                }

                case LNEG:
                {
                    ++code;
                    frame->push(-frame->pop().as_long());
                    break;
                }

                case LOOKUPSWITCH:
                {
                    uint32_t beginProgramCounter = CURRENT_PC(frame);
                    ++code;

                    uint32_t programCounter = CURRENT_PC(frame);
                    programCounter = (programCounter + 3) & ~0x03;
                    code = &frame->method->code()[programCounter];

                    int32_t defaultValue =
                        (*code << 24) | (*(++code) << 16) | (*(++code) << 8) |
                        *(++code);

                    int32_t pairCount = (*(++code) << 24) | (*(++code) << 16) |
                                        (*(++code) << 8) | *(++code);

                    jint keyValue = frame->pop().as_int();

                    int32_t offsetValue = defaultValue;

                    for (int32_t i = 0; i < pairCount; ++i)
                    {
                        int32_t matchValue =
                            (*(++code) << 24) | (*(++code) << 16) |
                            (*(++code) << 8) | *(++code);

                        if (matchValue == keyValue)
                        {
                            offsetValue =
                                (*(++code) << 24) | (*(++code) << 16) |
                                (*(++code) << 8) | *(++code);
                            break;
                        }
                        else
                        {
                            code += 4;
                        }
                    }

                    code = &frame->method->code()[beginProgramCounter +
                                                  offsetValue];

                    break;
                }

                case LOR:
                {
                    ++code;
                    jlong value2 = frame->pop().as_long();
                    jlong value1 = frame->pop().as_long();
                    frame->push(value1 | value2);
                    break;
                }

                case LREM:
                {
                    ++code;
                    jlong value2 = frame->pop().as_long();
                    jlong value1 = frame->pop().as_long();
                    if (value2 == 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_ARITHMETICEXCEPTION);
                        break;
                    }
                    frame->push(value1 % value2);
                    break;
                }

                case LRETURN:
                {
                    TRETURN
                    break;
                }

                case LSHL:
                {
                    ++code;
                    jlong value2 = frame->pop().as_long();
                    jlong value1 = frame->pop().as_long();
                    frame->push(value1 << value2);
                    break;
                }

                case LSHR:
                {
                    ++code;
                    jlong value2 = frame->pop().as_long();
                    jlong value1 = frame->pop().as_long();
                    frame->push(value1 >> value2);
                    break;
                }

                case LSTORE:
                {
                    TSTORE
                    break;
                }

                case LSTORE_0:
                {
                    TSTORE_N(0)
                    break;
                }

                case LSTORE_1:
                {
                    TSTORE_N(1)
                    break;
                }

                case LSTORE_2:
                {
                    TSTORE_N(2)
                    break;
                }

                case LSTORE_3:
                {
                    TSTORE_N(3)
                    break;
                }

                case LSUB:
                {
                    TSUB(jlong, as_long)
                    break;
                }

                case LUSHR:
                {
                    ++code;
                    jlong value2 = frame->pop().as_long();
                    jlong value1 = frame->pop().as_long();
                    frame->push((jlong)(((uint64_t) value1) >> value2));
                    break;
                }

                case LXOR:
                {
                    ++code;
                    jlong value2 = frame->pop().as_long();
                    jlong value1 = frame->pop().as_long();
                    frame->push(value1 ^ value2);
                    break;
                }

                case MONITORENTER:
                {
                    ++code;
                    Object *object = frame->pop().as_object();
                    if (!object)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }
                    object->ensure_monitor()->enter();
                    break;
                }

                case MONITOREXIT:
                {
                    ++code;
                    Object *object = frame->pop().as_object();
                    if (!object)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }
                    object->ensure_monitor()->exit();
                    break;
                }

                case MULTIANEWARRAY:
                {
                    uint8_t index1 = *(++code);
                    uint8_t index2 = *(++code);
                    uint8_t dimensions = *(++code);
                    ++code;
                    fixed_stack <jint> sizes;
                    sizes.init(dimensions);
                    for (uint8_t i = 0; i < dimensions; ++i)
                    {
                        jint size = frame->pop().as_int();
                        if (size < 0)
                        {
                            THROW_WITH_RETURN_ON_UNWIND(
                                CLASSNAME_NEGATIVEARRAYSIZEEXCEPTION);
                            break;
                        }
                        sizes.push(size);
                    }
                    uint64_t classIndex = (index1 << 8) | index2;
                    Class *clazz;
                    error_t errorValue = frame->clazz->get_class_from_cp(
                        classIndex, &clazz);
                    BREAK_ON_FAIL(errorValue);
                    Array *array;
                    errorValue = createMultiArray(clazz, sizes,
                        sizes.size() - 1, dimensions, &array);
                    BREAK_ON_FAIL(errorValue);
                    frame->push(array);
                    break;
                }

                case NEW:
                {
                    uint16_t classIndex = (*(++code) << 8) | *(++code);
                    ++code;

                    Class *clazz;
                    error_t errorValue = frame->clazz->get_class_from_cp(
                        classIndex, &clazz);
                    BREAK_ON_FAIL(errorValue);

                    if (clazz->is_abstract())
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_INSTANTIATIONERROR);
                        break;
                    }

                    errorValue = _vm->class_loader()->initialize_class(clazz);
                    BREAK_ON_FAIL(errorValue);

                    Object *object;
                    errorValue = _vm->memory_manager()->allocate_object(clazz,
                        &object);
                    BREAK_ON_FAIL(errorValue);

                    frame->push(object);
                    break;
                }

                case NEWARRAY:
                {
                    uint8_t type = *(++code);
                    ++code;
                    Value countValue = frame->pop();
                    jint count = countValue.as_int();
                    if (count < 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NEGATIVEARRAYSIZEEXCEPTION);
                        break;
                    }
                    const char *name;
                    switch (type)
                    {
                        case 4:
                            name = "[Z";
                            break;
                        case 5:
                            name = "[C";
                            break;
                        case 6:
                            name = "[F";
                            break;
                        case 7:
                            name = "[D";
                            break;
                        case 8:
                            name = "[B";
                            break;
                        case 9:
                            name = "[S";
                            break;
                        case 10:
                            name = "[I";
                            break;
                        case 11:
                            name = "[J";
                            break;
                    }
                    Class *clazz;
                    error_t errorValue = _vm->class_loader()->load_array(name,
                        0, &clazz);
                    BREAK_ON_FAIL(errorValue);
                    Array *array;
                    errorValue = _vm->memory_manager()->allocate_array(clazz,
                        countValue.as_int(), &array);
                    BREAK_ON_FAIL(errorValue);
                    frame->push(array);
                    break;
                }

                case NOP:
                {
                    ++code;
                    break;
                }

                case POP:
                {
                    ++code;
                    frame->pop();
                    break;
                }

                case POP2:
                {
                    ++code;
                    Value value = frame->pop();
                    if (value.type() != Type::TYPE_LONG &&
                        value.type() != Type::TYPE_DOUBLE)
                    {
                        frame->pop();
                    }
                    break;
                }

                case PUTFIELD:
                {

                    uint16_t fieldIndex = (*(++code) << 8) | *(++code);
                    ++code;

                    Field *field;
                    error_t errorValue = frame->clazz->get_field_from_cp(
                        fieldIndex, &field);
                    BREAK_ON_FAIL(errorValue);

                    if (field->is_static())
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_INCOMPATIBLECLASSCHANGEERROR);
                        break;
                    }

                    Value value = frame->pop();
                    Object *object = frame->pop().as_object();
                    if (object == 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }

                    field->set(object, value);
                    break;
                }

                case PUTSTATIC:
                {

                    uint64_t fieldIndex = (*(++code) << 8) | *(++code);
                    ++code;

                    Field *field;
                    error_t errorValue = frame->clazz->get_field_from_cp(
                        fieldIndex, &field);
                    BREAK_ON_FAIL(errorValue);

                    errorValue = _vm->class_loader()->initialize_class(
                        field->declaring_class());
                    BREAK_ON_FAIL(errorValue);

                    if (!field->is_static())
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_INCOMPATIBLECLASSCHANGEERROR);
                        break;
                    }

                    field->set_static(frame->pop());
                    break;
                }

                case RET:
                {
                    uint32_t programCounter = frame->localVariables[*(++code)].as_return_addr();
                    code = &frame->method->code()[programCounter];
                    break;
                }

                case RETURN:
                {
                    if (frame->method->is_synchronized())
                    {
                        frame->callee()->ensure_monitor()->exit();
                    }

                    _frames.pop();
                    return RETURN_OK;
                }

                case SALOAD:
                {
                    ++code;

                    jint index = frame->pop().as_int();
                    Array *array = frame->pop().as_array();
                    if (array == 0)
                    {
                        THROW_WITH_RETURN_ON_UNWIND(
                            CLASSNAME_NULLPOINTEREXCEPTION);
                        break;
                    }

                    Value value;
                    Class *clazz = array->type();
                    error_t errorValue = array->get_value(index, &value);
                    BREAK_ON_FAIL(errorValue);

                    frame->push(value.as_int());
                    break;
                }

                case SASTORE:
                {
                    TASTORE // TODO truncate int
                    break;
                }

                case SIPUSH:
                {
                    jshort value = (*(++code) << 8) | *(++code);
                    ++code;
                    frame->push((jint) value);
                    break;
                }

                case SWAP:
                {
                    ++code;
                    Value value1 = frame->pop();
                    Value value2 = frame->pop();
                    frame->push(value1);
                    frame->push(value2);
                    break;
                }

                case TABLESWITCH:
                {
                    uint32_t beginProgramCounter = CURRENT_PC(frame);
                    ++code;

                    uint32_t programCounter = CURRENT_PC(frame);
                    programCounter = (programCounter + 3) & ~0x03;
                    code = &frame->method->code()[programCounter];

                    int32_t defaultValue =
                        (*code << 24) | (*(++code) << 16) | (*(++code) << 8) |
                        *(++code);

                    int32_t lowValue = (*(++code) << 24) | (*(++code) << 16) |
                                       (*(++code) << 8) | *(++code);

                    int32_t highValue = (*(++code) << 24) | (*(++code) << 16) |
                                        (*(++code) << 8) | *(++code);

                    jint index = frame->pop().as_int();

                    if (index < lowValue || index > highValue)
                    {
                        programCounter = beginProgramCounter + defaultValue;
                    }
                    else
                    {
                        ++code;
                        int32_t offset = (index - lowValue) * 4;
                        uint8_t jumpByte1 = *(code + offset);
                        uint8_t jumpByte2 = *(code + offset + 1);
                        uint8_t jumpByte3 = *(code + offset + 2);
                        uint8_t jumpByte4 = *(code + offset + 3);
                        int32_t jumpValue =
                            (jumpByte1 << 24) | (jumpByte2 << 16) |
                            (jumpByte3 << 8) | jumpByte4;
                        programCounter = beginProgramCounter + jumpValue;
                    }

                    code = &frame->method->code()[programCounter];

                    break;
                }

                case WIDE:
                {
                    EXIT_FATAL("unimplemented instruction wide");
                }

                default:
                {
                    EXIT_FATAL("unimplemented instruction");
                }
            }
        }

        // Should never be reached, because every frame should complete either:
        // - normally (frame returns with one of the return-instructions) or
        // - abruptly (frame was unwinded during exception-handling)
        EXIT_FATAL("interpreter returns catastrophically");

        return RETURN_ERROR;
    }


    error_t Interpreter::fill_parameters(Frame *frame, Method *method,
        Object **object, Value *parameters)
    {
        size_t parameterSize = method->parameter_types().size();
        while (parameterSize != 0)
        {
            parameters[--parameterSize] = frame->pop();
        }

        if (!method->isStatic())
        {
            Value objectValue = frame->pop();
            if (objectValue.value() == 0)
            {
                throw_exception(CLASSNAME_NULLPOINTEREXCEPTION);
                return RETURN_EXCEPTION;
            }
            *object = objectValue.as_object();
        }
        else if (method->isNative())
        {
            *object = method->declaring_class()->object;
        }
        else
        {
            *object = 0;
        }

        return RETURN_OK;
    }

}

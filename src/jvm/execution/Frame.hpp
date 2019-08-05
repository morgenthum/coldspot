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

#ifndef COLDSPOT_JVM_EXECUTION_FRAME_HPP_
#define COLDSPOT_JVM_EXECUTION_FRAME_HPP_

#include <cstdint>

#include <jvm/common/List.hpp>
#include <jvm/jdk/Global.hpp>
#include <jvm/Value.hpp>

#define KB                  1024
#define MB                  (KB * KB)

#define JAVA_STACK_SIZE     (256 * KB)
#define NATIVE_STACK_SIZE   MB

#define CURRENT_PC(frame) \
  ((uint32_t) (frame->currentCode - frame->method->code()))

namespace coldspot
{

    class Class;
    class Object;
    class Method;

    // A frame can either be executed by the jvm or native.
    enum FrameType
    {
        FRAMETYPE_JAVA, FRAMETYPE_NATIVE
    };

    // Every executing method is represented by a frame on the stack.
    // It holds all relevant information that are important for the execution.
    class Frame
    {
    public:

        // Returns the size of a frame for the given method.
        static uint32_t size(Method *method)
        {
            // The size of the frame
            uint32_t size = sizeof(Frame);

            // Plus the size of all local-variables and operands
            size += sizeof(Value) * method->locals_count();
            size += sizeof(Value) * method->operands_count();

            return size;
        }

        // Creates a frame in a pre-allocated memory-block.
        static Frame *create(FrameType type, Class *clazz, Method *method,
            uint8_t *memory)
        {
            // Create frame in the memory-block.
            Frame *frame = new(memory) Frame(type, clazz, method,
                method->code());

            // Local-variables are located after the frame
            frame->localVariables = (Value * )(memory + sizeof(Frame));

            // Operands are located after the local-variables
            uint32_t localVariablesSize =
                sizeof(Value) * method->locals_count();
            frame->operands = (Value * )(
                memory + sizeof(Frame) + localVariablesSize);

            return frame;
        }

        // Type of the frame
        FrameType type;

        // General info
        Class *clazz;
        Method *method;

        // Value storage
        Value *localVariables;
        Value *operands;
        uint16_t operandsCount;

        // Current instruction
        uint8_t *currentCode;

        // Local references of the frame
        List <jobject> *localReferences;

        // Pending exception
        Object *exception;

        // Used for stack-unwinding
        bool valid;

        Frame(FrameType type, Class *clazz, Method *method, uint8_t *code)
            : type(type), clazz(clazz), method(method), localVariables(0),
              operands(0), operandsCount(0), currentCode(code),
              localReferences(0), exception(0), valid(true) { }

        Frame(const Frame &other) = delete;
        Frame &operator=(const Frame &other) = delete;

        void push(const Value &value) { operands[operandsCount++] = value; }
        Value &peek() { return operands[operandsCount - 1]; }
        Value pop() { return operands[--operandsCount]; }

        Object *callee() const
        {
            return method->isStatic() ? clazz->object
                                      : localVariables[0].as_object();
        }
    };

}

#endif

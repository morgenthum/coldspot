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

#ifndef COLDSPOT_JVM_EXECUTION_INSTRUCTIONS_HPP_
#define COLDSPOT_JVM_EXECUTION_INSTRUCTIONS_HPP_

namespace coldspot
{

    const uint8_t NOP = 0;
    const uint8_t ACONST_NULL = 1;
    const uint8_t ICONST_M1 = 2;
    const uint8_t ICONST_0 = 3;
    const uint8_t ICONST_1 = 4;
    const uint8_t ICONST_2 = 5;
    const uint8_t ICONST_3 = 6;
    const uint8_t ICONST_4 = 7;
    const uint8_t ICONST_5 = 8;
    const uint8_t LCONST_0 = 9;
    const uint8_t LCONST_1 = 10;
    const uint8_t FCONST_0 = 11;
    const uint8_t FCONST_1 = 12;
    const uint8_t FCONST_2 = 13;
    const uint8_t DCONST_0 = 14;
    const uint8_t DCONST_1 = 15;
    const uint8_t BIPUSH = 16;
    const uint8_t SIPUSH = 17;
    const uint8_t LDC = 18;
    const uint8_t LDC_W = 19;
    const uint8_t LDC2_W = 20;
    const uint8_t ILOAD = 21;
    const uint8_t LLOAD = 22;
    const uint8_t FLOAD = 23;
    const uint8_t DLOAD = 24;
    const uint8_t ALOAD = 25;
    const uint8_t ILOAD_0 = 26;
    const uint8_t ILOAD_1 = 27;
    const uint8_t ILOAD_2 = 28;
    const uint8_t ILOAD_3 = 29;
    const uint8_t LLOAD_0 = 30;
    const uint8_t LLOAD_1 = 31;
    const uint8_t LLOAD_2 = 32;
    const uint8_t LLOAD_3 = 33;
    const uint8_t FLOAD_0 = 34;
    const uint8_t FLOAD_1 = 35;
    const uint8_t FLOAD_2 = 36;
    const uint8_t FLOAD_3 = 37;
    const uint8_t DLOAD_0 = 38;
    const uint8_t DLOAD_1 = 39;
    const uint8_t DLOAD_2 = 40;
    const uint8_t DLOAD_3 = 41;
    const uint8_t ALOAD_0 = 42;
    const uint8_t ALOAD_1 = 43;
    const uint8_t ALOAD_2 = 44;
    const uint8_t ALOAD_3 = 45;
    const uint8_t IALOAD = 46;
    const uint8_t LALOAD = 47;
    const uint8_t FALOAD = 48;
    const uint8_t DALOAD = 49;
    const uint8_t AALOAD = 50;
    const uint8_t BALOAD = 51;
    const uint8_t CALOAD = 52;
    const uint8_t SALOAD = 53;
    const uint8_t ISTORE = 54;
    const uint8_t LSTORE = 55;
    const uint8_t FSTORE = 56;
    const uint8_t DSTORE = 57;
    const uint8_t ASTORE = 58;
    const uint8_t ISTORE_0 = 59;
    const uint8_t ISTORE_1 = 60;
    const uint8_t ISTORE_2 = 61;
    const uint8_t ISTORE_3 = 62;
    const uint8_t LSTORE_0 = 63;
    const uint8_t LSTORE_1 = 64;
    const uint8_t LSTORE_2 = 65;
    const uint8_t LSTORE_3 = 66;
    const uint8_t FSTORE_0 = 67;
    const uint8_t FSTORE_1 = 68;
    const uint8_t FSTORE_2 = 69;
    const uint8_t FSTORE_3 = 70;
    const uint8_t DSTORE_0 = 71;
    const uint8_t DSTORE_1 = 72;
    const uint8_t DSTORE_2 = 73;
    const uint8_t DSTORE_3 = 74;
    const uint8_t ASTORE_0 = 75;
    const uint8_t ASTORE_1 = 76;
    const uint8_t ASTORE_2 = 77;
    const uint8_t ASTORE_3 = 78;
    const uint8_t IASTORE = 79;
    const uint8_t LASTORE = 80;
    const uint8_t FASTORE = 81;
    const uint8_t DASTORE = 82;
    const uint8_t AASTORE = 83;
    const uint8_t BASTORE = 84;
    const uint8_t CASTORE = 85;
    const uint8_t SASTORE = 86;
    const uint8_t POP = 87;
    const uint8_t POP2 = 88;
    const uint8_t DUP = 89;
    const uint8_t DUP_X1 = 90;
    const uint8_t DUP_X2 = 91;
    const uint8_t DUP2 = 92;
    const uint8_t DUP2_X1 = 93;
    const uint8_t DUP2_X2 = 94;
    const uint8_t SWAP = 95;
    const uint8_t IADD = 96;
    const uint8_t LADD = 97;
    const uint8_t FADD = 98;
    const uint8_t DADD = 99;
    const uint8_t ISUB = 100;
    const uint8_t LSUB = 101;
    const uint8_t FSUB = 102;
    const uint8_t DSUB = 103;
    const uint8_t IMUL = 104;
    const uint8_t LMUL = 105;
    const uint8_t FMUL = 106;
    const uint8_t DMUL = 107;
    const uint8_t IDIV = 108;
    const uint8_t LDIV = 109;
    const uint8_t FDIV = 110;
    const uint8_t DDIV = 111;
    const uint8_t IREM = 112;
    const uint8_t LREM = 113;
    const uint8_t FREM = 114;
    const uint8_t DREM = 115;
    const uint8_t INEG = 116;
    const uint8_t LNEG = 117;
    const uint8_t FNEG = 118;
    const uint8_t DNEG = 119;
    const uint8_t ISHL = 120;
    const uint8_t LSHL = 121;
    const uint8_t ISHR = 122;
    const uint8_t LSHR = 123;
    const uint8_t IUSHR = 124;
    const uint8_t LUSHR = 125;
    const uint8_t IAND = 126;
    const uint8_t LAND = 127;
    const uint8_t IOR = 128;
    const uint8_t LOR = 129;
    const uint8_t IXOR = 130;
    const uint8_t LXOR = 131;
    const uint8_t IINC = 132;
    const uint8_t I2L = 133;
    const uint8_t I2F = 134;
    const uint8_t I2D = 135;
    const uint8_t L2I = 136;
    const uint8_t L2F = 137;
    const uint8_t L2D = 138;
    const uint8_t F2I = 139;
    const uint8_t F2L = 140;
    const uint8_t F2D = 141;
    const uint8_t D2I = 142;
    const uint8_t D2L = 143;
    const uint8_t D2F = 144;
    const uint8_t I2B = 145;
    const uint8_t I2C = 146;
    const uint8_t I2S = 147;
    const uint8_t LCMP = 148;
    const uint8_t FCMPL = 149;
    const uint8_t FCMPG = 150;
    const uint8_t DCMPL = 151;
    const uint8_t DCMPG = 152;
    const uint8_t IFEQ = 153;
    const uint8_t IFNE = 154;
    const uint8_t IFLT = 155;
    const uint8_t IFGE = 156;
    const uint8_t IFGT = 157;
    const uint8_t IFLE = 158;
    const uint8_t IF_ICMPEQ = 159;
    const uint8_t IF_ICMPNE = 160;
    const uint8_t IF_ICMPLT = 161;
    const uint8_t IF_ICMPGE = 162;
    const uint8_t IF_ICMPGT = 163;
    const uint8_t IF_ICMPLE = 164;
    const uint8_t IF_ACMPEQ = 165;
    const uint8_t IF_ACMPNE = 166;
    const uint8_t GOTO = 167;
    const uint8_t JSR = 168;
    const uint8_t RET = 169;
    const uint8_t TABLESWITCH = 170;
    const uint8_t LOOKUPSWITCH = 171;
    const uint8_t IRETURN = 172;
    const uint8_t LRETURN = 173;
    const uint8_t FRETURN = 174;
    const uint8_t DRETURN = 175;
    const uint8_t ARETURN = 176;
    const uint8_t RETURN = 177;
    const uint8_t GETSTATIC = 178;
    const uint8_t PUTSTATIC = 179;
    const uint8_t GETFIELD = 180;
    const uint8_t PUTFIELD = 181;
    const uint8_t INVOKEVIRTUAL = 182;
    const uint8_t INVOKESPECIAL = 183;
    const uint8_t INVOKESTATIC = 184;
    const uint8_t INVOKEINTERFACE = 185;
    const uint8_t INVOKEDYNAMIC = 186;
    const uint8_t NEW = 187;
    const uint8_t NEWARRAY = 188;
    const uint8_t ANEWARRAY = 189;
    const uint8_t ARRAYLENGTH = 190;
    const uint8_t ATHROW = 191;
    const uint8_t CHECKCAST = 192;
    const uint8_t INSTANCEOF = 193;
    const uint8_t MONITORENTER = 194;
    const uint8_t MONITOREXIT = 195;
    const uint8_t WIDE = 196;
    const uint8_t MULTIANEWARRAY = 197;
    const uint8_t IFNULL = 198;
    const uint8_t IFNONNULL = 199;
    const uint8_t GOTO_W = 200;
    const uint8_t JSR_W = 201;

}

#endif

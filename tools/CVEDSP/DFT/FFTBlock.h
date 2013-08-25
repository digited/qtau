/*
 * FFTBlock.h
 * Copyright (C) 2013 Sleepwalking
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Sleepwalking'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 
 * CVEDSP IS PROVIDED BY Sleepwalking ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Sleepwalking OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef FFTBLOCK_H
#define FFTBLOCK_H
#include "FFTCommon.h"

#ifdef ROE
//2^12 = 4096, half = 2048, quarter = 1024
#define FFTBlock12(Re, Im) \
			ComplexCopy(_FRe, _FIm, Re, Im, 2048);\
			StaticFFTCodeTable[8]();\
			ComplexCopy(Re, Im, _FRe, _FIm, 2048);\
			ComplexCopy(_FRe, _FIm, Re + 2048, Im + 2048, 1024);\
			StaticFFTCodeTable[7]();\
			ComplexCopy(Re + 2048, Im + 2048, _FRe, _FIm, 1024);\
			ComplexCopy(_FRe, _FIm, Re + 3072, Im + 3072, 1024);\
			StaticFFTCodeTable[7]();\
			ComplexCopy(Re + 3072, Im + 3072, _FRe, _FIm, 1024);\
			SplitRadix_RadixL(Re, Im, 0, 4096);

//2^13 = 8192, half = 4096, quarter = 2048
#define FFTBlock13(Re, Im) \
			FFTBlock12(Re, Im)\
			ComplexCopy(_FRe, _FIm, Re + 4096, Im + 4096, 2048);\
			StaticFFTCodeTable[8]();\
			ComplexCopy(Re + 4096, Im + 4096, _FRe, _FIm, 2048);\
			ComplexCopy(_FRe, _FIm, Re + 6144, Im + 6144, 2048);\
			StaticFFTCodeTable[8]();\
			ComplexCopy(Re + 6144, Im + 6144, _FRe, _FIm, 2048);\
			SplitRadix_RadixL(Re, Im, 0, 8192);

//2^14 = 16384, half = 8192, quarter = 4096
#define FFTBlock14(Re, Im) \
			FFTBlock13(Re, Im)\
			FFTBlock12(Re + 8192, Im + 8192)\
			FFTBlock12(Re + 12288, Im + 12288)\
			SplitRadix_RadixL(Re, Im, 0, 16384);
#else
//2^12 = 4096, half = 2048, quarter = 1024
#define FFTBlock12(Re, Im) \
			ComplexCopy(_FRe, _FIm, Re, Im, 2048);\
			StaticFFTExecTable[8]();\
			ComplexCopy(Re, Im, _FRe, _FIm, 2048);\
			ComplexCopy(_FRe, _FIm, Re + 2048, Im + 2048, 1024);\
			StaticFFTExecTable[7]();\
			ComplexCopy(Re + 2048, Im + 2048, _FRe, _FIm, 1024);\
			ComplexCopy(_FRe, _FIm, Re + 3072, Im + 3072, 1024);\
			StaticFFTExecTable[7]();\
			ComplexCopy(Re + 3072, Im + 3072, _FRe, _FIm, 1024);\
			SplitRadix_RadixL(Re, Im, 0, 4096);

//2^13 = 8192, half = 4096, quarter = 2048
#define FFTBlock13(Re, Im) \
			FFTBlock12(Re, Im)\
			ComplexCopy(_FRe, _FIm, Re + 4096, Im + 4096, 2048);\
			StaticFFTExecTable[8]();\
			ComplexCopy(Re + 4096, Im + 4096, _FRe, _FIm, 2048);\
			ComplexCopy(_FRe, _FIm, Re + 6144, Im + 6144, 2048);\
			StaticFFTExecTable[8]();\
			ComplexCopy(Re + 6144, Im + 6144, _FRe, _FIm, 2048);\
			SplitRadix_RadixL(Re, Im, 0, 8192);

//2^14 = 16384, half = 8192, quarter = 4096
#define FFTBlock14(Re, Im) \
			FFTBlock13(Re, Im)\
			FFTBlock12(Re + 8192, Im + 8192)\
			FFTBlock12(Re + 12288, Im + 12288)\
			SplitRadix_RadixL(Re, Im, 0, 16384);
#endif
#endif
QT += core widgets network multimedia

TARGET = RocaTool
TEMPLATE = app

INCLUDEPATH = ../..

HEADERS = \
    rocatool.h \
    spectrum.h \
    ../../editor/audio/Source.h \
    ../../editor/audio/Player.h \
    ../../editor/audio/CodecBase.h \
    ../../editor/audio/Codec.h \
    ../../editor/Utils.h \
    Interface.h \
    ../ffft/OscSinCos.hpp \
    ../ffft/OscSinCos.h \
    ../ffft/FFTRealUseTrigo.hpp \
    ../ffft/FFTRealUseTrigo.h \
    ../ffft/FFTRealSelect.hpp \
    ../ffft/FFTRealSelect.h \
    ../ffft/FFTRealPassInverse.hpp \
    ../ffft/FFTRealPassInverse.h \
    ../ffft/FFTRealPassDirect.hpp \
    ../ffft/FFTRealPassDirect.h \
    ../ffft/FFTRealFixLenParam.h \
    ../ffft/FFTRealFixLen.hpp \
    ../ffft/FFTRealFixLen.h \
    ../ffft/FFTReal.hpp \
    ../ffft/FFTReal.h \
    ../ffft/DynArray.hpp \
    ../ffft/DynArray.h \
    ../ffft/def.h \
    ../ffft/Array.hpp \
    ../ffft/Array.h \
    ../CVEDSP/Tester.h \
    ../CVEDSP/Plot.h \
    ../CVEDSP/Interpolation.h \
    ../CVEDSP/FrameProcessor.h \
    ../CVEDSP/DFT/StaticRev.h \
    ../CVEDSP/DFT/StaticFFT_Small.h \
    ../CVEDSP/DFT/StaticFFT_11.h \
    ../CVEDSP/DFT/StaticFFT_10.h \
    ../CVEDSP/DFT/StaticFFT_9.h \
    ../CVEDSP/DFT/StaticFFT_8.h \
    ../CVEDSP/DFT/StaticFFT_7.h \
    ../CVEDSP/DFT/StaticFFT_6.h \
    ../CVEDSP/DFT/StaticFFT_5.h \
    ../CVEDSP/DFT/SplitRadixGen.h \
    ../CVEDSP/DFT/SplitRadix.h \
    ../CVEDSP/DFT/Radix2Gen.h \
    ../CVEDSP/DFT/Radix2.h \
    ../CVEDSP/DFT/FFTBlock.h \
    ../CVEDSP/DFT/FFTCommon.h \
    ../CVEDSP/DFT/FFT.h \
    ../CVEDSP/DFT/DFT.h \
    ../CVEDSP/FreqDomain/Spectrum.h \
    ../CVEDSP/FreqDomain/Formant.h \
    ../CVEDSP/FreqDomain/Filter.h \
    ../CVEDSP/FreqDomain/FECSOLA.h \
    ../CVEDSP/FreqDomain/FDAnalysis.h \
    ../CVEDSP/FreqDomain/DFW.h \
    ../CVEDSP/FreqDomain/ControlPointFilter.h \
    ../CVEDSP/IntrinUtil/Memory.h \
    ../CVEDSP/IntrinUtil/LogicArray.h \
    ../CVEDSP/IntrinUtil/IntArray.h \
    ../CVEDSP/IntrinUtil/FloatArray.h \
    ../CVEDSP/IntrinUtil/Calculation.h \
    ../CVEDSP/TimeDomain/Window.h \
    ../CVEDSP/TimeDomain/TDEffects.h \
    ../CVEDSP/TimeDomain/TDAnalysis.h \
    ../CVEDSP/TimeDomain/PSOLA.h

SOURCES = \
    main.cpp \
    rocatool.cpp \
    spectrum.cpp \
    ../../editor/audio/Source.cpp \
    ../../editor/audio/Player.cpp \
    ../../editor/audio/CodecBase.cpp \
    ../../editor/audio/Codec.cpp \
    ../../editor/Utils.cpp \
    Interface.cpp

QMAKE_CXXFLAGS += -Wunused-parameter

FORMS += \
    rocatool_window.ui

RESOURCES += \
    res.qrc

INCLUDEPATH += $$PWD/../CVEDSP
DEPENDPATH += $$PWD/../CVEDSP

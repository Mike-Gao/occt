TEMPLATE = app
CONFIG += debug_and_release qt
CONFIG += lrelease
CONFIG += embed_translations
QT += widgets

TARGET = Tutorial


# SAMPLESROOT = $$quote($$(CSF_OCCTSamplesPath)/standard/qt)
SAMPLESROOT = C:/Work/occt-CR31570/samples/standard/qt

HEADERS   = src/*.h \
            $${SAMPLESROOT}/../samples_src/*.h 

SOURCES   = src/*.cxx \
            $${SAMPLESROOT}/../samples_src/*.cxx
			
RESOURCES += $${SAMPLESROOT}/../samples_src/Samples.qrc 
RESOURCES += ./res/Tutorial.qrc

TRANSLATIONS += ./res/Tutorial-string.ts


RES_DIR   = $$quote($$(RES_DIR))

INCLUDEPATH += $$quote($$(CSF_SampleSources))
INCLUDEPATH += $$quote($${SAMPLESROOT}/Common/src)
INCLUDEPATH += $$quote($$(CSF_OCCTIncludePath))


OCCT_DEFINES = $$(CSF_DEFINES)

DEFINES = $$split(OCCT_DEFINES, ;)

unix {
    UNAME = $$system(uname -s)
    LIBLIST = $$(LD_LIBRARY_PATH)
    LIBPATHS = $$split(LIBLIST,":")
    for(lib, LIBPATHS):LIBS += -L$${lib}

    CONFIG(debug, debug|release) {
        DESTDIR = ./$$UNAME/bind
        OBJECTS_DIR = ./$$UNAME/objd
        MOC_DIR = ./$$UNAME/mocd
    } else {
        DESTDIR = ./$$UNAME/bin
        OBJECTS_DIR = ./$$UNAME/obj
        MOC_DIR = ./$$UNAME/moc
    }

    MACOSX_USE_GLX = $$(MACOSX_USE_GLX)

    !macx | equals(MACOSX_USE_GLX, true): INCLUDEPATH += $$QMAKE_INCDIR_X11 $$QMAKE_INCDIR_OPENGL $$QMAKE_INCDIR_THREAD
    equals(MACOSX_USE_GLX, true): DEFINES += MACOSX_USE_GLX
    DEFINES += OCC_CONVERT_SIGNALS QT_NO_STL
    !macx | equals(MACOSX_USE_GLX, true): LIBS += -L$$QMAKE_LIBDIR_X11 $$QMAKE_LIBS_X11 -L$$QMAKE_LIBDIR_OPENGL $$QMAKE_LIBS_OPENGL $$QMAKE_LIBS_THREAD
    LIBS += -lfreeimageplus
    LIBS += -ltbb -ltbbmalloc
    QMAKE_CXXFLAGS += -std=gnu++11
}

win32 {
    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG
        DESTDIR = ./win$$(ARCH)/$$(VCVER)/bind
        OBJECTS_DIR = ./win$$(ARCH)/$$(VCVER)/objd
        MOC_DIR = ./win$$(ARCH)/$$(VCVER)/mocd
    } else {
        DEFINES += NDEBUG
        DESTDIR = ./win$$(ARCH)/$$(VCVER)/bin
        OBJECTS_DIR = ./win$$(ARCH)/$$(VCVER)/obj
        MOC_DIR = ./win$$(ARCH)/$$(VCVER)/moc
    }
    LIBS = -L$$(QTDIR)/lib;$$(CSF_OCCTLibPath)
    DEFINES += NO_COMMONSAMPLE_EXPORTS NO_IESAMPLE_EXPORTS
}

LIBS += -lTKernel -lTKMath -lTKService -lTKV3d -lTKOpenGl \
        -lTKBRep -lTKIGES -lTKSTL -lTKVRML -lTKSTEP -lTKSTEPAttr -lTKSTEP209 \
        -lTKSTEPBase -lTKGeomBase -lTKGeomAlgo -lTKG3d -lTKG2d \
        -lTKXSBase -lTKShHealing -lTKHLR -lTKTopAlgo -lTKMesh -lTKPrim \
        -lTKCDF -lTKBool -lTKBO -lTKFillet -lTKOffset -lTKLCAF -lTKCAF -lTKVCAF\

!exists($${RES_DIR}) {
    win32 {
        system(mkdir $${RES_DIR})
    } else {
        system(mkdir -p $${RES_DIR})
    }
}

 
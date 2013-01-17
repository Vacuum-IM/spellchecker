DEFINES       += HAVE_HUNSPELL
DEFINES       += HUNSPELL_STATIC
INCLUDEPATH   += ./thirdparty/
LIBS          += -lhunspell
LIBS          += -L./thirdparty/hunspell/
HEADERS       += hunspellchecker.h
SOURCES       += hunspellchecker.cpp

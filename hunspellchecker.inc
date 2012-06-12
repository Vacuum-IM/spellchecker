USE_HUNSPELL {
  DEFINES       += HAVE_HUNSPELL
  win32 {
	INCLUDEPATH += $${HUNSPELL_DEV_PATH}/include
	LIBS        += -L$${HUNSPELL_DEV_PATH}/lib
	LIBS        += -lhunspell
  }
  HEADERS       += hunspellchecker.h
  SOURCES       += hunspellchecker.cpp
}

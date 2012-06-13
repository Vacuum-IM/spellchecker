USE_HUNSPELL {
  DEFINES       += HAVE_HUNSPELL
  win32 {
	INCLUDEPATH += $${HUNSPELL_DEV_PATH}
	LIBS        += -L$${HUNSPELL_LIB_PATH}
	LIBS        += -llibhunspell
  }
  HEADERS       += hunspellchecker.h
  SOURCES       += hunspellchecker.cpp
}

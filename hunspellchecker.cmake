add_definitions(-DHAVE_HUNSPELL) 
add_definitions(-DHUNSPELL_STATIC)

# FIXME: CHECKME: Who need cmake in vacuum-im under windows?
if (WIN32)
	add_subdirectory(thirdparty/hunspell hunspell)
	set(ADD_LIBS hunspell)
	message(STATUS "Spellchecker backend: bundled hunspell")
endif (WIN32)

if (UNIX)
	FIND_PACKAGE(PkgConfig)
	pkg_check_modules(SYSTEM_HUNSPELL hunspell>=1.2.0)
	if (SYSTEM_HUNSPELL_FOUND)
		add_definitions(-DUSE_SYSTEM_HUNSPELL)
		set(ADD_LIBS ${SYSTEM_HUNSPELL_LIBRARIES})
		message(STATUS "Spellchecker backend: system hunspell")
	else (SYSTEM_HUNSPELL_FOUND)
		add_subdirectory(thirdparty/hunspell hunspell)
		set(ADD_LIBS hunspell)
		message(STATUS "Spellchecker backend: bundled hunspell")
endif (SYSTEM_HUNSPELL_FOUND)
endif (UNIX)

set(SOURCES ${SOURCES} "hunspellchecker.cpp")
set(HEADERS ${HEADERS} "hunspellchecker.h")
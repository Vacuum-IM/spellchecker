#Plugin file name
TARGET              = spellchecker
include(config.inc)

#Project Configuration
TEMPLATE            = lib
CONFIG             += plugin
QT                  = core gui xml
LIBS               += -l$${TARGET_UTILS}
LIBS               += -L$${VACUUM_LIB_PATH}
DEPENDPATH         += $${VACUUM_SRC_PATH}
INCLUDEPATH        += $${VACUUM_SRC_PATH}

#Include Backends
USE_ENCHANT {
  include(enchantchecker.inc)
} else:USE_ASPELL {
  include(aspellchecker.inc)
} else {
  include(hunspellchecker.inc)
}

#Install
include(install.inc)

#Translation
include(translations.inc)

include(spellchecker.pri)

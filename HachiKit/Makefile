# Project Name
TARGET = HachiKit

# Sources
CPP_SOURCES += HachiKit.cpp \
Screen.cpp \
sounds/Bd8.cpp \
sounds/Sd8.cpp \
sounds/SdNoise.cpp \
sounds/FmDrum.cpp \
sounds/HhSource68.cpp \
sounds/Ch.cpp \
audio/AhdEnv.cpp \
sounds/Oh.cpp \
sounds/Cy.cpp \
sounds/Cow8.cpp \
sounds/Tom.cpp \
sounds/ClickSource.cpp \
sounds/Clap.cpp \
sounds/DigiClap.cpp \
sounds/Clave8.cpp \
sounds/MultiTomSource.cpp \
sounds/MultiTom.cpp \
audio/Mixer16.cpp


OPT = -Os

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

cleanlite:
	-rm -fR $(BUILD_DIR)/HachiKit*

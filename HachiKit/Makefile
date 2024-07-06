# Project Name
TARGET = HachiKit

# Sources
CPP_SOURCES += HachiKit.cpp \
ParamSet.cpp \
Screen.cpp \
audio/AhdEnv.cpp \
audio/Mixer.cpp \
audio/Mixer16.cpp \
audio/Processing.cpp \
sounds/Bd8.cpp \
sounds/Ch.cpp \
sounds/Clap.cpp \
sounds/Clave8.cpp \
sounds/ClickSource.cpp \
sounds/Cow8.cpp \
sounds/Cy.cpp \
sounds/DigiClap.cpp \
sounds/FmDrum.cpp \
sounds/HhSource68.cpp \
sounds/MultiTom.cpp \
sounds/MultiTomSource.cpp \
sounds/Oh.cpp \
sounds/Sd8.cpp \
sounds/SdNoise.cpp \
sounds/Tom.cpp 


OPT = -Os

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

cleanlite:
	-rm -fR $(BUILD_DIR)/HachiKit*

# Project Name
TARGET = HachiKit
KIT_NAME = Kit-ABCD-32k
BIN_DIR = bin


# Sources
CPP_SOURCES += HachiKit.cpp \
ParamSet.cpp \
DrumWrapper.cpp \
Runner.cpp \
Screen.cpp \
audio/AhdEnv1.cpp \
audio/AhdEnv.cpp \
audio/Mixer.cpp \
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
sounds/FmTom.cpp \
sounds/HhSource68.cpp \
sounds/Oh.cpp \
sounds/Sd8.cpp \
sounds/SdNoise.cpp \
sounds/Tom.cpp \
sounds/Tom8.cpp 


# uncomment one of these to put code in SRAM or QSPI; use 'make program-boot' to update
#APP_TYPE=BOOT_SRAM
#APP_TYPE=BOOT_QSPI

OPT = -Os

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile


# Kits
# create a kit file with a main and build with something like:
# >  make clean; make KIT_NAME=<kitname> kit
# 
# then use dfu.sh to send the .bin to the daisy patch

cleanlite:
	-rm -fR $(BUILD_DIR)/${TARGET}*

kitbefore:
	cp $(KIT_NAME).cpp HachiKit.cpp

kitafter:
	cp $(BUILD_DIR)/$(TARGET_BIN) $(BIN_DIR)/$(KIT_NAME).bin

kit: kitbefore all kitafter


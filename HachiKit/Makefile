# Project Name
TARGET = HachiKit

# Sources
CPP_SOURCES += HachiKit.cpp \
ParamSet.cpp \
DrumWrapper.cpp \
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
sounds/HhSource68.cpp \
sounds/MultiTom.cpp \
sounds/MultiTomSource.cpp \
sounds/Oh.cpp \
sounds/Sd8.cpp \
sounds/SdNoise.cpp \
sounds/Tom.cpp 

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

cleanlite:
	-rm -fR $(BUILD_DIR)/${TARGET}*


#abcd-32k: $(BUILD_DIR)/Kit-ABCD-32k.elf $(BUILD_DIR)/Kit-ABCD-32k.hex $(BUILD_DIR)/Kit-ABCD-32k.bin
#kits: abcd-32k

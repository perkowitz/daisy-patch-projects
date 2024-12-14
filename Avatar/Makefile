# Project Name
TARGET = Avatar
BIN_DIR = bin

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisySP
HACHILIB_DIR = ../lib

# Sources
# HACHILIB_SOURCES := $(shell find $(HACHILIB_DIR) -name '*.cpp')
# LOCAL_SOURCES := $(shell find . -name '*.cpp')

# CPP_SOURCES += $(HACHILIB_SOURCES) $(LOCAL_SOURCES)

CPP_SOURCES += SynthKit.cpp \
$(HACHILIB_DIR)/ParamPage.cpp \
$(HACHILIB_DIR)/ParamSet.cpp \
$(HACHILIB_DIR)/audio/AdsrEnv.cpp \
$(HACHILIB_DIR)/audio/Mixer.cpp \
$(HACHILIB_DIR)/audio/MultiOsc.cpp \
$(HACHILIB_DIR)/audio/Processing.cpp \
Runner.cpp \
Screen.cpp \
Toph.cpp


# uncomment one of these to put code in SRAM or QSPI; use 'make program-boot' to update
#APP_TYPE=BOOT_SRAM
#APP_TYPE=BOOT_QSPI

OPT = -Os

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile


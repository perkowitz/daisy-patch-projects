# Project Name
BIN_DIR = bin

SHELL = /bin/sh
CC    = gcc
FLAGS        = -std=gnu99 -Iinclude
DEBUGFLAGS   = -O0 -D _DEBUG
RELEASEFLAGS = -O2 -D NDEBUG -combine -fwhole-program

TARGET  = hachilib.so
SOURCES = $(shell echo *.cpp)
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.cpp=.o)

PREFIX = $(DESTDIR)/usr/local
BINDIR = $(BIN_DIR)

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisySP

C_DEFS += \
-I$(LIBDAISY_DIR) \
-I$(LIBDAISY_DIR)/src/ \
-I$(LIBDAISY_DIR)/src/sys \
-I$(LIBDAISY_DIR)/src/usbd \
-I$(LIBDAISY_DIR)/src/usbh \
-I$(LIBDAISY_DIR)/Drivers/CMSIS_5/CMSIS/Core/Include/ \
-I$(LIBDAISY_DIR)/Drivers/CMSIS-DSP/Include \
-I$(LIBDAISY_DIR)/Drivers/CMSIS-Device/ST/STM32H7xx/Include \
-I$(LIBDAISY_DIR)/Drivers/STM32H7xx_HAL_Driver/Inc/ \
-I$(LIBDAISY_DIR)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
-I$(LIBDAISY_DIR)/Middlewares/ST/STM32_USB_Host_Library/Core/Inc \
-I$(LIBDAISY_DIR)/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc \
-I$(LIBDAISY_DIR)/Middlewares/ST/STM32_USB_Host_Library/Class/MIDI/Inc \
-I$(SYSTEM_FILES_DIR)/

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -fPIC -shared -pedantic -Wall -Wextra -march=native -ggdb3


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -o $(TARGET) $(OBJECTS)

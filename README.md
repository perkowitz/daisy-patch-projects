# daisy-patch-projects

Projects for the [Daisy Patch](https://electro-smith.com/products/patch), 
derived from the [Daisy Examples](https://github.com/electro-smith/DaisyExamples) repo.

## Install

- `git submodule update --init --recursive`
- ./build_libs.sh

## Build

### For most projects 

- `make clean; make`
- `make program-dfu`

### For projects that use the bootloader

- Set `APP_TYPE=BOOT_SRAM` or `APP_TYPE=BOOT_QSPI` in the Makefile
- `make clean; make`
- `make program-boot` to load the bootloader
- bootloader may boot directly into whatever was there last time. if so press reset, then boot (while LED is flashing)
- once LED is flashing for bootloader `make program-dfu` to load the new bin

## Using the bootloader

Copied from [this doc](https://github.com/electro-smith/libDaisy/blob/master/doc/md/_a7_Getting-Started-Daisy-Bootloader.md#flashing-with-the-bootloader)

### Flashing with the bootloader

With your program compiled, you have a few options available for programming it using the bootloader:

### DFU

If the bootloader's LED is pulsing in the grace period, and the Daisy is connected to your computer via USB, you can run make program-dfu. The APP_TYPE will automatically adjust the DFU command to write to the correct address. Note that you'll have to change the app type back to BOOT_NONE or remove it to flash non-bootloader programs.

### SD Card

If your Daisy is connected to a micro SD card slot, you can just drag and drop the .bin file generated in your project's build folder onto the card. If you then plug it into the Daisy and restart it, the bootloader will automatically flash it to the QSPI chip.

At this time, the SD card needs to be fully wired up with all of the 4-bit data lines connected.

### USB Drive

If your Daisy's external USB pins (D29 and D30) are connected to a USB port, you can follow the same process as SD cards above.


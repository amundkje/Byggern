LDSCRIPT = sam/flash.ld
BOOTUP = sam/sam3x/source/exceptions.c sam/sam3x/source/startup_sam3x.c sam/sam3x/source/system_sam3x.c
MCUTYPE = __SAM3X8E__

# ! Changed !
SOURCE_FILES = $(BOOTUP) main.c $(wildcard common/src/*.c)

INCLUDES = -I. -I/common/include -Inode2


# Feel free to ignore anything below this line
# ^ LUL NO :D --- Heeeell Nah!
VPATH:= $(dir $(SOURCE_FILES))
BUILD_DIR := build
OBJS := $(patsubst %.c, $(BUILD_DIR)/%$(ARCH)$(DEBUG).o, $(notdir $(SOURCE_FILES)))
#Tools
CC:=arm-none-eabi-gcc
LD:=arm-none-eabi-gcc
AR:=arm-none-eabi-ar
AS:=arm-none-eabi-as

ELF=$(BUILD_DIR)/main.elf

LDFLAGS:= -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -Wl,--gc-sections
CFLAGS:= -mcpu=cortex-m3 -mthumb -g -std=c11 -MMD -DF_CPU=4915200UL



CFLAGS+= -I sam -I sam/sam3x/include -I sam/sam3x/source -I sam/cmsis -I . -I ../node2/common/include
CFLAGS+= -D $(MCUTYPE)

.DEFAULT_GOAL := $(ELF)
# compile and generate dependancy info-DF_CPU=4915200UL
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@
	#$(CC) -MM $(CFLAGS) $< > $@.d

$(BUILD_DIR)/%.o : %.s | $(BUILD_DIR)
	$(AS) $< -o $@

$(ELF) : $(OBJS) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Changed first line: pgr -> pgrep
.PHONY: debug
debug: $(ELF)
	if pgrep openocd; then killall -s 9 openocd; fi 
	x-terminal-emulator -e openocd -f sam/openocd.cfg -c "program $(ELF) verify"
	sleep 5
	arm-eabi-gdb -tui -iex "target extended-remote localhost:3333" $(ELF)
	killall -s 9 openocd	

# Changed second line: pgr -> pgrep
.PHONY: flash
flash: $(ELF)
	#./sam/write_bin.sh sam/openocd.cfg $(BUILD_DIR)/main.elf
	if pgrep openocd; then killall -s 9 openocd; fi
	openocd -f sam/openocd.cfg -c "program ${BUILD_DIR}/main.elf verify reset exit"


# pull in dependencies
-include	$(OBJS:.o=.d)


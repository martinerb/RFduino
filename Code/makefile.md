CPU ?= cortex-m0
FREQ ?= 16000000

LINKEROUTPUT ?= build

SOURCE ?= source/
INCLUDE ?= include/
BUILD ?= build/
CORE ?= core/
CMSIS ?= core/system/CMSIS/CMSIS/Include/
LIBS ?= libraries/

PROJECTSOURCE ?= start

CPATH ?= /home/martin/Arduino-compiler/tools/gcc-arm-none-eabi-4.8.3-2014q1/bin/
COMPILER ?= arm-none-eabi

#CPATH ?=
#COMPILER ?= arm-linux-gnueabihf
CXXCOMPILER ?= $(COMPILER)-g++
CCOMPILER ?= $(COMPILER)-gcc
ARCOMPILER ?= $(COMPILER)-ar
OBJCOMPILER ?= $(COMPILER)-objcopy
#-mfloat-abi=soft

CXXFLAGS ?= -std=c99 -c -g -Os -w -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fno-builtin -mcpu=$(CPU) -DF_CPU=$(FREQ) -DARDUINO=158 -mthumb -D__RFduino__
CFLAGS ?= -std=c99 -c -g -Os -w -ffunction-sections -fdata-sections -fno-builtin -mcpu=$(CPU) -DF_CPU=$(FREQ) -DARDUINO=158 -mthumb -D__RFduino__
ARFLAGS ?= rcs

CXXTAR =  $(patsubst $(SOURCE)%.cpp,$(BUILD)%.o,$(wildcard $(SOURCE)*.cpp))
CTAR =  $(patsubst $(SOURCE)%.c,$(BUILD)%.o,$(wildcard $(SOURCE)*.c))
CXXCORE = $(patsubst $(CORE)%.cpp,$(BUILD)%.o,$(wildcard $(CORE)*.cpp))
CCORE = $(patsubst $(CORE)%.c,$(BUILD)%.o,$(wildcard $(CORE)*.c))
AFILES = $(patsubst $(BUILD)%.o,$(BUILD)%.a,$(wildcard $(BUILD)*.o))
TAROBJ = $(subst $(SOURCE),$(BUILD),$(subst cpp,o,$(wildcard $(SOURCE)*.cpp)))


	
all: $(BUILD) $(CXXCORE) $(CCORE)
	
$(BUILD)%.o: $(CORE)%.cpp 
	$(CPATH)$(CXXCOMPILER) $(CXXFLAGS) -c -o $@ $< -I$(CORE) -I$(CORE)system/ -I$(CMSIS) -I$(INCLUDE)
	
$(BUILD)%.o: $(SOURCE)%.cpp 
	$(CPATH)$(CXXCOMPILER) $(CXXFLAGS) -c -o $@ $< -I$(CORE) -I$(CORE)system/ -I$(CMSIS) -I$(INCLUDE)

$(BUILD)%.o: $(SOURCE)%.c
	$(CPATH)$(CCOMPILER) $(CFLAGS) -c -o $@ $< -I$(CORE) -I$(CORE)system/ -I$(CMSIS) -I$(INCLUDE)
	
$(BUILD)%.o: $(CORE)%.c
	$(CPATH)$(CCOMPILER) $(CFLAGS) -c -o $@ $< -I$(CORE) -I$(CORE)system/ -I$(CMSIS) -I$(INCLUDE)
	
afiles:
	$(CPATH)$(ARCOMPILER) rcs $(BUILD)core.a $(wildcard $(BUILD)*.o)	

main: $(CXXTAR) $(CTAR)

#$(CPATH)$(CXXCOMPILER) -Wl,--gc-sections --specs=nano.specs -mcpu=$(CPU) -mthumb -D__RFduino__ -TRFduino.ld -Wl,-Map,$(BUILD)$(PROJECTSOURCE).map -Wl,--cref -o $(BUILD)$(PROJECTSOURCE).elf -L$(LINKEROUTPUT) -L$(ARMLIB) -Wl,--warn-common -Wl,--warn-section-align -Wl,--start-group $(BUILD)syscalls.o $(TAROBJ) $(BUILD)variant.o $(LIBS)libRFduinoSystem.a $(LIBS)libRFduino.a $(LIBS)libRFduinoBLE.a $(LIBS)libRFduinoGZLL.a $(BUILD)core.a -Wl,--end-group 
link:
	$(CPATH)$(CXXCOMPILER) -Wl,--gc-sections --specs=nano.specs -mcpu=$(CPU) -mthumb -D__RFduino__ -TRFduino.ld -Wl,-Map,$(BUILD)$(PROJECTSOURCE).map -Wl,--cref -o $(BUILD)$(PROJECTSOURCE).elf -L$(LINKEROUTPUT) -L$(ARMLIB) -Wl,--warn-common -Wl,--warn-section-align -Wl,--start-group $(BUILD)syscalls.o $(TAROBJ) $(BUILD)variant.o $(LIBS)libRFduinoSystem.a $(LIBS)libRFduino.a $(LIBS)libRFduinoGZLL.a $(LIBS)libRFduinoBLE.a $(BUILD)core.a -Wl,--end-group 
	$(CPATH)$(OBJCOMPILER) -O ihex $(BUILD)$(PROJECTSOURCE).elf $(BUILD)$(PROJECTSOURCE).hex 
	
$(BUILD):
	mkdir $@
	
clean :
	rm -rf $(BUILD)

cleansource :
	rm -f $(CTAR)
	rm -f $(CXXTAR)
	
load1:
	/usr/bin/wine /home/martin/Repository/RFduino/RFDLoader.exe -q com1 $(BUILD)$(PROJECTSOURCE).hex
	
load2:
	/usr/bin/wine /home/martin/Repository/RFduino/RFDLoader.exe -q com2 $(BUILD)$(PROJECTSOURCE).hex

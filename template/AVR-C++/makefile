#Project create in ucDevelop [ucdevelopVersion] by [autorName]
PROJECT = [projectName]
MCU = [mmcu]
CLK = 16000000
PRGRAMMER = [programmer]
#PORT_PROGRAMMER = -P
SRC = main.c class.cpp
EXT =


#################################################################################################

# include path
INCLUDE := $(foreach dir, $(EXT), -I$(dir))
# c flags
CFLAGS    = -Wall -Os -DF_CPU=$(CLK) -mmcu=$(MCU) $(INCLUDE)
# any aditional flags for c++
CPPFLAGS =

# executables
AVRDUDE = avrdude -c $(PRGRAMMER) $(PORT_PROGRAMMER) -p $(MCU) 
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE    = avr-size --format=avr --mcu=$(MCU)
CC      = avr-g++

# generate list of objects
CFILES    = $(filter %.c, $(SRC))
EXTC     := $(foreach dir, $(EXT), $(wildcard $(dir)/*.c))
CPPFILES  = $(filter %.cpp, $(SRC))
EXTCPP   := $(foreach dir, $(EXT), $(wildcard $(dir)/*.cpp))
OBJ       = $(CFILES:.c=.o) $(EXTC:.c=.o) $(CPPFILES:.cpp=.o) $(EXTCPP:.cpp=.o)

# user targets
# compile all files
all: $(PROJECT).hex

# test programmer connectivity
test:
	$(AVRDUDE) -v

# flash program to mcu
flash: all
	$(AVRDUDE) -U flash:w:$(PROJECT).hex:i

# generate disassembly files for debugging
disasm: $(PROJECT).elf
	$(OBJDUMP) -d $(PROJECT).elf

# remove compiled files
clean:
	rm -f *.hex *.elf *.o
	$(foreach dir, $(EXT), rm -f $(dir)/*.o;)

# other targets
# objects from c files
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# objects from c++ files
.cpp.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# elf file
$(PROJECT).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $(PROJECT).elf $(OBJ)

# hex file
$(PROJECT).hex: $(PROJECT).elf
	rm -f $(PROJECT).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(PROJECT).elf $(PROJECT).hex
	$(SIZE) $(PROJECT).elf

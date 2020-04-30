.SILENT:
.DELETE_ON_ERROR:

#
#  Set to 1 to build for debugging
#
DEBUG ?= 1

#
#  Uncomment this to output a include file dependency graph when compiling.
#  'make clean && make 2>dep.txt' to get the entire graph for the project.
#
#INCLUDE_DEP_GRAPH=-H

#
#  Script for programming from the command line
#
JFLASH="${HOME}/bin/jflash.sh"

#
#  A couple of directories
#
LDSCRIPTSDIR = ldscripts
OUTPUTDIR = output

#
#  Target source files
#
APPLICATION_INC   = .

APPLICATION_CSRC  = main.c
APPLICATION_CSRC += system/cr_startup_lpc15xx.c
APPLICATION_CSRC += system/sysinit.c

#
#  FreeRTOS sources
#
FREERTOS_INC  = freertos
FREERTOS_INC += freertos/inc

FREERTOS_SRC  = freertos/src/croutine.c
FREERTOS_SRC += freertos/src/event_groups.c
FREERTOS_SRC += freertos/src/FreeRTOSCommonHooks.c
FREERTOS_SRC += freertos/src/heap_3.c
FREERTOS_SRC += freertos/src/list.c
FREERTOS_SRC += freertos/src/port.c
FREERTOS_SRC += freertos/src/queue.c
FREERTOS_SRC += freertos/src/tasks.c
FREERTOS_SRC += freertos/src/timers.c

#
#  Libraries for platform
#
LPC15xxLIB_BASE = lpcopen_v220/software

LPC15xxLIB_INC  = $(LPC15xxLIB_BASE)/CMSIS/CMSIS/Include/
LPC15xxLIB_INC += $(LPC15xxLIB_BASE)/lpc_core/lpc_chip/chip_15xx
LPC15xxLIB_INC += $(LPC15xxLIB_BASE)/lpc_core/lpc_chip/chip_15xx/config_15xx
LPC15xxLIB_INC += $(LPC15xxLIB_BASE)/lpc_core/lpc_chip/chip_15xx/usbd_rom
LPC15xxLIB_INC += $(LPC15xxLIB_BASE)/lpc_core/lpc_chip/chip_common
LPC15xxLIB_INC += $(LPC15xxLIB_BASE)/lpc_core/lpc_board/board_common
LPC15xxLIB_INC += $(LPC15xxLIB_BASE)/lpc_core/lpc_board/boards_15xx/nxp_lpcxpresso_1549

# ./lpcopen_v220/software/lpc_core/lpc_board/boards_15xx/nxp_lpcxpresso_1549/board.h

LPC15xxLIB_C_SRC  = $(shell find $(LPC15xxLIB_BASE)/lpc_core -name \*.c)

#
#  Determine platform we're running on
#
MACH := $(shell uname | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/)
ifneq (,$(findstring cygwin,$(MACH)))
  OS_PLATFORM := cygwin
else ifneq (,$(findstring linux,$(MACH)))
  OS_PLATFORM := linux
else
  OS_PLATFORM := mac
endif

ifeq ($(OS_PLATFORM),cygwin)
  ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
  CYG_ROOT_DIR := $(shell cygpath --windows --absolute ${ROOT_DIR} | sed -e 's/\\/\\\\/g' | tr '[:upper:]' '[:lower:]')
  MD5 := md5sum
else
  MD5 := md5 -q
endif

GROUPNAME := $(shell id -gn)
USERNAME := $(shell id -un)

#
#
#
APPLICATION_CSRC += ${FREERTOS_SRC}
INCLUDEDIRS  = ${APPLICATION_INC}
INCLUDEDIRS += ${FREERTOS_INC}
INCLUDEDIRS += ${LPC15xxLIB_INC}

#
#
#
LIB_ASRC = $(LPC15xxLIB_A_SRC)
LIB_CSRC = $(LPC15xxLIB_C_SRC)
APPLICATION_LDSCRIPT=$(LDSCRIPTSDIR)/lpc1549.ld

#
#  The targets
#
APPLICATION=$(OUTPUTDIR)/display

#
#  These shouldn't need to be changed
#
#  -Wcast-align is removed from WARNINGS because the Cortex-M3 will do unaligned accesses.
#
BASEINCLUDE=$(patsubst %,-I%,$(INCLUDEDIRS))

ifeq ($(OS_PLATFORM),cygwin)
  #GCC_PATH := "/cygdrive/c/Program Files (x86)/GNU Tools ARM Embedded/9 2019-q4-major/bin/"
  GCC_PATH := "/cygdrive/c/nxp/LPCXpresso_8.2.2_650/lpcxpresso/tools/bin/"
else ifeq ($(OS_PLATFORM),linux)
  GCC_PATH := "/home/jcw/opt/gcc-arm-none-eabi-7-2018-q2-update/bin/"
else
  GCC_PATH := ""
endif

GCC_PREFIX=$(GCC_PATH)arm-none-eabi-
CC=$(GCC_PREFIX)gcc
AS=$(GCC_PREFIX)as
AR=$(GCC_PREFIX)ar
NM=$(GCC_PREFIX)nm
OBJCOPY=$(GCC_PREFIX)objcopy
OBJDUMP=$(GCC_PREFIX)objdump
SIZE=$(GCC_PREFIX)size

ifneq ($(OS_PLATFORM),cygwin)
  ifneq (, $(shell which ccache))
    XX := ccache $(CC)
    CC := $(XX)
  endif
endif

CFLAGS  = $(BASEINCLUDE)
CFLAGS += -D__CODE_RED
CFLAGS += -D__USE_LPCOPEN
CFLAGS += -D__REDLIB__
CFLAGS += -DCORE_M3
CFLAGS += -D__GNU_VISIBLE
CFLAGS += -D_GNU_SOURCE
CFLAGS += -mcpu=cortex-m3
CFLAGS += -mthumb
CFLAGS += -fmessage-length=0
CFLAGS += -fno-builtin
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -Wall -Wextra -Wformat-security -Wswitch-default -Wshadow -Wpointer-arith -Wbad-function-cast -Wsign-compare -Wunused -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wno-packed-bitfield-compat
ifeq ($(DEBUG), 1)
  CFLAGS += -DDEBUG -g3 -O0 -gdwarf-2
else
  CFLAGS += -DNDEBUG -g3 -Os -fomit-frame-pointer
endif
#CFLAGS += -Wwrite-strings
#CFLAGS += -MMD -MP

APPLICATION_LDFLAGS=-nostdlib -u _printf_float -Wl,--gc-sections,--discard-none,-Map=$(APPLICATION).map,--cref,-u,Reset_Handler -T $(APPLICATION_LDSCRIPT) "-lc" "-lgcc" --specs=redlib.specs

LIB_COBJ=$(LIB_CSRC:.c=.o)
LIB_SOBJ=$(LIB_ASRC:.s=.o)
APPLICATION_COBJ=$(APPLICATION_CSRC:.c=.o)
APPLICATION_SOBJ=$(APPLICATION_ASRC:.s=.o)

APPLICATION_OBJS = $(APPLICATION_COBJ) $(APPLICATION_SOBJ) $(LIB_COBJ) $(LIB_SOBJ)

#
#
#
.PHONY: all
all : .depend $(APPLICATION).bin

.PHONY: app
app : .depend $(APPLICATION).bin
ifeq (.depend,$(wildcard .depend))
  include .depend
endif

#
#  Application targets
#
$(APPLICATION).bin : $(APPLICATION).hex
	echo "  OC    $@"
	$(OBJCOPY) --gap-fill 0xff -I ihex -O binary $(APPLICATION).hex $(APPLICATION).bin

$(APPLICATION).hex : $(APPLICATION).sym
	echo "  OC    $@"
	$(OBJCOPY) --change-addresses 0x08000000 -O ihex $(APPLICATION).elf $(APPLICATION).hex

$(APPLICATION).sym : $(APPLICATION).elf
	echo "  NM    $@"
	$(NM) -n -f sysv $(APPLICATION).elf > $@

$(APPLICATION).elf : $(APPLICATION_OBJS) $(APPLICATION_LDSCRIPT) Makefile
	echo "  LD    $@"
	$(CC) $(CFLAGS) $(APPLICATION_LDFLAGS) $(APPLICATION_OBJS) -nostartfiles -o $@
	$(OBJDUMP) -h $(APPLICATION).elf >$(APPLICATION).hdrs
	$(SIZE) $(APPLICATION).elf

#
#  .c and .s to .o rules
#
%.o : %.c Makefile
	echo "  CC    $<"
	$(CC) $(INCLUDE_DEP_GRAPH) -c $(CFLAGS) $< -o $@

%.o : %.s Makefile
	echo "  AS    $<"
	$(CC) -c $(CFLAGS) $< -o $@

#
#  Target for .lst file
#
.PHONY: lst
lst : $(APPLICATION).elf
	$(OBJDUMP) -D -S $(APPLICATION).elf >$(APPLICATION).lst

#
#  The .depend files contains the list of header files that the
#  various source files depend on.  By doing this, we'll only
#  rebuild the .o's that are affected by header files changing.
#
.depend:
	$(CC) $(CFLAGS) -MM $(APPLICATION_CSRC) > .depend
ifeq ($(OS_PLATFORM),cygwin)
	cat .depend | perl -pne "s/\Q$(CYG_ROOT_DIR)\E\\\\//; s/([^ :])\\\\/\1\//g" | perl -pne 'if (m|^(.*): \w+/\w+\.c |) { s|^(\S+) (.*?/)|\2\1 \2|; }' >.dependx
	mv .dependx .depend
endif

#
#  Utility targets
#
.PHONY: tags
tags :
	rm -f tags ctags
	find . -regex '.*\.\(c\|cpp\|h\)$$' -print0 | xargs -0 ctags
	cat tags | sed -e '/mpu_wrappers/d' > tags.tmp
	mv tags.tmp tags

.PHONY: clean
clean :
	find . -type f -name \*.o -exec rm -f {} \;
	find . -type f -name \*.d -exec rm -f {} \;
	find . -type f -name \*.exe -exec rm -f {} \;
	find . -type f -name .depend -exec rm -f {} \;
	find . -type f -name tags -exec rm -f {} \;

.PHONY: jflash
jflash :
	 $(JFLASH) -min -openprjspazbee_20.jflash -eliminate -auto -startapp -exit

.PHONY: fix_crlf
fix_crlf :
	chown -R $(USERNAME):$(GROUPNAME) *
	find . -type f -name \*.c -exec chmod 644 {} \;
	find . -type f -name \*.h -exec chmod 644 {} \;
	find . -type f -name \*.c -exec dos2unix -q {} \;
	find . -type f -name \*.h -exec dos2unix -q {} \;
	find . -type f -name \*.s -exec dos2unix -q {} \;

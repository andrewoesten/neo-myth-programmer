.SUFFIXES:
#.SILENT:

SOURCES		:=	../../src/console ../../src/core
INCLUDES	:=	$(SOURCES)
BUILD		:=	build
TARGET		:=	../../bin/neocmd
PLUGINS		:=	n64_plugin


ASFLAGS		:=
CXXFLAGS	:=	-O2 -Wall  -Wno-multichar -Wunused -fno-rtti -fno-exceptions
CFLAGS		:=	-O2 -Wall

UNAME := $(shell uname -s)

ifneq (,$(findstring n64_plugin,$(PLUGINS)))
    CXXFLAGS := $(CXXFLAGS) -Duse_n64_plugin
    CFLAGS   := $(CFLAGS)   -Duse_n64_plugin
    SOURCES  := $(SOURCES)  ../../src/conle_n64_plugin ../../src/conle_n64_plugin/SuperN64_Engine ../../src/conle_n64_plugin/SN64PLUG_NeoOpenProgrammer
    INCLUDES := $(INCLUDES) ../../src/conle_n64_plugin ../../src/conle_n64_plugin/SuperN64_Engine ../../src/conle_n64_plugin/SN64PLUG_NeoOpenProgrammer
endif

ifneq (,$(findstring MINGW,$(UNAME)))
	PLATFORM	:= win32
	EXE		:= .exe
	SYS_LIBS	:= -lsetupapi -lkernel32
	LDFLAGS		:= -s
endif

BINARIES	:=	$(TARGET)$(EXE)

ifneq (,$(findstring Linux,$(UNAME)))
	PLATFORM	:= linux
	EXE		:=
	SYS_LIB_PATH	:= -L/usr/X11R6/lib
	SYS_LIBS	:= -lm -lXext -lX11
	BINARIES	+= $(TARGET)_static
	LDFLAGS		:= -s
endif

ifneq (,$(findstring Darwin,$(UNAME)))
	PLATFORM	:= OSX
	EXE		:=
	SYS_LIB_PATH	:=
	SYS_LIBS	:= -lm
	BINARIES	+= $(TARGET)_static
	LDFLAGS		:= -framework AGL -framework OpenGL -framework Carbon -framework ApplicationServices
endif

LDLIBS := -lusb $(SYS_LIB_PATH) $(SYS_LIBS)

CXX			:=	g++
CC			:=	gcc

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))


export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
ASMFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

ifneq (,$(findstring MINGW,$(UNAME)))
	RESOURCEFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.rc)))
endif

ifneq (,$(findstring Linux,$(UNAME)))
export	STATIC		:= $(OUTPUT)_static
endif


export OFILES	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)
export DEPENDS	:=	$(OFILES:.o=.d)

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo cleaning ...
	@rm -fr $(BUILD) $(OUTPUT)$(EXE)

run:
	$(OUTPUT)$(EXE)

#---------------------------------------------------------------------------------
dist:
#---------------------------------------------------------------------------------
	@tar --exclude=*svn* -cvjf $(OUTPUT)_$(PLATFORM).tar.bz2 docs $(BINARIES) $(EXTRAS)

#---------------------------------------------------------------------------------
else

$(OUTPUT)$(EXE):	$(STATIC) $(OFILES)
	@echo Linking $(notdir $@)...
	@$(CXX) $(LDFLAGS)  $(OFILES) $(LDLIBS) -o $@

$(STATIC):	$(OFILES)
	@echo Linking $(notdir $@)...
	@$(CXX) -static $(LDFLAGS)  $(OFILES) $(LDLIBS) -o $@


%.o:	%.cpp
	@echo Compiling $(notdir $<)...
	@$(CXX)  -MMD $(CXXFLAGS) $(INCLUDE) -c $<

%.o:	%.c
	@echo Compiling $(notdir $<)...
	@$(CC)  -MMD $(CFLAGS) $(INCLUDE) -c $<

%.o:	%.s
	@echo Compiling $(notdir $<)...
	@$(CXX) -MMD $(ASFLAGS) -c $< -o $@

%.o:	%.rc
	@echo Compiling resource $(notdir $<)...
	@windres -i $< -o $@

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

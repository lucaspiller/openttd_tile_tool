######
#
# Targets:
#
# Defaults to building binary
# clean: remove intermediate build files
# osx: OS X application
# release: used by OSX to make a dmg file ready to release
#
# OS choices:
#
# Defaults to UNIX
# WIN32: building on Windows
# UNIX: building on *nix derivate (Linux, FreeBSD)
# OSX: building on Mac OS X
#
######

######
#
# General configuration
#

ifndef SDLCONFIG
	ifeq ($(shell uname), FreeBSD)
		SDLCONFIG:=sdl11-config
	else
		SDLCONFIG:=sdl-config
	endif
endif

######
#
# Compiler configuration
#

APP = TileTool
VERSION = 3
LINKOBJ  = main.o tiles.o font.o $(RES)
LIBS = $(shell $(SDLCONFIG) --static-libs) -lstdc++
INCS =  -I"include" 
CXXINCS =  -I"include/c++/3.3.1/mingw32"  -I"include/c++/3.3.1"  -I"lib/gcc/mingw32/3.4.2/include"  -I"include/c++/3.4.2/backward"  -I"include/c++/3.4.2/mingw32"  -I"include/c++/3.4.2"
CXXFLAGS = $(CXXINCS) -Wall
CFLAGS = $(INCS)  
RM = rm -f
RELEASE_FOLDER:=release

ifeq ($(shell uname), Darwin)
	#set up stuff specific to OSX
	#remember that if people should use the binary on any computer other than the one it is compiled on, it is preferred to be static linked
	# -lz can always be used on OSX since the OS itself needs it, so it's always present
	OSXAPP:=$(APP).app
	OSX:=OSX
	OS = osx
	OSX_MOVE:=OSX_MOVE

	PREFIX:=/sw/lib
	LIBS += $(PREFIX)/libSDL_image.a $(PREFIX)/libjpeg.a $(PREFIX)/libpng.a -lz	#we hope that people used the default fink location. Most people did
else
	ifdef WINDIR # we are on windows :(
		OS = win32
		EXE = .exe
	else # we are on unix
		OS = $(shell uname)-$(shell uname -m)
		EXE =
	endif
	
	LIBS += -lSDL_image
	NON_OSX_MOVE:=NON_OSX_MOVE # indicate that we are NOT building a mac binary
	
endif

BIN = $(APP)$(EXE)

ifdef OS
	ifdef VERSION
		#we should add a dot between VERSION and OS since both are present
		DOT = .
	endif
endif

RELEASE_FILE = $(APP)-$(VERSION)$(DOT)$(OS).zip

# Check if there is a windres override
ifndef WINDRES
	WINDRES = windres
endif

# Make sure we have a compiler
ifndef CPP
	CPP  = g++
endif

######
#
# What to compile
#

CXX_SOURCES += main.cpp
CXX_SOURCES += tiles.cpp
CXX_SOURCES += font.cpp
CXX_SOURCES += error.cpp

OBJS = $(C_SOURCES:%.c=%.o) $(CXX_SOURCES:%.cpp=%.o) $(OBJC_SOURCES:%.m=%.o)

######
#
# Build commands
#


# If we are verbose, we will show commands prefixed by $(Q).
# The $(Q)s get replaced by @ in non-verbose mode.
# Inspired by the Linux kernel build system.
ifdef VERBOSE
	Q =
else
	Q = @
endif

######
#
# Targets
#

### Normal build rules

all: $(BIN) $(OSX)

# main build
$(BIN): $(OBJS) $(MAKE_CONFIG)
	@echo '===> Linking $@'
	$(Q) $(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

# osx build
$(OSX):
	$(Q)rm -fr "$(OSXAPP)"
	$(Q)mkdir -p "$(OSXAPP)"/Contents/MacOS
	$(Q)mkdir -p "$(OSXAPP)"/Contents/Resources
	$(Q)echo "APPL????" > "$(OSXAPP)"/Contents/PkgInfo
	$(Q)cp OSX/Info.plist "$(OSXAPP)"/Contents/Info.plist
	$(Q)cp OSX/openttd.icns "$(OSXAPP)"/Contents/Resources/TT.icns
	$(Q)cp $(BIN) "$(OSXAPP)"/Contents/MacOS/TT

# create a release folder
CREATE_FOLDER:
	$(Q)mkdir -p "$(RELEASE_FOLDER)"

# target to make a release
#   first it compiles and build the mac binary if needed, then it creates the folder for the release
#   after that it copies the binary with either $(NON_OSX_MOVE) or $(OSX_MOVE). Both can't be active at the same time
#   last it copies the data and COPYING
RELEASE: all CREATE_FOLDER $(OSX_MOVE) $(NON_OSX_MOVE)
	$(Q)cp -R data "$(RELEASE_FOLDER)"/data
	$(Q)cp COPYING "$(RELEASE_FOLDER)"/COPYING
	$(Q)cp README "$(RELEASE_FOLDER)"/README
	$(Q)zip $(RELEASE_FILE) "$(RELEASE_FOLDER)"/* > /dev/null
	@echo '===> Created $(RELEASE_FILE)'

clean:
	@echo '===> Cleaning up'
	$(Q)rm -rf $(BIN) core $(OBJS) *~ $(RELEASE_FOLDER) $(RELEASE_FILE)
	
.PHONY: clean all $(OSX)

### Internal build rules

%.o: %.c $(MAKE_CONFIG)
	@echo '===> Compiling $<'
	$(Q)$(CC) $(CFLAGS) $(CDEFS) -c -o $@ $<

%.o: %.cpp  $(MAKE_CONFIG)
	@echo '===> Compiling $<'
	$(Q)$(CXX) $(CFLAGS) $(CDEFS) -c -o $@ $<

%.o: %.m  $(MAKE_CONFIG)
	@echo '===> Compiling $<'
	$(Q)$(CC) $(CFLAGS) $(CDEFS) -c -o $@ $<
	
#copy the binary
$(NON_OSX_MOVE):
	$(Q)cp $(BIN) "$(RELEASE_FOLDER)"/$(BIN)

#copy the mac binary
$(OSX_MOVE):
	$(Q)cp -R "$(OSXAPP)" "$(RELEASE_FOLDER)"/

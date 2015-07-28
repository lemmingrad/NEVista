SHELL = /bin/bash
CC = g++

TARGET = ds3d-server

PROJECTDIR = ~/gitroot/DS3DServer
NEVISTA = ~/gitroot/NEVista


SOURCES = $(shell find $(PROJECTDIR) -type f -name '*.cpp')
SOURCES += $(shell find $(NEVISTA) -type f -name '*.cpp')

SOURCEDIRS = $(sort $(dir $(SOURCES)))

HEADERS = $(shell find $(PROJECTDIR) -type f -name '*.h')
HEADERS += $(shell find $(NEVISTA) -type f -name '*.h')

HEADERDIRS = $(patsubst %, -I%, $(sort $(dir $(HEADERS))))

COMMON =


LIBS = -lsqlite3

LIBDIRS =


CFLAGS = -Wno-multichar \
	-Wno-fatal-errors \
	-Wno-error \
	-Wno-long-long \
	-Wno-unused-parameter \
	-Wall \
	-Wextra \
	-g \
	-pedantic

DEBUGFLAGS = -O0 -D _DEBUG -D LINUX
RELEASEFLAGS = -O2 -D NDEBUG -D LINUX -combine -fwhole-program


BUILDDIR = $(PROJECTDIR)/obj
DEBUGDIR = $(PROJECTDIR)/debug
RELEASEDIR = $(PROJECTDIR)/release


COMPCOL = \\e[32m
DEPCOL = \\e[1\;32m
CLEANCOL = \\e[32m
NOCOL = \\e[0m



######################################
# DO NOT MODIFY BELOW THIS LINE
######################################

vpath %.cpp $(SOURCEDIRS)

OBJECTS = $(patsubst %.cpp, $(BUILDDIR)/%.o, $(notdir $(SOURCES)))
DEPS = $(OBJECTS:.o=.d)


all: $(TARGET)

$(TARGET): $(OBJECTS) $(COMMON)
	@echo -e $(COMPCOL)Linking debug:$(NOCOL)
	@mkdir -p $(DEBUGDIR)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(HEADERDIRS) -o $(DEBUGDIR)/$(TARGET) $(OBJECTS) $(LIBDIRS) $(LIBS)
	@cp -f $(DEBUGDIR)/$(TARGET) $(PROJECTDIR)/$(TARGET) 

release: $(SOURCES) $(HEADERS) $(COMMON)
	@echo -e $(COMPCOL)Compiling/linking release:$(NOCOL)
	@mkdir -p $(RELEASEDIR)
	$(CC) $(CFLAGS) $(RELEASEFLAGS) $(HEADERDIRS) -o $(RELEASEDIR)/$(TARGET) $(SOURCES) $(LIBDIRS) $(LIBS)
	@cp -f $(RELEASEDIR)/$(TARGET) $(PROJECTDIR)/$(TARGET)

clean:
	@echo -e $(CLEANCOL)Cleaning:$(NOCOL)
	-rm -rf $(BUILDDIR)
	-rm -f $(TARGET)

vars:
	@echo Project: $(PROJECTDIR)
	@echo Sources: $(SOURCES)
	@echo Headers: $(HEADERS)
	@echo Objects: $(OBJECTS)
	@echo Dependencies: $(DEPS)
	@echo Src folders: $(SOURCEDIRS)
	@echo Include folders: $(HEADERDIRS)
	@echo Lib folders: $(LIBDIRS)
	@echo Obj build: $(BUILDDIR)		
	@echo Debug build: $(DEBUGDIR) 
	@echo Release build: $(RELEASEDIR)


## First attempt at auto-dependencies.
## No caching, and doesn't behave quite right.

# .SECONDEXPANSION:
# 
# $(foreach SRC, $(SOURCES), \
#  $(eval $(notdir $(SRC))_DEPS = \
#   $(shell $(CC) $(HEADERDIRS) -MM $(SRC) \
#   | sed \
#     -e 's/\\//' \
#     -e 's/.*://' \
#     -e 's/[ \t]*//' \
#   > $(SRC)_dep.d \
#  ) \
# ) \
#)
#
#$(BUILDDIR)/%.o: %.cpp $($(notdir $<)_DEPS)
#	@echo -e $(COMPCOL)Compiling $@:$(NOCOL)
#	@echo -e $(DEPCOL)Dependencies: $($(notdir $<)_DEPS)$(NOCOL)
#	@mkdir -p $(BUILDDIR)
#	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(HEADERDIRS) -c -o $@ $<


## Second attempt at auto-dependencies
## Much improved! This one actually seems to work!

-include $(DEPS)

$(BUILDDIR)/%.o: %.cpp
	@echo -e $(COMPCOL)Compiling $@:$(NOCOL)
	@mkdir -p $(BUILDDIR)
	$(CC) -c $(CFLAGS) $(DEBUGFLAGS) $(HEADERDIRS) -MMD -o $@ $<

.PHONY : all release clean vars

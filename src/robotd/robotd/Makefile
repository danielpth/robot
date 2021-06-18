#****************************************************************************
#
# Makefile for robotd.
# Daniel Porath
#
#****************************************************************************

# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := NO

# PROFILE can be set to YES to include profiling info, or NO otherwise
PROFILE        := NO

#TINYXML_USE_STL := YES
#****************************************************************************

CC     := gcc
CXX    := g++
LD     := g++
AR     := ar rc
RANLIB := ranlib

DEBUG_CFLAGS     := -Wall -g -DDEBUG -std=c11
DEBUG_CXXFLAGS   := -Wall -g -DDEBUG -std=c++11

RELEASE_CFLAGS   := -Wall -Werror -O2 -std=c17
RELEASE_CXXFLAGS := -Wall -Werror -O2 -std=c++17

DEBUG_LDFLAGS    := -g
RELEASE_LDFLAGS  := -g0

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
   STRIP        := @echo
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
   STRIP        := strip -s
endif

ifeq (YES, ${PROFILE})
   CFLAGS   := ${CFLAGS} -pg -O3
   CXXFLAGS := ${CXXFLAGS} -pg -O3
   LDFLAGS  := ${LDFLAGS} -pg
endif

#****************************************************************************
# Preprocessor directives
#****************************************************************************

ifeq (YES, ${TINYXML_USE_STL})
  DEFS := -DTIXML_USE_STL
else
  DEFS :=
endif

LINUX_LIBS := pthread

LIBS		 := $(addprefix -l, ${LINUX_LIBS}) $(addprefix -l, ${LOCAL_LIBS})

#****************************************************************************
# Include paths
#****************************************************************************

#INCS := -I/usr/include/g++-2 -I/usr/local/include
INCS := -I/usr/include/ -I/usr/include/c++/ -I/usr/include/c++/9/ -I/usr/local/include


#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := ${CFLAGS}   ${DEFS}
CXXFLAGS := ${CXXFLAGS} ${DEFS}

#****************************************************************************
# Targets of the build
#****************************************************************************

OUTPUT := robotd
OBJ_DIR := ./obj/
SRC_DIR := ./

#****************************************************************************
# Source files
#****************************************************************************

FILES_CPP := $(basename $(notdir $(wildcard ${SRC_DIR}*.cpp)))

SRCS := $(addprefix ${SRC_DIR}, $(addsuffix .cpp,${FILES_CPP}))

# Add on the sources for libraries
#SRCS := ${SRCS}

OBJS := $(addprefix ${OBJ_DIR}, $(addsuffix .o, ${FILES_CPP}))

all:
	make _all -j8

_all: ${OBJ_DIR} ${OUTPUT}

${OBJ_DIR}:
	@echo "Creating OBJ dir"
	mkdir -p ${OBJ_DIR}
#****************************************************************************
# Output
#****************************************************************************

${OUTPUT}: ${OBJS}
	${LD} -o $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS}
	${STRIP} ${OUTPUT}

#****************************************************************************
# common rules
#****************************************************************************

# Rules for compiling source files to object files
${OBJ_DIR}%.o : ${SRC_DIR}%.cpp
	${CXX} -c ${CXXFLAGS} ${INCS} $< -o $@

${OBJ_DIR}%.o : ${SRC_DIR}%.c
	-uncrustify -c ../cfg/uncrustify.cfg -f $< -o $<
	${CC} -c ${CFLAGS} ${INCS} $< -o $@

#dist:
#	bash makedistlinux

clean:
	-rm -f core ${OBJS} ${OUTPUT}
	-rm -f ../src/*.unc-backup~
	-rm -f ../src/*.unc-backup.md5~
	-rm -f ../include/*.unc-backup~
	-rm -f ../include/*.unc-backup.md5~

distclean: clean
	#make -C ../lib/tinyxml clean

depend:
	#makedepend ${INCS} ${SRCS}

format:
	-uncrustify -c ../cfg/uncrustify.cfg --replace --if-changed --mtime ../src/*.cpp
	#-uncrustify -c ../cfg/uncrustify.cfg --replace --if-changed --mtime ../include/*.h



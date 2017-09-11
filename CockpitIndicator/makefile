#---------
#
# CppUTest Examples Makefile
#
#----------

#Set this to @ to keep the makefile quiet
ifndef SILENCE
	SILENCE = @
endif

#--- Inputs ----#
COMPONENT_NAME = CockpitNodeFirmware
#CPPUTEST_HOME = /cygdrive/c/cpputest-3.6
CPPUTEST_HOME = ~/cpputest-3.6

CPPUTEST_USE_EXTENSIONS = Y
CPP_PLATFORM = Gcc
CPPUTEST_PEDANTIC_ERRORS = N

# This line is overriding the default new macros.  This is helpful
# when using std library includes like <list> and other containers
# so that memory leak detection does not conflict with stl.
###CPPUTEST_MEMLEAK_DETECTOR_NEW_MACRO_FILE = -include ApplicationLib/ExamplesNewOverrides.h
SRC_DIRS = \
	firmware

TEST_SRC_DIRS = \
	tests

INCLUDE_DIRS =\
  .\
  firmware\
  $(CPPUTEST_HOME)/include\

CPPUTEST_EXE_FLAGS =

include $(CPPUTEST_HOME)/build/MakefileWorker.mk



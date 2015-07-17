# $Id: Makefile.in,v 1.3 2013/04/10 09:15:36 miyachi Exp $
#
#   This is an example of user program using installed Mellin package.
#   Using headers and libraries installed by "make install",
#   one can make own program using Mellin Class Library.
#
#   copy the entire user_program directory and rename.

TARGET = user_program
OBJS   = ESRData.o Rho.o MyKernel.o

## ----------------------------------------------------------------------- #
##                   ROOT Object Dictionary Management                     #
## ----------------------------------------------------------------------- #
ROOTOBJS    = Fitter.o LineShape.o DipoleKernel.o KernelCore.o MixedDensity.o AGaus.o MyApplication.o ESRLine.o ESR.o ESRHeader.o ESRHeaderElement.o
ROOTOBJ_HH  = $(patsubst %.o, %.hh, $(ROOTOBJS))
ROOTLINKDEF = RootLinkDef.hh
ROOTDICT_CC = RootObjDict.cc
ROOTCINT = rootcint

OBJS += $(patsubst %.cc, %.o, $(ROOTDICT_CC)) $(ROOTOBJS)

# ---------------------------------------------------------------------- #
# Default setting  
# ---------------------------------------------------------------------- #
prefix      = /opt/mellin
exec_prefix = ${prefix}

CXX         = g++
#CXX         = clang++

CFLAGS      = -g -O2 -MMD
CXXFLAGS    = -g -O2 -MMD
CPPFLAGS    = 
LDFLAGS     = 
LDLIBS      =

CPPFLAGS   += -I${prefix}/include
LDFLAGS    += -L${exec_prefix}/lib

## -----------------------------------------------------------------
## Mellin libraries
## -----------------------------------------------------------------
LIB_UTILITY   = -lUtility -pthread -lxml++-2.6 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lsigc-2.0 -lgthread-2.0 -lrt -lglib-2.0  
LIB_TRANSFORM = -lTransform
LDLIBS += $(LIB_TRANSFORM) $(LIB_UTILITY)

# ---------------------------------------------------------------------- #
#  ROOT setting
# ---------------------------------------------------------------------- #
ROOTSYS = `root-config --prefix`
CPPFLAGS += `root-config --cflags`
LDLIBS   += `root-config --libs` -lMinuit
# LDLIBS   += `root-config --glibs`
# LDLIBS   += -lMinuit

all: $(TARGET)

$(TARGET) : $(OBJS)

## ----------------------------------------------------------------------- #
##                   ROOT Object Dictionary Management                     #
## ----------------------------------------------------------------------- #
$(ROOTDICT_CC) : $(ROOTOBJ_HH) $(ROOTLINKDEF)
	$(ROOTSYS)/bin/$(ROOTCINT) -f $@ -c -I${prefix}/include $(ROOTOBJ_HH) $(ROOTLINKDEF)

clean:
	-@/bin/rm -f *.o *.*~

-include $(patsubst %.o, %.d, $(OBJS))

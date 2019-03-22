############################################################
#
#  Project:        quTAG User Library
#
#  Filename:       Makefile
#
#  Purpose:        GCC makefile for examples
#
#  Author:         N-Hands GmbH & Co KG
#
############################################################
# $Id: makefile.lx,v 1.1 2017/12/14 12:50:37 trurl Exp $

CXX = $(shell root-config --cxx)

INC = $(shell pwd)
LD = $(shell root-config --ld)
CPPFLAGS := $(shell root-config --cflags) -I$(INC)/../inc
LDFLAGS := $(shell root-config --glibs) $(STDLIBDIR)

CPPFLAGS += -g 

TARGET = read_qutag

SRC = read_qutag.cc

OBJ = $(SRC:.c=.o)

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(LD) $(CPPFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^


%.o : %.c
	$(CXX) $(CPPFLAGS) -o $@ -c $<
	@echo $@
	@echo $<


clean : 
	rm -f *.o $(TARGET)


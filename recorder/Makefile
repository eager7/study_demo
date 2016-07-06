##########################################
#  	eyeSight Mobile Technologies Ltd.
#	(c) Copyright.  All Rights Reserved.
##########################################
.SUFFIXES:
.SUFFIXES:	.cpp .c .o

CROSS_COMPILE?=arm-linux-gnueabihf-

#CFLAGS=-Wall -Wno-switch -Wno-unused-function -Wno-comment 
#CFLAGS+=-fsigned-char
CFLAGS=-Wall -fsigned-char -O3 -g -mfloat-abi=hard -marm -mfpu=neon -pthread -Wno-reorder
LDFLAGS=-marm -pthread 
LIBS=-L./lib/

TARGET=eyesight_recorder

##############################################
#             set target compiler
##############################################
CC=$(CROSS_COMPILE)g++
CCC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)g++

################################################
#            include folders list
################################################
BUILD_DIR := build
OBJDIR = build

INC := \
	-I./ \

############################# Build rules #############################
# These rules tell make how to create .o from .cpp/.c files
# The flags "-MMD -MP -MT $@" tell GCC to create a dependency file (.d)
#######################################################################
$(OBJDIR)/%.o:	%.cpp
	$(CC) -c -MMD -MP -MT $@ $(CFLAGS) $(INC) $< -o $@

$(OBJDIR)/%.o:	%.c
	$(CCC) -c -MMD -MP -MT $@ $(CFLAGS) $(INC) $< -o $@

.PHONY: all
all: $(TARGET)

#############################################################
# The following is the list of CPP and C files to be compiled
#############################################################
CPP_SRCS := $(wildcard ./*.cpp)
C_SRCS	 := $(wildcard ./*.c)

####################################
# merge all source files and objects
####################################
SRCS   		:= $(CPP_SRCS) $(C_SRCS)
_OBJS		:= $(CPP_SRCS:%.cpp=%.o) $(C_SRCS:%.c=%.o)
OBJS 		:= $(patsubst %,$(OBJDIR)/%,$(_OBJS))
-include $(OBJS:.o=.d)

####################################
#            build 
####################################
$(OBJS): | $(OBJDIR)
$(OBJDIR): 
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJS)
	mkdir -p build/
	rm -f $(TARGET)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)
	@sleep 1
	@echo "---------------------------------------------------------------";\
	echo "Build completed.";\
	echo "---------------------------------------------------------------";\

####################################
#              clean
####################################
.PHONY: clean
clean:
	rm -f build/*.d build/*.o $(TARGET)

CC = g++
LD = ld
CFLAGS =-Wall -Wextra -g -std=c++11
CUFLAGS = -g
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/dsp/*.cpp) $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/util/*.c)
BIN_DIR = bin
BIN = dsp_main
INCLUDE = -Iinc
LIBS = 

CUDA_EN = 1
CUDA_PATH = /opt/cuda
#CUDA_PATH = /usr/local/cuda
CUDA_LIBPATH = $(CUDA_PATH)/lib64
CUDA_INCPATH = $(CUDA_PATH)/include
ifeq ($(CUDA_EN),1)
GPU_DEF 		= -DGPU_EN
CUDA_SOURCES 	= $(wildcard $(SRCDIR)/cuda/*.cu)
CUDA_OBJDIR 	= $(OBJDIR)/cuda
#CUDA_OBJECTS 	= $(CUDA_SOURCES:.cu=.o)
CUDA_OBJECTS 	= $(CUDA_SOURCES:$(SRCDIR)/cuda/%.cu=$(CUDA_OBJDIR)/%.o)
LIBS 			+= -L$(CUDA_LIBPATH) -lcudart -lcuda 
INCLUDE 		+= -I$(CUDA_INCPATH)
else
GPU_DEF 		= 
CUDA_SOURCES 	=
CUDA_OBJDIR 	=
CUDA_OBJECTS 	=
CUDA_BLOB 		=
endif

OBJDIR = obj
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

GTEST_DIR = tests
GTEST_BIN = dsp_gtest
GTEST_SRC = $(GTEST_DIR)/*.cpp

$(OBJDIR)/%.o:$(SRCDIR)/%.cpp
	mkdir -p $(@D)	# generate the directory
	$(CC) $(CFLAGS) $(INCLUDE) $(GPU_DEF) -c $< -o $(@)

$(OBJDIR)/%.o:$(SRCDIR)/%.c
	mkdir -p $(@D)	# generate the directory
	$(CC) $(CFLAGS) $(INCLUDE) $(GPU_DEF) -c $< -o $(@)

$(OBJDIR)/cuda/%.o:$(SRCDIR)/cuda/%.cu
	mkdir -p $(@D)	# generate the directory
	nvcc -c $(CUFLAGS) $(INCLUDE) $(GPU_DEF) $< -o $(@)

$(BIN_DIR)/$(BIN): $(OBJECTS) $(CUDA_OBJECTS)
	mkdir -p $(BIN_DIR)
	echo $(CUDA_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE) $(GPU_DEF) -o $(BIN_DIR)/$(BIN) $(OBJECTS) $(CUDA_OBJECTS) $(LIBS)
	@echo "Program built"

$(BIN_DIR)/$(GTEST_BIN): $(OBJECTS) $(CUDA_OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) $(GPU_DEF) -lgtest -o $(BIN_DIR)/$(GTEST_BIN) $(OBJECTS) $(CUDA_OBJECTS) $(GTEST_SRC) $(LIBS) 
	@echo "gtest suite built"

all: $(BIN_DIR)/$(BIN)

gtest: $(BIN_DIR)/$(GTEST_BIN)
	./$(GTEST_BIN)

clean:
	rm -r $(OBJDIR)
	rm -r $(BIN_DIR)

test: $(BIN_DIR)/$(BIN)
	$(BIN_DIR)/$(BIN)

mem: $(BIN_DIR)/$(BIN)
	valgrind --leak-check=yes $(BIN_DIR)/$(BIN)

debug: $(BIN_DIR)/$(BIN)
	gdb $(BIN_DIR)/$(BIN)

.PHONY: all clean test gtest mem debug

CC = g++
LD = ld
CFLAGS =-Wall -Wextra -g
CUFLAGS = -g
SOURCES = $(wildcard src/dsp/*.cpp) $(wildcard src/*.cpp) 
EXECUTABLE_DIR = bin
EXECUTABLE = cufft_main
INCLUDE = -Iinc -I/opt/cuda/include 

GPU_EN = GPU_EN
#GPU_EN  = GPU_DIS

OBJDIR = obj
OBJECTS = $(SOURCES:.cpp=.o)

CUDA_SOURCES = $(wildcard src/cuda/*.cu)
CUDA_OBJDIR = $(OBJDIR)/cuda
CUDA_OBJECTS = $(CUDA_SOURCES:.cu=.o)
CUDA_BLOB = cuda.o

LIBS = -L/opt/cuda/lib64/ -lcudart -lcuda 


GTEST_DIR = tests
GTEST_EXEC = cufft_gtest
GTEST_SRC = $(GTEST_DIR)/*.cpp

#.cpp.o:
%.o:%.cpp
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -D$(GPU_EN) -c $< -o $(OBJDIR)/$(@F)

#.cu.o:
%.o:%.cu
	mkdir -p $(CUDA_OBJDIR)
	nvcc -c $(CUFLAGS) $(INCLUDE) -D$(GPU_EN) $< -o $(CUDA_OBJDIR)/$(@F)

$(EXECUTABLE): $(OBJECTS) $(CUDA_OBJECTS)
	mkdir -p $(EXECUTABLE_DIR)
	echo $(CUDA_OBJECTS)
ifeq ($(CUDA_OBJECTS), )
	$(CC) $(CFLAGS) $(LIBS) -o $(EXECUTABLE_DIR)/$(EXECUTABLE) $(addprefix $(OBJDIR)/,$(notdir $(OBJECTS)))
else
	$(CC) $(CFLAGS) $(LIBS) -D$(GPU_EN) -o $(EXECUTABLE_DIR)/$(EXECUTABLE) $(addprefix $(OBJDIR)/,$(notdir $(OBJECTS))) $(addprefix $(CUDA_OBJDIR)/,$(notdir $(CUDA_OBJECTS)))
endif

$(GTEST_EXEC): $(OBJECTS) $(CUDA_OBJECTS)
ifeq ($(CUDA_OBJECTS), )
	$(CC) $(CFLAGS) $(LIBS) -lgtest -o $(GTEST_DIR)/$(GTEST_EXEC) $(addprefix $(OBJDIR)/,$(notdir $(OBJECTS)))$(GTEST_SRC)
else
	$(CC) $(CFLAGS) $(LIBS) -D$(GPU_EN) -lgtest -o $(GTEST_DIR)/$(GTEST_EXEC) $(addprefix $(OBJDIR)/,$(notdir $(OBJECTS))) $(addprefix $(CUDA_OBJDIR)/,$(notdir $(CUDA_OBJECTS))) $(GTEST_SRC)
endif

all: $(EXECUTABLE)
	@echo "Program built"

gtest: $(GTEST_EXEC)
	@echo "gtest built"
	./$(GTEST_EXEC)

clean:
	rm -r $(OBJDIR)
	rm -r $(EXECUTABLE_DIR)

test: $(EXECUTABLE)
	$(EXECUTABLE_DIR)/$(EXECUTABLE)

mem: $(EXECUTABLE)
	valgrind --leak-check=yes $(EXECUTABLE_DIR)/$(EXECUTABLE)

debug: $(EXECUTABLE)
	gdb $(EXECUTABLE_DIR)/$(EXECUTABLE)

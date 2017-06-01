CC = g++
LD = ld
CFLAGS =-Wall -Wextra -g
SOURCES = $(wildcard src/*.c) $(wildcard src/*.cu)
EXECUTABLE = cufft_main
INCLUDE = -Iinc -I/opt/cuda/include 
OBJDIR = obj
CUDAOBJDIR = $(OBJDIR)/cuda
OBJECTS = $(SOURCES:.c=.o) 
CUDA_OBJECTS = $(SOURCES:.cu=.o)
CUDA_BLOB = cuda.o

LIBS = -lcuda -lcudart 


GTEST_DIR = tests
GTEST_EXEC = cufft_gtest
GTEST_SRC = $(GTEST_DIR)/*.cpp

.c.o:
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $(OBJDIR)/$(@F)

.cu.o:
	mkdir -p $(CUDAOBJDIR)
	nvcc -c $< -o $(CUDAOBJDIR)/$(@F)

$(EXECUTABLE): $(OBJECTS) $(CUDA_OBJECTS)
	nvcc -dlink $(addprefix $(CUDAOBJDIR)/,$(notdir $(CUDA_OBJECTS))) -o $(CUDAOBJDIR)/$(CUDA_BLOB)
	$(CC) $(CFLAGS) $(LIBS) -o $(EXECUTABLE) $(addprefix $(OBJDIR)/,$(notdir $(OBJECTS))) $(CUDAOBJDIR)$(CUDA_BLOB)

$(GTEST_EXEC): $(OBJECTS) $(CUDA_OBJECTS)
	nvcc -dlink $(addprefix $(CUDAOBJDIR)/,$(notdir $(CUDA_OBJECTS))) -o $(CUDAOBJDIR)/$(CUDA_BLOB)
	$(CC) $(CFLAGS) $(LIBS) -lgtest -o $(GTEST_EXEC) $(addprefix $(OBJDIR)/,$(notdir $(OBJECTS))) $(CUDAOBJDIR)$(CUDA_BLOB) $(GTEST_SRC)

all: $(EXECUTABLE)
	@echo "Program built"

gtest: $(GTEST_EXEC)
	@echo "gtest built"
	./$(GTEST_EXEC)

clean:
	rm -r $(OBJDIR)
	rm $(EXECUTABLE)

test: $(EXECUTABLE)
	./$(EXECUTABLE)

mem: $(EXECUTABLE)
	valgrind --leak-check=yes ./$(EXECUTABLE)

debug: $(EXECUTABLE)
	gdb ./$(EXECUTABLE)

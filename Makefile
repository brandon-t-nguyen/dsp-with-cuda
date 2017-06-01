CC = g++
LD = ld
CFLAGS =-Wall -Wextra -g
SOURCES = $(wildcard src/*.c) 
EXECUTABLE = cufft_main
INCLUDE = -Iinc -I/opt/cuda/include 

OBJDIR = obj
OBJECTS = $(SOURCES:.c=.o) 

CUDA_SOURCES = $(wildcard src/cuda/*.cu)
CUDA_OBJDIR = $(OBJDIR)/cuda
CUDA_OBJECTS = $(CUDA_SOURCES:.cu=.o)
CUDA_BLOB = cuda.o

LIBS = -lcuda


GTEST_DIR = tests
GTEST_EXEC = cufft_gtest
GTEST_SRC = $(GTEST_DIR)/*.cpp

.c.o:
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $(OBJDIR)/$(@F)

.cu.o:
	mkdir -p $(CUDA_OBJDIR)
	nvcc -c $< -o $(CUDA_OBJDIR)/$(@F)

$(EXECUTABLE): $(OBJECTS) $(CUDA_OBJECTS)
ifeq ($(CUDA_OBJECTS), )
	$(CC) $(CFLAGS) $(LIBS) -o $(EXECUTABLE) $(addprefix $(OBJDIR)/,$(notdir $(OBJECTS)))
else
	nvcc -dlink $(addprefix $(CUDA_OBJDIR)/,$(notdir $(CUDA_OBJECTS))) -o $(CUDA_OBJDIR)/$(CUDA_BLOB)
	$(CC) $(CFLAGS) $(LIBS) -o $(EXECUTABLE) $(addprefix $(OBJDIR)/,$(notdir $(OBJECTS))) $(CUDA_OBJDIR)/$(CUDA_BLOB)
endif

$(GTEST_EXEC): $(OBJECTS) $(CUDA_OBJECTS)
ifeq ($(CUDA_OBJECTS), )
	$(CC) $(CFLAGS) $(LIBS) -lgtest -o $(GTEST_EXEC) $(addprefix $(OBJDIR)/,$(notdir $(OBJECTS)))$(GTEST_SRC)
else
	nvcc -dlink $(addprefix $(CUDA_OBJDIR)/,$(notdir $(CUDA_OBJECTS))) -o $(CUDA_OBJDIR)/$(CUDA_BLOB)
	$(CC) $(CFLAGS) $(LIBS) -lgtest -o $(GTEST_EXEC) $(addprefix $(OBJDIR)/,$(notdir $(OBJECTS))) $(CUDA_OBJDIR)/$(CUDA_BLOB) $(GTEST_SRC)
endif

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

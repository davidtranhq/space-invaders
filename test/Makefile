INCLUDE_FLAGS = -I../include \
				-Ii8080
LINKER_FLAGS = 
LIBRARY_FLAGS = 
CFLAGS = -DDEBUG -g

_DEPS = cpu.hpp test.hpp
DEPS = $(patsubst %, ../include/%, $(_DEPS))

ODIR = obj
_OBJS = test.o main_test.o
OBJS = $(patsubst %, $(ODIR)\\%, $(_OBJS))

_CPU_DEPS = cpu.hpp
CPU_DEPS = $(patsubst %, ../include/%, $(_CPU_DEPS))

CPU_ODIR = ..\src\obj
CPU_SRC_DIR = ..\src
_CPU_OBJS = cpu.o instructions.o
CPU_OBJS = $(patsubst %, $(CPU_ODIR)\\%, $(_CPU_OBJS))

	
test: $(OBJS) $(CPU_OBJS) i8080\\i8080.o
	g++ -o $@ $^ $(INCLUDE_FLAGS) $(LINKER_FLAGS) $(LIBRARY_FLAGS)
	
$(ODIR)\\%.o: %.cpp $(DEPS)
	g++ -c -o $@ $< $(INCLUDE_FLAGS) $(CFLAGS)

$(CPU_ODIR)\\%.o: $(CPU_SRC_DIR)\\%.cpp $(CPUS_DEPS)
	$(MAKE) cpu -C ../src

i8080\\i8080.o : i8080\\i8080.c i8080\\i8080.h
	gcc -c -o $@ $< -g -Wall -Wextra -O2 -std=c99 -pedantic

.PHONY: clean cpu 

clean:
	del $(OBJS) /Q


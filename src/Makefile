INCLUDE_FLAGS = -I../include \
				-IC:/mingw_dev_lib/include/SDL2
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2
LIBRARY_FLAGS = -LC:/mingw_dev_lib/lib
CFLAGS = -DDEBUG -g
_DEPS = cpu.hpp machine.hpp
DEPS = $(pathsubst %, ..\\include\\%, $(_DEPS))
ODIR = obj
_OBJS = cpu.o machine.o instructions.o main.o
OBJS = $(patsubst %, $(ODIR)\\%, $(_OBJS))
	

	
$(ODIR)\\%.o: %.cpp $(DEPS)
	g++ -c -o $@ $< $(INCLUDE_FLAGS) $(CFLAGS)
	
emulator: $(OBJS)
	g++ -o $@ $^ $(INCLUDE_FLAGS) $(LINKER_FLAGS) $(LIBRARY_FLAGS)

.PHONY: clean cpu

CPU_OBJS = $(patsubst %, $(ODIR)\\%, cpu.o instructions.o)

cpu: $(CPU_OBJS) 

clean:
	del $(OBJS) /Q


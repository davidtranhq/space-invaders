# The compiler we are using
CC = g++

# include directories
INCLUDE_FLAGS = -I../include \
				-IC:/mingw_dev_lib/include/SDL2
				
CFLAGS = -DDEBUG
# dependency directory
IDIR = ../include
# object directory
ODIR = obj
# libraries needed
LIBRARY_FLAGS = -lmingw32 -lSDL2main -lSDL2
# library directories
LIBRARY_DIR = -LC:/mingw_dev_lib/lib

_DEPS = cpu.hpp machine.hpp
DEPS = $(pathsubst %,$(IDIR)/%,$(_DEPS))

_OBJS = cpu.o arithmetic.o branch.o data_transfer.o logic.o stack.o test.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(INCLUDE_FLAGS) $(CFLAGS)
	
test: $(OBJS)
	$(CC) -o $@ $^ $(INCLUDE_FLAGS) $(LIBRARY_DIR) $(LIBRARY_FLAGS)
	
.PHONY: clean

clean:
	del $(ODIR)\*.o /Q
CXX_FLAGS=-Wall -pedantic
CXX_COMP_FLAGS=$(CXX_FLAGS)
CXX_LINK_FLAGS=$(CXX_FLAGS) -lgflags -lglog
BINARIES=connect4

all: $(BINARIES)

connect4.o: connect4.cpp
	$(CXX) -c $^ $(CXX_COMP_FLAGS)

connect4: connect4.o
	$(CXX) -o $@ $< $(CXX_LINK_FLAGS)

clean:
	rm -f *.o *~
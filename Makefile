CXX_FLAGS=-std=c++0x -Wall -pedantic -O4 -m64 -DNDEBUG
CXX_COMP_FLAGS=$(CXX_FLAGS)
CXX_LINK_FLAGS=$(CXX_FLAGS) -lgflags -lglog
BINARIES=connect4 weight_tunning

all: $(BINARIES)

Board.o: Board.cpp Board.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

Player.o: Player.cpp Player.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

Minimax.o: Minimax.cpp Minimax.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

Node.o: Node.cpp Node.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

Utils.o: Utils.cpp Utils.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

connect4.o: connect4.cpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

weight_tunning.o: weight_tunning.cpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

connect4: connect4.o Board.o Player.o Node.o Utils.o
	$(CXX) -o $@ $^ $(CXX_LINK_FLAGS)

weight_tunning: weight_tunning.o Board.o Player.o Node.o Utils.o
	$(CXX) -o $@ $^ $(CXX_LINK_FLAGS)

clean:
	rm -f *.o *~
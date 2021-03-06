CXX_FLAGS=-std=c++0x -Wall -pedantic -O4 -DNDEBUG
CXX_COMP_FLAGS=$(CXX_FLAGS)
CXX_LINK_FLAGS=$(CXX_FLAGS) -lgflags -lglog -lpthread -pthread
BINARIES=connect4 weight_tunning

all: $(BINARIES)

Board.o: Board.cpp Board.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

Coord.o: Coord.cpp Coord.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

Heuristic.o: Heuristic.cpp Heuristic.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

Player.o: Player.cpp Player.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

Negamax.o: Negamax.cpp Negamax.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

Utils.o: Utils.cpp Utils.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

Winner.o: Winner.cpp Winner.hpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

connect4.o: connect4.cpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

weight_tunning.o: weight_tunning.cpp
	$(CXX) -c $< $(CXX_COMP_FLAGS)

connect4: connect4.o Board.o Coord.o Player.o Negamax.o Heuristic.o Utils.o Winner.o
	$(CXX) -o $@ $^ $(CXX_LINK_FLAGS)

weight_tunning: weight_tunning.o Board.o Coord.o Player.o Negamax.o Heuristic.o Utils.o Winner.o
	$(CXX) -o $@ $^ $(CXX_LINK_FLAGS)

clean:
	rm -f *.o *~
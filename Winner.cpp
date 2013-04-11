#include "Winner.hpp"

const uint8_t Winner::NONE = 0;

Winner::Winner() : player(NONE) {
}

Winner::Winner(const uint8_t p, const Coord c[4])
    : cells{c[0], c[1], c[2], c[3]}, player(p) {
}

bool Winner::operator == (const Winner& oth) const {
  return player == oth.player;
}

bool Winner::operator != (const Winner& oth) const {
  return player != oth.player;
}

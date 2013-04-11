#ifndef WINNER_HPP_
#define WINNER_HPP_

#include "Coord.hpp"
#include <stdint.h>

struct Winner {
  static const uint8_t NONE;
  Coord cells[4];
  uint8_t player;
  Winner();
  Winner(const uint8_t p, const Coord c[4]);
  bool operator == (const Winner& oth) const;
  bool operator != (const Winner& oth) const;
};

#endif  // WINNER_HPP_

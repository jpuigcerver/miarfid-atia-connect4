#ifndef COORD_HPP_
#define COORD_HPP_

#include <stdint.h>
#include <iostream>

struct Coord {
  uint16_t c;
  uint16_t r;
  Coord();
  Coord(const uint16_t c, const uint16_t r);
  friend std::ostream& operator << (std::ostream& os, const Coord& c) {
    os << "(" << c.c << "," << c.r << ")";
    return os;
  }
};

#endif  // COORD_HPP_

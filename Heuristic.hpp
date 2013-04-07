#ifndef HEURISTIC_HPP_
#define HEURISTIC_HPP_

#include "Board.hpp"

#include <stdint.h>

class Heuristic {
 public:
  virtual float operator () (const Board& b, const uint8_t pa, const uint8_t pb) const = 0;
};

class SimpleHeuristic : public Heuristic {
 public:
  virtual float operator () (const Board& b, const uint8_t pa, const uint8_t pb) const;
 private:
  virtual float LineHeuristic(const uint8_t line[4], const uint8_t pa, const uint8_t pb) const;
};

class WeightHeuristic : public Heuristic {
 public:
  WeightHeuristic(const float weights[6]);
  virtual float operator () (const Board& b, const uint8_t pa, const uint8_t pb) const;
 private:
  virtual float LineHeuristic(const uint8_t line[4], const uint8_t pa, const uint8_t pb) const;
  float weights_[6];
};

#endif

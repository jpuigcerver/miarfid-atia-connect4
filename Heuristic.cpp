#include "Heuristic.hpp"

#include <cmath>

float SimpleHeuristic::LineHeuristic(
    const uint8_t line[4], const uint8_t pa, const uint8_t pb) const {
  size_t counter[2] = {0, 0};
  for (size_t i = 0; i < 4; ++i) {
    if (line[i] == pa) { ++counter[0]; }
    else if (line[i] == pb) { ++counter[1]; }
  }
  if (counter[0] == 4) return +INFINITY;
  else if (counter[1] == 4) return -INFINITY;
  else return 0.0f;
}

float SimpleHeuristic::operator () (
    const Board& b, const uint8_t pa, const uint8_t pb) const {
  for (uint16_t c = 0; c < b.Cols(); ++c) {
    for (uint16_t r = 0; r < b.Rows(); ++r) {
      if (r + 3 < b.Rows()) {
        const uint8_t vert[4] = { b.Get(c, r), b.Get(c, r + 1),
                                  b.Get(c, r + 2), b.Get(c, r + 3) };
        const float score = LineHeuristic(vert, pa, pb);
        if (!std::isfinite(score)) return score;
      }
      if (c + 3 < b.Cols()) {
        const uint8_t hori[4] = { b.Get(c, r), b.Get(c + 1, r),
                                  b.Get(c + 2, r), b.Get(c + 3, r) };
        const float score = LineHeuristic(hori, pa, pb);
        if (!std::isfinite(score)) return score;
      }
      if (c + 3 < b.Cols() && r + 3 < b.Rows()) {
        const uint8_t dia1[4] = { b.Get(c, r), b.Get(c + 1, r + 1),
                                  b.Get(c + 2, r + 2), b.Get(c + 3, r + 3) };
        const float score = LineHeuristic(dia1, pa, pb);
        if (!std::isfinite(score)) return score;
      }
      if (c + 3 < b.Cols() && r >= 3) {
        const uint8_t dia2[4] = { b.Get(c, r), b.Get(c + 1, r - 1),
                                  b.Get(c + 2, r - 2), b.Get(c + 3, r - 3) };
        const float score = LineHeuristic(dia2, pa, pb);
        if (!std::isfinite(score)) return score;
      }
    }
  }
  return 0.0f;
}

WeightHeuristic::WeightHeuristic(const float weights[6]) {
  weights_[0] = weights[0];
  weights_[1] = weights[1];
  weights_[2] = weights[2];
  weights_[3] = weights[3];
  weights_[4] = weights[4];
  weights_[5] = weights[5];
}

float WeightHeuristic::LineHeuristic(
    const uint8_t line[4], const uint8_t pa, const uint8_t pb) const {
  size_t counter[2] = {0, 0};
  for (size_t i = 0; i < 4; ++i) {
    if (line[i] == pa) { ++counter[0]; }
    else if (line[i] == pb) { ++counter[1]; }
  }
  if (counter[0] == 4) { return INFINITY; }
  else if (counter[1] == 4) { return -INFINITY; }
  else if (counter[0] > 0 && counter[1] == 0) {
    return (counter[0]/3) * weights_[2] +
        (counter[0]/2) * weights_[1] +
        counter[0] * weights_[0];
  } else if (counter[0] == 0 && counter[1] > 0) {
    return (counter[1]/3) * weights_[5] +
        (counter[1]/2) * weights_[4] +
        counter[1] * weights_[3];
  }
  return 0.0f;
}

float WeightHeuristic::operator () (
    const Board& b, const uint8_t pa, const uint8_t pb) const {
  float score = 0.0f;
  for (uint16_t c = 0; c < b.Cols(); ++c) {
    for (uint16_t r = 0; r < b.Rows(); ++r) {
      if (r + 3 < b.Rows()) {
        const uint8_t vert[4] = { b.Get(c, r), b.Get(c, r + 1),
                                  b.Get(c, r + 2), b.Get(c, r + 3) };
        score += LineHeuristic(vert, pa, pb);
      }
      if (c + 3 < b.Cols()) {
        const uint8_t hori[4] = { b.Get(c, r), b.Get(c + 1, r),
                                  b.Get(c + 2, r), b.Get(c + 3, r) };
        score += LineHeuristic(hori, pa, pb);
      }
      if (c + 3 < b.Cols() && r + 3 < b.Rows()) {
        const uint8_t dia1[4] = { b.Get(c, r), b.Get(c + 1, r + 1),
                                  b.Get(c + 2, r + 2), b.Get(c + 3, r + 3) };
        score += LineHeuristic(dia1, pa, pb);
      }
      if (c + 3 < b.Cols() && r >= 3) {
        const uint8_t dia2[4] = { b.Get(c, r), b.Get(c + 1, r - 1),
                                  b.Get(c + 2, r - 2), b.Get(c + 3, r - 3) };
        score += LineHeuristic(dia2, pa, pb);
      }
    }
  }
  return score;
}


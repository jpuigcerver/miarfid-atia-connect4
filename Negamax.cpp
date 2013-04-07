#include "Negamax.hpp"

#include <cmath>
#include <algorithm>
#include <random>

extern std::default_random_engine PRNG;

std::pair<float, uint32_t> Negamax(
    const Board& board, const uint8_t pa, const uint8_t pb, const size_t depth,
    const Heuristic& h, const bool shuffle, size_t* nodes) {
  if (nodes != NULL) { ++(*nodes); }
  float v = h(board, pa, pb);
  if (std::isfinite(v) == false || depth == 0) {
    return std::pair<float,uint32_t>(v, ~0);
  }
  std::vector<std::pair<uint32_t, Board> > ch_board =
      board.Expand(pa);
  if (ch_board.size() == 0) {
    return std::pair<float,uint32_t>(v, ~0);
  }
  if (shuffle) {
    std::shuffle(ch_board.begin(), ch_board.end(), PRNG);
  }
  uint32_t m = ch_board.front().first;
  v = -INFINITY;
  for (const auto& chb : ch_board) {
    const float sc = -(Negamax(
        chb.second, pb, pa, depth - 1, h, shuffle, nodes).first);
    if (sc > v) { v = sc; m = chb.first; }
  }
  return std::pair<float,uint32_t>(v, m);
}

std::pair<float, uint32_t> NegamaxAlphaBeta(
    const Board& board, const uint8_t pa, const uint8_t pb, const size_t depth,
    const Heuristic& h, const bool shuffle, float alpha, float beta, size_t* nodes) {
  if (nodes != NULL) { ++(*nodes); }
  float v = h(board, pa, pb);
  if (std::isfinite(v) == false || depth == 0) {
    return std::pair<float,uint32_t>(v, ~0);
  }
  std::vector<std::pair<uint32_t, Board> > ch_board =
      board.Expand(pa);
  if (ch_board.size() == 0) {
    return std::pair<float,uint32_t>(v, ~0);
  }
  if (shuffle) {
    std::shuffle(ch_board.begin(), ch_board.end(), PRNG);
  }
  uint32_t m = ch_board.front().first;
  v = -INFINITY;
  for (const auto& chb : ch_board) {
    const float sc = -(NegamaxAlphaBeta(
        chb.second, pb, pa, depth - 1, h, shuffle, -beta, -alpha, nodes).first);
    if (sc > v) { v = sc; m = chb.first; }
    if (sc > alpha) { alpha = sc; }
    if (alpha >= beta) { v = sc; m = chb.first; break; }
  }
  return std::pair<float,uint32_t>(v, m);
}

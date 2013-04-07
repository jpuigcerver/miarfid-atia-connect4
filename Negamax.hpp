#ifndef MINIMAX_HPP_
#define MINIMAX_HPP_

#include <utility>
#include <stdint.h>
#include "Board.hpp"
#include "Heuristic.hpp"

std::pair<float, uint32_t> Negamax(
    const Board& board, const uint8_t pa, const uint8_t pb, const size_t depth,
    const Heuristic& h, const bool shuffle, size_t* nodes = NULL);

std::pair<float, uint32_t> NegamaxAlphaBeta(
    const Board& board, const uint8_t pa, const uint8_t pb, const size_t depth,
    const Heuristic& h, const bool shuffle, float alpha, float beta,
    size_t* nodes = NULL);

#endif

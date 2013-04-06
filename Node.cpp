#include "Node.hpp"
#include <math.h>
#include <glog/logging.h>
#include <algorithm>

Node::Node(const Board& board, const uint8_t ids[2], const size_t depth)
    : board_(board), score_(0.0f), best_m_(0), num_children_(0),
      winner_(board_.CheckWinner()), depth_(depth) {
  player_ids_[0] = ids[0];
  player_ids_[1] = ids[1];
  if (winner_.player != Winner::NONE) {
    score_ = (winner_.player == player_ids_[0] ? INFINITY : -INFINITY);
  }
}

size_t Node::NumChildren() const {
  return num_children_;
}

std::pair<uint32_t, float> Node::BestMovement() const {
  return std::pair<uint32_t, float>(best_m_, score_);
}

void Node::ExpandMinimax(const size_t depth) {
  if (winner_.player != Winner::NONE) { return; }
  if (board_.CheckFull()) { return; }
  if (depth == 0) { ComputeHeuristic(); return; }
  // Generate children
  std::list<std::pair<uint32_t, Board> > ch_board =
      board_.Expand(player_ids_[0]);
  // Check there are chidren to work with
  CHECK_GT(ch_board.size(), 0);
  const uint8_t player_ids_ch[2] = {player_ids_[1], player_ids_[0]};
  // Expand children and select the best one
  best_m_ = ch_board.front().first;
  score_ = -INFINITY;
  for (const auto& chb : ch_board) {
    Node* ch = CreateNode(chb.second, player_ids_ch, depth_ + 1);
    ch->ExpandMinimax(depth - 1);
    num_children_ += ch->NumChildren() + 1;
    const float sc = -ch->score_;
    if (sc > score_) { score_ = sc; best_m_ = chb.first; }
    delete ch;
  }
}

void Node::ExpandAlphaBeta(const size_t depth, float alpha, float beta) {
  if (winner_.player != Winner::NONE) { return; }
  if (board_.CheckFull()) { return; }
  if (depth == 0) { ComputeHeuristic(); return; }
  // Generate children
  std::list<std::pair<uint32_t, Board> > ch_board =
      board_.Expand(player_ids_[0]);
  // Check there are chidren to work with
  CHECK_GT(ch_board.size(), 0);
  const uint8_t player_ids_ch[2] = {player_ids_[1], player_ids_[0]};
  // Expand children and select the best one
  best_m_ = ch_board.front().first;
  score_ = -INFINITY;
  for (const auto& chb : ch_board) {
    Node* ch = CreateNode(chb.second, player_ids_ch, depth_ + 1);
    ch->ExpandAlphaBeta(depth - 1, -beta, -alpha);
    num_children_ += ch->NumChildren() + 1;
    const float sc = -ch->score_;
    if (sc > score_) { score_ = sc; best_m_ = chb.first; }
    if (sc > alpha) { alpha = sc; }
    if (alpha >= beta) { score_ = sc; best_m_ = chb.first; break; }
    delete ch;
  }
}

// Very naive heuristic
Heuristic00_Node::Heuristic00_Node(
    const Board& board, const uint8_t ids[2], const size_t depth)
    : Node(board, ids, depth) {
}

Node* Heuristic00_Node::CreateNode(
    const Board& board, const uint8_t ids[2], const size_t depth) const {
  return new Heuristic00_Node(board, ids, depth);
}

void Heuristic00_Node::ComputeHeuristic()  {
  score_ = 0.0f;
}

// Heuristic 01
Heuristic01_Node::Heuristic01_Node(
    const Board& board, const uint8_t ids[2], const size_t depth,
    const float weights[6]) : Node(board, ids, depth) {
  memcpy(weights_, weights, sizeof(float) * 6);
}

Node* Heuristic01_Node::CreateNode(
    const Board& board, const uint8_t ids[2], const size_t depth) const {
  return new Heuristic01_Node(board, ids, depth, weights_);
}

float Heuristic01_Node::LineHeuristic(const uint8_t line[4]) const {
  size_t counter[2] = {0, 0};
  for (size_t i = 0; i < 4; ++i) {
    if (line[i] == player_ids_[0]) { ++counter[0]; }
    else if (line[i] == player_ids_[1]) { ++counter[1]; }
  }
  if (counter[0] > 0 && counter[1] == 0) {
    if (counter[0] == 4) { return INFINITY; }
    return (counter[0]/3) * weights_[2] +
        (counter[0]/2) * weights_[1] +
        counter[0] * weights_[0];
  } else if (counter[0] == 0 && counter[1] > 0) {
    if (counter[1] == 4) { return -INFINITY; }
    return (counter[1]/3) * weights_[5] +
        (counter[1]/2) * weights_[4] +
        counter[1] * weights_[3];
  }
  return 0.0f;
}

void Heuristic01_Node::ComputeHeuristic() {
  score_ = 0.0f;
  for (uint16_t c = 0; c < board_.Cols(); ++c) {
    for (uint16_t r = 0; r < board_.Rows(); ++r) {
      if (r + 3 < board_.Rows()) {
        uint8_t vert[4] = { board_.Get(c, r), board_.Get(c, r + 1), board_.Get(c, r + 2), board_.Get(c, r + 3) };
        score_ += LineHeuristic(vert);
      }
      if (c + 3 < board_.Cols()) {
        uint8_t hori[4] = { board_.Get(c, r), board_.Get(c + 1, r), board_.Get(c + 2, r), board_.Get(c + 3, r) };
        score_ += LineHeuristic(hori);
      }
      if (c + 3 < board_.Cols() && r + 3 < board_.Cols()) {
        uint8_t dia1[4] = { board_.Get(c, r), board_.Get(c + 1, r + 1), board_.Get(c + 2, r + 2), board_.Get(c + 3, r + 3) };
        score_ += LineHeuristic(dia1);
      }
      if (c + 3 < board_.Cols() && r >= 3) {
        uint8_t dia2[4] = { board_.Get(c, r), board_.Get(c + 1, r - 1), board_.Get(c + 2, r - 2), board_.Get(c + 3, r - 3) };
        score_ += LineHeuristic(dia2);
      }
    }
  }
}

#include "Player.hpp"

#include <glog/logging.h>
#include <iostream>
#include <random>

extern std::default_random_engine PRNG;

Player::Player(const uint8_t player_ids[2]) {
  player_ids_[0] = player_ids[0];
  player_ids_[1] = player_ids[1];
}

uint8_t Player::Id() const {
  return player_ids_[0];
}

// HumanPlayer
HumanPlayer::HumanPlayer(const uint8_t player_ids[2]) : Player(player_ids) {
  LOG(INFO) << "Player = " << player_ids_[0] << ": Type = " << "Human";
}

uint32_t HumanPlayer::Move(const Board& b) {
  std::cout << "Current board: " << std::endl << b;
  uint16_t col = 0;
  do {
    std::cout << "Player " << Id() << ", choose column (0 - "
              << b.Cols() - 1 << "): ";
    std::cin >> col;
  } while (col >= b.Cols() || b.Height(col) >= b.Rows());
  return col;
}

// RandomPlayer
RandomPlayer::RandomPlayer(const uint8_t player_ids[2]) : Player(player_ids) {
  LOG(INFO) << "Player = " << player_ids_[0] << ": Type = " << "Random";
}

uint32_t RandomPlayer::Move(const Board& b) {
  std::vector<uint16_t> not_full_cols;
  for (uint16_t c = 0; c < b.Cols(); ++c) {
    if (b.Height(c) < b.Rows()) {
      not_full_cols.push_back(c);
    }
  }
  if (not_full_cols.size() == 0) {
    return 0;
  }
  std::uniform_int_distribution<uint16_t> uniform(0, not_full_cols.size() - 1);
  return not_full_cols[uniform(PRNG)];
}

// MinimaxPlayer
MinimaxPlayer::MinimaxPlayer(const uint8_t player_ids[2], const size_t max_depth)
    : Player(player_ids), root_(NULL), max_depth_(max_depth) {
  LOG(INFO) << "Player = " << player_ids_[0] << ": Type = " << "Minimax";
  LOG(INFO) << "Player = " << player_ids_[0] << ": Depth = " << max_depth_;
}

MinimaxPlayer::~MinimaxPlayer() {
  delete root_;
}

size_t MinimaxPlayer::NumChildren() const {
  return root_->NumChildren() + 1;
}

uint32_t MinimaxPlayer::Move(const Board& b) {
  Expand(b);
  LOG(INFO) << "Player = " << player_ids_[0] << ": Nodes = " << NumChildren();
  std::pair<uint32_t, float> best_move = root_->BestMovement();
  return best_move.first;
}

void MinimaxPlayer::Expand(const Board& init_board) {
  if (root_ != NULL) { delete root_; }
  root_ = CreateRoot(init_board);
  //root_->ExpandMinimax(max_depth_);
  root_->ExpandAlphaBeta(max_depth_, -INFINITY, +INFINITY);
}


// Heuristic00_MinimaxPlayer
Heuristic00_MinimaxPlayer::Heuristic00_MinimaxPlayer(
    const uint8_t player_ids[2], const size_t max_depth)
    : MinimaxPlayer(player_ids, max_depth) {
  LOG(INFO) << "Player = " << player_ids_[0]
            << ": Heuristic = " << "Heuristic00";
}

Node* Heuristic00_MinimaxPlayer::CreateRoot(const Board& init_board) const {
  return new Heuristic00_Node(init_board, player_ids_, 0);
}


// Heuristic01_MinimaxPlayer
Heuristic01_MinimaxPlayer::Heuristic01_MinimaxPlayer(
    const uint8_t player_ids[2], const size_t max_depth,
    const float weights[6]) : MinimaxPlayer(player_ids, max_depth), weights_{
  weights[0], weights[1], weights[2], weights[3], weights[4], weights[5]} {
  LOG(INFO) << "Player = " << player_ids_[0] << ": Heuristic = "
            << "Heuristic01";
  LOG(INFO) << "Player = " << player_ids_[0] << ": Weights = "
            << weights_[0] << ", " << weights_[1] << ", "
            << weights_[2] << ", " << weights_[3] << ", "
            << weights_[4] << ", " << weights_[5];
}

Node* Heuristic01_MinimaxPlayer::CreateRoot(const Board& init_board) const {
  return new Heuristic01_Node(init_board, player_ids_, 0, weights_);
}

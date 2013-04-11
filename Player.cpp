#include "Player.hpp"

#include <glog/logging.h>
#include <iostream>
#include <random>
#include <chrono>

extern std::default_random_engine PRNG;

Player::Player(const uint8_t player_ids[2])
    : player_ids_{player_ids[0], player_ids[1]} {}

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

// NegamaxPlayer generic
template <class Heuristic>
NegamaxPlayer<Heuristic>::NegamaxPlayer(
    const uint8_t player_ids[2], const size_t max_depth, const Heuristic& heur,
    const bool shuffle) : Player(player_ids), max_depth_(max_depth),
                          heuristic_(heur), shuff_(shuffle) {
  LOG(INFO) << "Player = " << player_ids_[0] << ": Type = " << "Negamax";
  LOG(INFO) << "Player = " << player_ids_[0] << ": Depth = " << max_depth_;
}

template<class Heuristic>
uint32_t NegamaxPlayer<Heuristic>::Move(const Board& b) {
  size_t num_nodes = 0;
  const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  const std::pair<float, uint32_t> best_move =
      Negamax(b, player_ids_[0], player_ids_[1], max_depth_, heuristic_,
              shuff_, &num_nodes);
  const std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
  std::chrono::duration<float> ts = t2 - t1;
  LOG(INFO) << "Player = " << player_ids_[0] << ": Nodes = " << num_nodes << ", Time = " << ts.count() << "sec.";
  return best_move.second;
}

// NegamaxAlphaBetaPlayer generic
template <class Heuristic>
NegamaxAlphaBetaPlayer<Heuristic>::NegamaxAlphaBetaPlayer(
    const uint8_t player_ids[2], const size_t max_depth, const Heuristic& heur,
    const bool shuffle) : Player(player_ids), max_depth_(max_depth),
                          heuristic_(heur), shuff_(shuffle) {
  LOG(INFO) << "Player = " << player_ids_[0] << ": Type = " << "NegamaxAlphaBeta";
  LOG(INFO) << "Player = " << player_ids_[0] << ": Depth = " << max_depth_;
}

template<class Heuristic>
uint32_t NegamaxAlphaBetaPlayer<Heuristic>::Move(const Board& b) {
  size_t num_nodes = 0;
  const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  const std::pair<float, uint32_t> best_move = NegamaxAlphaBeta(
      b, player_ids_[0], player_ids_[1], max_depth_, heuristic_,
      shuff_, -INFINITY, +INFINITY, &num_nodes);
  const std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
  std::chrono::duration<float> ts = t2 - t1;
  LOG(INFO) << "Player = " << player_ids_[0] << ": Nodes = " << num_nodes << ", Time = " << ts.count() << "sec.";
  return best_move.second;
}

// SimpleHeuristic with Negamax
SimpleHeuristic_NegamaxPlayer::SimpleHeuristic_NegamaxPlayer(
    const uint8_t player_ids[2], const size_t max_depth, const bool shuffle)
    : NegamaxPlayer(player_ids, max_depth, SimpleHeuristic(), shuffle) {
  LOG(INFO) << "Player = " << player_ids_[0]
            << ": Heuristic = Heuristic00";
}

// WeightHeuristic with Negamax
WeightHeuristic_NegamaxPlayer::WeightHeuristic_NegamaxPlayer(
    const uint8_t player_ids[2], const size_t max_depth,
    const float weights[6], const bool shuffle)
    : NegamaxPlayer(player_ids, max_depth, WeightHeuristic(weights), shuffle) {
  LOG(INFO) << "Player = " << player_ids_[0]
            << ": Heuristic = Heuristic01";
  LOG(INFO) << "Player = " << player_ids_[0] << ": Weights = "
            << weights[0] << ", " << weights[1] << ", "
            << weights[2] << ", " << weights[3] << ", "
            << weights[4] << ", " << weights[5];
}

// SimpleHeuristic with Negamax and Alpha-Beta pruning
SimpleHeuristic_NegamaxAlphaBetaPlayer::SimpleHeuristic_NegamaxAlphaBetaPlayer(
    const uint8_t player_ids[2], const size_t max_depth, const bool shuffle)
    : NegamaxAlphaBetaPlayer(
        player_ids, max_depth, SimpleHeuristic(), shuffle) {
  LOG(INFO) << "Player = " << player_ids_[0]
            << ": Heuristic = Heuristic00";
}

// WeightHeuristic with Negamax and Alpha-Beta pruning
WeightHeuristic_NegamaxAlphaBetaPlayer::WeightHeuristic_NegamaxAlphaBetaPlayer(
    const uint8_t player_ids[2], const size_t max_depth,
    const float weights[6], const bool shuffle)
    : NegamaxAlphaBetaPlayer(
        player_ids, max_depth, WeightHeuristic(weights), shuffle) {
  LOG(INFO) << "Player = " << player_ids_[0]
            << ": Heuristic = Heuristic01";
  LOG(INFO) << "Player = " << player_ids_[0] << ": Weights = "
            << weights[0] << ", " << weights[1] << ", "
            << weights[2] << ", " << weights[3] << ", "
            << weights[4] << ", " << weights[5];
}

NetworkPlayer::NetworkPlayer(const uint8_t player_ids[2], const int fd)
    : Player(player_ids), sockfd(fd) {}

uint32_t NetworkPlayer::Move(const Board& b) {
  char* buff = NULL;
  size_t size = 0;
  size_t mov = 0;
  b.Serialize(&buff, &size);
  CHECK_EQ(write(sockfd, buff, size), size);
  CHECK_EQ(read(sockfd, &mov, sizeof(uint32_t)), sizeof(uint32_t));
  return mov;
}

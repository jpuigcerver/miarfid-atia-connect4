#include "Board.hpp"
#include "Player.hpp"
#include "Utils.hpp"

#include <glog/logging.h>
#include <google/gflags.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <random>

DEFINE_string(o, "", "Output filename. Use '-' for stdout");
DEFINE_uint64(rows, 6, "Board rows");
DEFINE_uint64(cols, 7, "Board columns");
DEFINE_uint64(seed, 0, "Random seed");
DEFINE_string(ai, "Human:Human", "Valid intelligences: Human | Random | "
              "SimpleNegamax | SimpleAlphaBeta | WeightNegamax | WeightAlphaBeta");
DEFINE_string(max_depth, "5:5", "Max. depth for Minimax algorithm");
DEFINE_string(wh, "1;5;100;-2;-6;-200:1;5;100;-2;-6;-200", "Values for weight heuristic");
DEFINE_string(random, "0:0", "Non-deterministic Negamax algorithm");

class Game {
 public:
  typedef enum {PLY_HUMAN, PLY_RANDOM, PLY_SIMPLE_NEGAMAX, PLY_SIMPLE_ALPHABETA,
                PLY_WEIGHT_NEGAMAX, PLY_WEIGHT_ALPHABETA} PlayerType;
  Game() : board_(Board(FLAGS_cols, FLAGS_rows)), curr_player_(0) {
    // Parse AI type from arguments
    std::string player_types_str[2];
    splitStrIntoTwoStr(FLAGS_ai, player_types_str);
    player_type_[0] = getPlayerTypeFromString(player_types_str[0]);
    player_type_[1] = getPlayerTypeFromString(player_types_str[1]);
    // Parse Max-Depth from arguments
    splitStrIntoTwoSize_t(FLAGS_max_depth, player_max_depth_);
    // Parse weight heuristic options
    splitStrIntoTwoFloatLists(FLAGS_wh, player_wh_);
    CHECK_EQ(player_wh_[0].size(), 6);
    CHECK_EQ(player_wh_[1].size(), 6);
    // Parse Negamax random expansion
    splitStrIntoTwoBool(FLAGS_random, player_random_);

    players_[0] = createPlayer(0, 'O', 'X');
    players_[1] = createPlayer(1, 'X', 'O');
  }
  ~Game() {
    delete players_[0];
    delete players_[1];
  }
  static PlayerType getPlayerTypeFromString(const std::string& str) {
    if (str == "Human") {
      return Game::PLY_HUMAN;
    } else if (str == "Random") {
      return Game::PLY_RANDOM;
    } else if (str == "SimpleNegamax") {
      return Game::PLY_SIMPLE_NEGAMAX;
    } else if (str == "SimpleAlphaBeta") {
      return Game::PLY_SIMPLE_ALPHABETA;
    } else if (str == "WeightNegamax") {
      return Game::PLY_WEIGHT_NEGAMAX;
    } else if (str == "WeightAlphaBeta") {
      return Game::PLY_WEIGHT_ALPHABETA;
    } else {
      LOG(WARNING) << "Wrong player type: \"" << str << "\". Using Human.";
      return Game::PLY_HUMAN;
    }
  }
  Player* createPlayer(const uint8_t p, const uint8_t my_id, const uint8_t en_id) {
    const uint8_t player_ids[2] = {my_id, en_id};
    switch(player_type_[p]) {
      case Game::PLY_HUMAN:
        return new HumanPlayer(player_ids);
      case Game::PLY_RANDOM:
        return new RandomPlayer(player_ids);
      case Game::PLY_SIMPLE_NEGAMAX:
        return new SimpleHeuristic_NegamaxPlayer(player_ids, player_max_depth_[p], player_random_[p]);
      case Game::PLY_SIMPLE_ALPHABETA:
        return new SimpleHeuristic_NegamaxAlphaBetaPlayer(player_ids, player_max_depth_[p], player_random_[p]);
      case Game::PLY_WEIGHT_NEGAMAX:
        return new WeightHeuristic_NegamaxPlayer(player_ids, player_max_depth_[p], player_wh_[p].data(), player_random_[p]);
      case Game::PLY_WEIGHT_ALPHABETA:
        return new WeightHeuristic_NegamaxAlphaBetaPlayer(player_ids, player_max_depth_[p], player_wh_[p].data(), player_random_[p]);
      default:
        return NULL;
    }
  }
  void Play() {
    std::ofstream of;
    if (FLAGS_o != "") {
      of.open(FLAGS_o);
      CHECK(of.is_open()) << "File \"" << FLAGS_o << "\" could not been opened.";
    }
    Winner win;
    while (!board_.CheckFull() && win.player == Winner::NONE) {
      Player* curr_player = players_[curr_player_];
      const Player* next_player = players_[(curr_player_ + 1) % 2];
      uint32_t move = curr_player->Move(board_);
      if(!board_.Move(move, curr_player->Id())) {
        std::cout << "Player " << curr_player->Id() <<
            " tried to do a invalid movement ("<< move << "). This is like cheating!"
            " Player " << next_player->Id() << " wins!" << std::endl;
        return;
      }
      win = board_.CheckWinner();
      if (FLAGS_o == "") { std::cout << board_ << std::endl; }
      else { of << board_ << std::endl; }
      curr_player_ = (curr_player_ + 1) % 2;
    }
    of.close();
    if (win.player != Winner::NONE) {
      std::cout << "Player " << win.player << " wins! Winning cells are "
                << win.cells[0] << " " << win.cells[1] << " " << win.cells[2]
                << " " << win.cells[3] << std::endl;
    } else {
      std::cout << "Players tie!" << std::endl;
    }
  }
 private:
  Board board_;
  Player* players_[2];
  std::vector<float> player_wh_[2];
  PlayerType player_type_[2];
  size_t player_max_depth_[2];
  bool player_random_[2];
  uint8_t curr_player_;
};


std::default_random_engine PRNG;

int main(int argc, char** argv) {
  // Google tools initialization
  google::InitGoogleLogging(argv[0]);
  google::SetUsageMessage(
      "A Connect Four game based on Minimax with Alpha-Beta prunning.");
  google::ParseCommandLineFlags(&argc, &argv, true);
  // Random seed
  PRNG.seed(FLAGS_seed);
  // Show used options
  LOG(INFO) << "-o " << FLAGS_o;
  LOG(INFO) << "-rows " << FLAGS_rows;
  LOG(INFO) << "-cols " << FLAGS_cols;
  LOG(INFO) << "-seed " << FLAGS_seed;
  LOG(INFO) << "-ai " << FLAGS_ai;
  LOG(INFO) << "-max_depth " << FLAGS_max_depth;
  LOG(INFO) << "-wh " << FLAGS_wh;
  LOG(INFO) << "-random " << FLAGS_random;
  // Play!
  Game game;
  game.Play();
  return 0;
}

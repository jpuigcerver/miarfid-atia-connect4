#include <glog/logging.h>
#include <google/gflags.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstring>
#include <cstdlib>

#include "Board.hpp"
#include "Player.hpp"
#include "Utils.hpp"

DEFINE_string(o, "", "Output filename. Use '-' for stdout");
DEFINE_uint64(r, 6, "Board rows");
DEFINE_uint64(c, 7, "Board columns");
DEFINE_uint64(seed, 0, "Random seed");
DEFINE_string(ai, "Human:Human", "Valid intelligences: Human | Random | Minimax00 | Minimax01");
DEFINE_string(max_depth, "5:5", "Max. depth for Minimax algorithm");
DEFINE_string(h01_w, "1;5;100;-2;-6;-200:1;5;100;-2;-6;-200", "Weight values for heuristic 3");

class Game {
 public:
  typedef enum {PLY_HUMAN, PLY_RANDOM, PLY_MINIMAX_HEUR00, PLY_MINIMAX_HEUR01} PlayerType;
  Game() : board_(Board(FLAGS_c, FLAGS_r)), curr_player_(0) {
    // Parse AI type from arguments
    std::string player_types_str[2];
    splitStrIntoTwoStr(FLAGS_ai, player_types_str);
    player_type_[0] = getPlayerTypeFromString(player_types_str[0]);
    player_type_[1] = getPlayerTypeFromString(player_types_str[1]);
    // Parse Max-Depth from arguments
    splitStrIntoTwoSize_t(FLAGS_max_depth, player_max_depth_);
    // Parse heuristic 3 options
    splitStrIntoTwoFloatLists(FLAGS_h01_w, player_h01_w_);
    CHECK_EQ(player_h01_w_[0].size(), 6);
    CHECK_EQ(player_h01_w_[1].size(), 6);

    players_[0] = createPlayer(0, 'O', 'X');
    players_[1] = createPlayer(1, 'X', 'O');
  }
  ~Game() {
    delete players_[0];
    delete players_[1];
  }
  static void splitStrIntoTwoFloatLists(const std::string& str, std::vector<float>* arr) {
    size_t p = str.find(':');
    if ( p == std::string::npos) {
      LOG(FATAL) << "Bad option format. Expected format: float_list:float_list";
    }
    std::string p1 = str.substr(0, p);
    std::string p2 = str.substr(p + 1);
    parseFloatList(p1.c_str(), &arr[0]);
    parseFloatList(p1.c_str(), &arr[1]);
  }
  static void splitStrIntoTwoFloat(const std::string& str, float* arr) {
    size_t p = str.find(':');
    if (p == std::string::npos) {
      LOG(FATAL) << "Bad option format. Expected format: float:float";
    }
    arr[0] = strtof(str.substr(0, p).c_str(), NULL);
    arr[1] = strtof(str.substr(p + 1).c_str(), NULL);
  }
  static void splitStrIntoTwoSize_t(const std::string& str, size_t* arr) {
    size_t p = str.find(':');
    if (p == std::string::npos) {
      LOG(FATAL) << "Bad option format. Expected format: uint64:uint64";
    }
    arr[0] = strtoul(str.substr(0, p).c_str(), NULL, 10);
    arr[1] = strtoul(str.substr(p + 1).c_str(), NULL, 10);
  }
  static void splitStrIntoTwoStr(const std::string& str, std::string* arr) {
    size_t p = str.find(':');
    if (p == std::string::npos) {
      LOG(FATAL) << "Bad option format. Expected format: string:string";
    }
    arr[0] = str.substr(0, p);
    arr[1] = str.substr(p + 1);
  }
  static PlayerType getPlayerTypeFromString(const std::string& str) {
    if (str == "Human") {
      return Game::PLY_HUMAN;
    } else if (str == "Random") {
      return Game::PLY_RANDOM;
    } else if (str == "Minimax00") {
      return Game::PLY_MINIMAX_HEUR00;
    } else if (str == "Minimax01") {
      return Game::PLY_MINIMAX_HEUR01;
    } else {
      LOG(WARNING) << "Wrong player type: \"" << str << "\". Using Human.";
      return Game::PLY_HUMAN;
    }
  }
  Player* createPlayer(const uint8_t p, const uint8_t my_id, const uint8_t en_id) {
    const uint8_t player_ids[2] = {my_id, en_id};
    switch(player_type_[p]) {
      case PLY_HUMAN:
        return new HumanPlayer(player_ids);
      case PLY_RANDOM:
        return new RandomPlayer(player_ids);
      case PLY_MINIMAX_HEUR00:
        return new Heuristic00_MinimaxPlayer(player_ids, player_max_depth_[p]);
      case PLY_MINIMAX_HEUR01:
        return new Heuristic01_MinimaxPlayer(player_ids, player_max_depth_[p], player_h01_w_[p].data());
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
    while (!board_.CheckFull()) {
      Player* curr_player = players_[curr_player_];
      const Player* next_player = players_[(curr_player_ + 1) % 2];
      uint32_t move = curr_player->Move(board_);
      if(!board_.Move(move, curr_player->Id())) {
        std::cout << "Player " << curr_player->Id() <<
            " tried to do a invalid movement ("<< move << "). This is like cheating!"
            " Player " << next_player->Id() << " wins!" << std::endl;
        return;
      }
      Winner win = board_.CheckWinner();
      if (FLAGS_o == "") { std::cout << board_ << std::endl; }
      else { of << board_ << std::endl; }
      if (win.player != Winner::NONE) {
        std::cout << "Player " << win.player << " wins! Winning cells are "
                  << win.cells[0] << " " << win.cells[1] << " " << win.cells[2]
                  << " " << win.cells[3] << std::endl;
        break;
      }
      curr_player_ = (curr_player_ + 1) % 2;
    }
    of.close();
  }
 private:
  Board board_;
  Player* players_[2];
  std::vector<float> player_h01_w_[2];
  PlayerType player_type_[2];
  size_t player_max_depth_[2];
  uint8_t curr_player_;
};



#include <random>
std::default_random_engine PRNG;



int main(int argc, char** argv) {
  // Google tools initialization
  google::InitGoogleLogging(argv[0]);
  google::SetUsageMessage(
      "A Connect Four game based on Minimax with Alpha-Beta prunning.");
  google::ParseCommandLineFlags(&argc, &argv, true);
  // Random seed
  PRNG.seed(FLAGS_seed);

  Game game;
  game.Play();
  return 0;
}

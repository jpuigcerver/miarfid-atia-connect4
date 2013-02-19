#include <iostream>
#include <stdint.h>
#include <string.h>
#include <glog/logging.h>
#include <google/gflags.h>

#include "Board.hpp"
#include "Player.hpp"

using namespace std;

class Game {
 public:
  typedef enum {MODE_HH, MODE_HC, MODE_CC} PlayerMode;
  Game(const uint16_t cols, const uint16_t rows, PlayerMode player_mode)
      : board_(Board(cols, rows)), player_mode_(player_mode), curr_player_(0) {
    if (player_mode_ == MODE_HH) {
      players_[0] = new HumanPlayer('1');
      players_[1] = new HumanPlayer('2');
    } else if (player_mode_ == MODE_HC) {
      players_[0] = new HumanPlayer('1');
      players_[1] = new ComputerPlayer('2');
    } else if (player_mode_ == MODE_CC) {
      players_[0] = new ComputerPlayer('1');
      players_[1] = new ComputerPlayer('2');
    }
  }
  ~Game() {
    delete players_[0];
    delete players_[1];
  }
  void Play() {
    while (!board_.CheckFull()) {
      Player* curr_player = players_[curr_player_];
      const Player* next_player = players_[(curr_player_ + 1) % 2];
      uint8_t col = curr_player->Move(board_);
      if(!board_.Put(col, curr_player->Id())) {
        LOG(INFO) << "Player " << curr_player->Id() <<
            " tried to do a invalid movement. This is like cheating!"
            " Player " << next_player->Id() << " wins!";
        return;
      }
      curr_player_ = (curr_player_ + 1) % 2;
    }
  }
 private:
  Board board_;
  PlayerMode player_mode_;
  uint8_t curr_player_;
  Player* players_[2];
};

DEFINE_string(players, "HvsH", "Players mode. Human vs. Human (HvsH), "
              "Human vs. Computer (HvsC), Computer vs. Computer (CvsC)");

int main(int argc, char** argv) {
  // Google tools initialization
  google::InitGoogleLogging(argv[0]);
  google::SetUsageMessage(
      "A Connect Four game based on Minimax with Alpha-Beta prunning.");
  google::ParseCommandLineFlags(&argc, &argv, true);
  Game::PlayerMode player_mode;
  if (FLAGS_players == "HvsH") {
    player_mode = Game::MODE_HH;
  } else if (FLAGS_players == "HvsC") {
    player_mode = Game::MODE_HC;
  } else if (FLAGS_players == "CvsC") {
    player_mode = Game::MODE_CC;
  } else {
    cout << "Wrong player mode: " << FLAGS_players << endl;
    return 1;
  }
  Game game(7, 6, player_mode);
  //game.Play();
  LOG(INFO) << "hola";
  return 0;
}

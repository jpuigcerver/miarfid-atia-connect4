#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <glog/logging.h>
#include <stdint.h>
#include <iostream>
#include "Board.hpp"

class Player {
 protected:
  const uint8_t id_;
 public:
  Player(const uint8_t id) : id_(id) {}
  virtual ~Player() {};
  virtual uint8_t Id() const { return id_; }
  virtual uint16_t Move(const Board& b) = 0;
};

class HumanPlayer : public Player {
 public:
  HumanPlayer(const uint8_t id) : Player(id) {}
  virtual uint16_t Move(const Board& b) {
    std::cout << "Current board: " << std::endl << b;
    uint16_t col = 0;
    do {
      std::cout << "Player " << id_ << ", choose column (0 - "
                << b.Cols() - 1 << "): ";
      std::cin >> col;
    } while (col >= b.Cols() && b.Height(col) >= b.Rows());
    return col;
  }
};

class ComputerPlayer : public Player {
 public:
  ComputerPlayer(const uint8_t id) : Player(id) {}
  virtual uint16_t Move(const Board& b) {
    return 0;
  }
};

#endif  // PLAYER_HPP_

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "Board.hpp"
#include "Node.hpp"

#include <stdint.h>

class Player {
 protected:
  uint8_t player_ids_[2];
 public:
  Player(const uint8_t player_ids[2]);
  virtual ~Player() {};
  virtual uint8_t Id() const;
  virtual uint32_t Move(const Board& b) = 0;
};

class HumanPlayer : public Player {
 public:
  HumanPlayer(const uint8_t player_ids[2]);
  virtual uint32_t Move(const Board& b);
};

class RandomPlayer : public Player {
 public:
  RandomPlayer(const uint8_t player_ids[2]);
  virtual uint32_t Move(const Board& b);
};

class MinimaxPlayer : public Player {
 public:
  MinimaxPlayer(const uint8_t player_ids[2], const size_t max_depth);
  virtual ~MinimaxPlayer();
  virtual size_t NumChildren() const;
  virtual uint32_t Move(const Board& b);
 private:
  Node* root_;
  const size_t max_depth_;
  virtual void Expand(const Board& init_board);
  virtual Node* CreateRoot(const Board& init_board) const = 0;
};

class Heuristic00_MinimaxPlayer : public MinimaxPlayer {
 public:
  Heuristic00_MinimaxPlayer(
      const uint8_t player_ids[2], const size_t max_depth);
 private:
  virtual Node* CreateRoot(const Board& init_board) const;
};

class Heuristic01_MinimaxPlayer : public MinimaxPlayer {
 public:
  Heuristic01_MinimaxPlayer(
      const uint8_t player_ids[2], const size_t max_depth,
      const float weights[6]);
 private:
  const float weights_[6];
  virtual Node* CreateRoot(const Board& init_board) const;
};

#endif  // PLAYER_HPP_

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "Board.hpp"
#include "Heuristic.hpp"
#include "Negamax.hpp"

#include <stdint.h>

class Player {
 protected:
  const uint8_t player_ids_[2];
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

template <class Heuristic>
class NegamaxPlayer : public Player {
 public:
  NegamaxPlayer(const uint8_t player_ids[2], const size_t max_depth,
                const Heuristic& heur, const bool shuffle);
  virtual uint32_t Move(const Board& b);
 private:
  const size_t max_depth_;
  const Heuristic heuristic_;
  const bool shuff_;
};

template <class Heuristic>
class NegamaxAlphaBetaPlayer : public Player {
 public:
  NegamaxAlphaBetaPlayer(const uint8_t player_ids[2], const size_t max_depth,
                         const Heuristic& heur, const bool shuffle);
  virtual uint32_t Move(const Board& b);
 private:
  const size_t max_depth_;
  const Heuristic heuristic_;
  const bool shuff_;
};

class SimpleHeuristic_NegamaxPlayer : public NegamaxPlayer<SimpleHeuristic> {
 public:
  SimpleHeuristic_NegamaxPlayer(
      const uint8_t player_ids[2], const size_t max_depth, const bool shuffle);
};

class WeightHeuristic_NegamaxPlayer : public NegamaxPlayer<WeightHeuristic> {
 public:
  WeightHeuristic_NegamaxPlayer(
      const uint8_t player_ids[2], const size_t max_depth,
      const float weights[6], const bool shuffle);
};

class SimpleHeuristic_NegamaxAlphaBetaPlayer :
    public NegamaxAlphaBetaPlayer<SimpleHeuristic> {
 public:
  SimpleHeuristic_NegamaxAlphaBetaPlayer(
      const uint8_t player_ids[2], const size_t max_depth, const bool shuffle);
};

class WeightHeuristic_NegamaxAlphaBetaPlayer :
    public NegamaxAlphaBetaPlayer<WeightHeuristic> {
 public:
  WeightHeuristic_NegamaxAlphaBetaPlayer(
      const uint8_t player_ids[2], const size_t max_depth,
      const float weights[6], const bool shuffle);
};

class NetworkPlayer : public Player {
 public:
  NetworkPlayer(const uint8_t player_ids[2], const int fd);
  virtual uint32_t Move(const Board& b);
 private:
  const int sockfd;
};


#endif  // PLAYER_HPP_

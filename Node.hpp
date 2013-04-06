#ifndef NODE_HPP_
#define NODE_HPP_

#include "Board.hpp"
#include <iostream>
#include <list>
#include <utility>
#include <string.h>

class Node {
 public:
  Node(const Board& board, const uint8_t ids[2], const size_t depth);
  virtual size_t NumChildren() const;
  virtual Node* CreateNode(
      const Board& board, const uint8_t ids[2], const size_t depth) const = 0;
  virtual void ComputeHeuristic() = 0;
  virtual std::pair<uint32_t, float> BestMovement() const;
  virtual void ExpandMinimax(const size_t depth);
  virtual void ExpandAlphaBeta(const size_t depth, float alpha, float beta);
 protected:
  typedef std::list<std::pair<Node*,uint32_t> > ChildrenList;
  Board board_;
  float score_;
  size_t best_m_;
  size_t num_children_;
  uint8_t player_ids_[2];
  const Winner winner_;
  const size_t depth_;
};

class Heuristic00_Node : public Node {
 public:
  Heuristic00_Node(
      const Board& board, const uint8_t ids[2], const size_t depth);
  virtual Node* CreateNode(
      const Board& board, const uint8_t ids[2], const size_t depth) const;
  virtual void ComputeHeuristic();
};

class Heuristic01_Node : public Node {
 public:
  Heuristic01_Node(
      const Board& board, const uint8_t ids[2], const size_t depth,
      const float weights[6]);
  virtual Node* CreateNode(
      const Board& board, const uint8_t ids[2], const size_t depth) const;
  virtual void ComputeHeuristic();
 private:
  float weights_[6];
  float LineHeuristic(const uint8_t line[4]) const;
};

#endif  // NODE_HPP_

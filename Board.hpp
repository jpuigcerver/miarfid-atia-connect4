#ifndef BOARD_HPP_
#define BOARD_HPP_

#include "Coord.hpp"
#include "Winner.hpp"

#include <glog/logging.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <utility>

class Board {
 public:
  Board(const uint16_t cols, const uint16_t rows);
  Board(const Board& board);
  virtual ~Board();
  virtual Board& operator = (const Board& other);
  virtual bool operator == (const Board& other) const;
  virtual bool Move(const uint32_t move_id, const uint8_t p);
  virtual Winner CheckWinner() const;
  virtual std::vector<std::pair<uint32_t,Board> > Expand(const uint8_t player) const;
  virtual void Serialize(char** buff, size_t* size) const;
  virtual bool Deserialize(const char* buff, const size_t size);
  virtual bool CheckFull() const;
  void Print(std::ostream& os, const size_t sp) const;
  inline uint16_t Cols() const { return cols_; }
  inline uint16_t Rows() const { return rows_; }
  inline uint16_t Height(const uint16_t col) const {
    CHECK_LT(col, cols_);
    return height_[col];
  }
  inline uint8_t Get(const uint16_t col, const uint16_t row) const {
    CHECK_LT(col, cols_); CHECK_LT(row, rows_);
    return board_[col + row * cols_];
  }
  friend std::ostream& operator << (std::ostream& os, const Board& b) {
    b.Print(os, 0);
    return os;
  }
 private:
  uint16_t cols_;
  uint16_t rows_;
  uint8_t* board_;
  uint16_t* height_;
};

#endif  // BOARD_HPP_

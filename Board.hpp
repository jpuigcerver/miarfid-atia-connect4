#ifndef BOARD_HPP_
#define BOARD_HPP_

#include <glog/logging.h>
#include <stdint.h>
#include <iostream>
#include <list>
#include <utility>

struct Coord {
  const uint16_t c;
  const uint16_t r;
  Coord() : c(c), r(r) {}
  Coord(const uint16_t c, const uint16_t r) : c(c), r(r) {}
  friend std::ostream& operator << (std::ostream& os, const Coord& c) {
    os << "(" << c.c << "," << c.r << ")";
    return os;
  }
};

struct Winner {
  static const uint8_t NONE;
  const Coord cells[4];
  const uint8_t player;
  Winner() : player(NONE) {}
  Winner(const uint8_t p, const Coord c[4])
      : cells{c[0], c[1], c[2], c[3]}, player(p) {}
};

class Board {
 public:
  Board(const uint16_t cols, const uint16_t rows);
  Board(const Board& board);
  virtual ~Board();
  virtual bool operator == (const Board& other) const;
  virtual bool Move(const uint32_t move_id, const uint8_t p);
  //virtual Winner CheckWinnerMovement(const uint32_t move_id) const;
  virtual Winner CheckWinner() const;
  virtual std::list<std::pair<uint32_t,Board> > Expand(const uint8_t player) const;
  bool CheckFull() const;
  inline uint8_t Get(const uint16_t col, const uint16_t row) const {
    CHECK_LT(col, cols_); CHECK_LT(row, rows_);
    return board_[col + row * cols_];
  }
  inline uint16_t Cols() const { return cols_; }
  inline uint16_t Rows() const { return rows_; }
  inline uint16_t Height(const uint16_t col) const { return height_[col]; }
  void Print(std::ostream& os, const size_t sp) const {
    for (size_t s = 0; s < sp; ++s) os << ' ';
    for (uint16_t c = 0; c < cols_; ++c)
      os << '-';
    os << std::endl;
    for (uint16_t r1 = rows_; r1 > 0; --r1) {
      const uint16_t r = r1 - 1;
      for (size_t s = 0; s < sp; ++s) os << ' ';
      for (uint16_t c = 0; c < cols_; ++c) {
        const uint8_t p = Get(c, r);
        os << p;
      }
      os << std::endl;
    }
    for (size_t s = 0; s < sp; ++s) os << ' ';
    for (uint16_t c = 0; c < cols_; ++c)
      os << '-';
    os << std::endl;
  }
  friend std::ostream& operator << (std::ostream& os, const Board& b) {
    b.Print(os, 0);
    return os;
  }
 private:
  const uint16_t cols_;
  const uint16_t rows_;
  uint8_t* board_;
  uint16_t* height_;
};

#endif  // BOARD_HPP_

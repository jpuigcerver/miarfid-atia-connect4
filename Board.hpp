#ifndef BOARD_HPP_
#define BOARD_HPP_

#include <glog/logging.h>
#include <stdint.h>
#include <string.h>

struct Winner {
  typedef enum {NONE = 0, HORIZONTAL, VERTICAL,
                RIGHT_DIAG, LEFT_DIAG} WinnerDirection;
  const WinnerDirection direction;
  const uint16_t column;
  const uint8_t player;
  Winner()
      : direction(NONE), column(0), player(0) { }
  Winner(const WinnerDirection d, const uint16_t c, const uint8_t p)    
      : direction(d), column(c), player(p) { }
};

class Board {
 public:
  Board(const uint16_t cols, const uint16_t rows)
      : cols_(cols), rows_(rows), board_(new uint8_t [cols_ * rows_]),
        height_(new uint16_t [cols_]) {
    memset(board_, ' ', sizeof(uint8_t) * cols_ * rows_);
    memset(height_, 0x00, sizeof(uint16_t) * cols_);
    // LOG
    /*LOG(INFO) << "Board created. Cols = " << cols_
      << ", Rows = " << rows_;*/
  }
  bool Put(const uint16_t col, const uint8_t p) {
    if (col >= cols_) {
      // WRONG COLUMN
      //LOG(INFO) << "Player " << p << " picked a wrong column.";
      return false;
    }
    if (height_[col] >= rows_) {
      // MAX HEIGHT REACHED
      //LOG(INFO) << "Player " << p << " chose a filled column.";
      return false;
    }
    /*LOG(INFO) << "Player " << p << " put a disc at column "
      << col << ".";*/
    const uint16_t row = height_[col];
    const uint32_t idx = col + row * cols_;
    board_[idx] = p;
    ++height_[col];
    return true;
  }
  inline uint8_t Get(const uint16_t col, const uint16_t row) const {
    return board_[col + row * cols_];
  }
  Winner CheckColumnWinner(const uint16_t col) const {
    if (height_[col] == 0) {
      return Winner();
    }
    const uint16_t row = height_[col] - 1;
    const uint8_t p = Get(col, row);
    // HORIZONTAL CONNECT, STARTING AT COLUMN col
    if (col < cols_ - 4 && Get(col + 1, row) == p &&
        Get(col + 2, row) == p && Get(col + 3, row) == p) {
      return Winner(Winner::HORIZONTAL, col, p);
    }
    // VERCTICAL CONNECT, STARTING AT COLUMN col
    if (row >= 3 && Get(col, row - 1) == p &&
        Get(col, row - 2) == p && Get(col, row - 3) == p) {
      return Winner(Winner::VERTICAL, col, p);
    }
    // RIGHT-DIAGONAL CONNECT, STARTING AT COLUMN col
    if (col < cols_ - 4 && row >= 3 && Get(col + 1, row - 1) == p &&
        Get(col + 2, row - 2) == p && Get(col + 3, row - 3) == p) {
      return Winner(Winner::RIGHT_DIAG, col, p);
    }
    // LEFT-DIAGONAL CONNECT, STARTING AT COLUMN col
    if (col >= 3 && row >= 3 && Get(col - 1, row - 1) == p &&
        Get(col - 2, row - 2) == p && Get(col - 3, row - 3) == p) {
      return Winner(Winner::LEFT_DIAG, col, p);
    }
    // No winner
    return Winner();
  }
  Winner CheckWinner() const {
    for (uint16_t col = 0; col < cols_; ++col) {
      Winner w = CheckColumnWinner(col);
      if (w.direction != Winner::NONE) {
        return w;
      }
    }
    return Winner();
  }
  bool CheckFull() const {
    for (uint16_t col = 0; col < cols_; ++col) {
      if (height_[col] < rows_) return false;
    }
    return true;
  }
  inline uint16_t Cols() const { return cols_; }
  inline uint16_t Rows() const { return rows_; }
  inline uint16_t Height(const uint16_t col) const { return height_[col]; }
  friend std::ostream& operator << (std::ostream& os, const Board& b) {
    for (uint16_t r1 = b.rows_; r1 > 0; --r1) {
      const uint16_t r = r1 - 1;
      for (uint16_t c = 0; c < b.cols_; ++c) {
        const uint8_t p = b.Get(c, r);
        os << p;
      }
      os << std::endl;
    }
    return os;
  }
 private:
  const uint16_t cols_;
  const uint16_t rows_;
  uint8_t* board_;
  uint16_t* height_;
};

#endif  // BOARD_HPP_

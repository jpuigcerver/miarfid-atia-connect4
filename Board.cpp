#include "Board.hpp"

#include <glog/logging.h>
#include <string.h>

#include <algorithm>

Board::Board(const uint16_t cols, const uint16_t rows)
    : cols_(cols), rows_(rows), board_(new uint8_t[cols_ * rows_]),
      height_(new uint16_t[cols_]) {
  memset(board_, ' ', sizeof(uint8_t) * cols_ * rows_);
  memset(height_, 0x00, sizeof(uint16_t) * cols_);
}

Board::Board(const Board& board)
    : cols_(board.cols_), rows_(board.rows_),
      board_(new uint8_t[cols_ * rows_]), height_(new uint16_t[cols_]) {
  memcpy(board_, board.board_, sizeof(uint8_t) * cols_ * rows_);
  memcpy(height_, board.height_, sizeof(uint16_t) * cols_);
}

Board::~Board() {
  delete [] board_;
  delete [] height_;
}

Board& Board::operator = (const Board& board) {
  delete [] board_;
  delete [] height_;
  cols_ = board.cols_;
  rows_ = board.rows_;
  board_ = new uint8_t[cols_ * rows_];
  height_ = new uint16_t[cols_];
  memcpy(board_, board.board_, sizeof(uint8_t) * cols_ * rows_);
  memcpy(height_, board.height_, sizeof(uint16_t) * cols_);
  return *this;
}

bool Board::operator == (const Board& other) const {
  return (cols_ == other.cols_ && rows_ == other.rows_ &&
          memcmp(board_, other.board_, sizeof(uint8_t) * cols_ * rows_) == 0);
}

bool Board::Move(const uint32_t move_id, const uint8_t p) {
  if (move_id >= cols_) {
    LOG(INFO) << "Player " << p << " picked a out-of-board column";
    return false;
  }
  const uint16_t col = move_id;
  if (height_[col] >= rows_) {
    LOG(INFO) << "Player " << p << " chose a filled column";
    return false;
  }
  DLOG(INFO) << "Player " << p << " put a disc at column " << col;
  const uint16_t row = height_[col];
  const uint32_t idx = col + row * cols_;
  board_[idx] = p;
  ++height_[col];
  return true;
}

Winner Board::CheckWinner() const {
  for (uint16_t col = 0; col < cols_; ++col) {
    for (uint16_t row = 0; row < height_[col]; ++row) {
      uint8_t p = Get(col, row);
      // VERTICAL
      if (row + 3 < height_[col] && Get(col, row + 1) == p &&
          Get(col, row + 2) == p && Get(col, row + 3) == p) {
        Coord coords[] = {Coord(col, row), Coord(col, row + 1),
                          Coord(col, row + 2), Coord(col, row + 3)};
        return Winner{p, coords};
      }
      // HORIZONTAL
      if (col + 3 < cols_ && Get(col + 1, row) == p &&
          Get(col + 2, row) == p && Get(col + 3, row) == p) {
        Coord coords[] = {Coord(col, row), Coord(col + 1, row),
                          Coord(col + 2, row), Coord(col + 3, row)};
        return Winner(p, coords);
      }
      // RIGHT-LEFT DIAGONAL
      if (col >= 3 && row + 3 < height_[col - 3] &&
          Get(col - 1, row + 1) == p && Get(col - 2, row + 2) == p &&
          Get(col - 3, row + 3) == p) {
        Coord coords[] = {Coord(col, row), Coord(col - 1, row + 1),
                          Coord(col - 2, row + 2), Coord(col - 3, row + 3)};
        return Winner(p, coords);
      }
      // LEFT-RIGHT DIAGONAL
      if (col + 3 < cols_ && row + 3 < height_[col + 3] &&
          Get(col + 1, row + 1) == p && Get(col + 2, row + 2) == p &&
          Get(col + 3, row + 3) == p) {
        Coord coords[] = {Coord(col, row), Coord(col + 1, row + 1),
                          Coord(col + 2, row + 2), Coord(col + 3, row + 3)};
        return Winner(p, coords);
      }
    }
  }
  return Winner();
}

bool Board::CheckFull() const {
  for (uint16_t col = 0; col < cols_; ++col) {
    if (height_[col] < rows_) return false;
  }
  return true;
}

std::vector<std::pair<uint32_t, Board> > Board::Expand(
    const uint8_t player) const {
  std::vector<std::pair<uint32_t, Board> > children;
  for (uint16_t col = 0; col < cols_; ++col) {
    if (height_[col] >= rows_) continue;
    Board ch(*this);
    ch.Move(col, player);
    children.push_back(std::pair<uint32_t, Board>(col, ch));
  }
  return children;
}

void Board::Serialize(char** buff, size_t* size) const {
  CHECK_NOTNULL(buff); CHECK_NOTNULL(size);
  *size = cols_ * rows_ * sizeof(uint8_t) + 2 * sizeof(uint16_t);
  *buff = new char[*size];
  char* cols_pos = *buff;
  char* rows_pos = cols_pos + sizeof(uint16_t);
  char* board_pos = rows_pos + sizeof(uint16_t);
  memcpy(cols_pos, (char*)&cols_, sizeof(uint16_t));
  memcpy(rows_pos, (char*)&rows_, sizeof(uint16_t));
  memcpy(board_pos, (char*)board_, cols_ * rows_ * sizeof(uint8_t));
}

bool Board::Deserialize(const char* buff, const size_t size) {
  CHECK_NOTNULL(buff);
  if (size < 2 * sizeof(uint16_t)) return false;
  const char* cols_pos = buff;
  const char* rows_pos = cols_pos + sizeof(uint16_t);
  const char* board_pos = rows_pos + sizeof(uint16_t);
  memcpy((char*)(&cols_), cols_pos, sizeof(uint16_t));
  memcpy((char*)(&rows_), rows_pos, sizeof(uint16_t));
  if (cols_ == 0 || rows_ == 0) return false;
  const size_t exp_size = cols_ * rows_ * sizeof(uint8_t) +
      2 * sizeof(uint16_t);
  if (size != exp_size) return false;
  height_ = new uint16_t[cols_];
  board_ = new uint8_t[cols_ * rows_];
  memcpy((char*)board_, board_pos, cols_ * rows_ * sizeof(uint16_t));
  for (uint16_t c = 0; c < cols_; ++c) {
    height_[c] = 0;
    for (uint16_t r = 0; r < rows_ && board_[c + r * cols_] != ' ';
         ++r, ++height_[c]);
  }
  return true;
}

void Board::Print(std::ostream& os, const size_t sp) const {
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

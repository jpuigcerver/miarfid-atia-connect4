#include <glog/logging.h>
#include <google/gflags.h>
#include <algorithm>
#include <iostream>
#include <random>

#include "Board.hpp"
#include "Player.hpp"
#include "Utils.hpp"

std::default_random_engine PRNG;

DEFINE_uint64(seed, 0, "Random seed");
DEFINE_uint64(generations, 1000, "Number of generations");
DEFINE_uint64(population, 1000, "Population size");
DEFINE_uint64(max_depth, 4, "Max depth");
DEFINE_double(mutation, 0.10, "Mutation probability");
DEFINE_double(crossover, 0.80, "Crossover probability");
DEFINE_double(face_ratio, 0.10, "Face ratio");
DEFINE_uint64(nbest, 3, "N-best");

struct Badness {
  float lost;
  float rounds;
  Badness() : lost(0), rounds(0.0f) {}
  Badness(float l, float r) : lost(l), rounds(r) {}
  bool operator < (const Badness& o) const {
    return (lost < o.lost || (lost == o.lost && rounds > o.rounds));
  }
  bool operator == (const Badness& o) const {
    return (lost == o.lost && rounds == o.rounds);
  }
  bool operator != (const Badness& o) const {
    return (lost != o.lost || rounds != o.rounds);
  }
  friend std::ostream& operator << (std::ostream& os, const Badness& b) {
    os << "(" << b.lost << ", " << b.rounds << ")";
    return os;
  }
};

uint32_t f2i(float f) {
  const uint32_t* pf = (const uint32_t*)(&f);
  return *pf;
}

class Individual {
 public:
  static void Crossover(Individual* a, Individual* b) {
    CHECK_NOTNULL(a); CHECK_NOTNULL(b);
    // Choose cross-over position uniformly
    std::uniform_int_distribution<size_t> udist(1, 5);
    const size_t cross_pos = udist(PRNG);
    // Do cross-over
    for (size_t i = 0; i < cross_pos; ++i) {
      std::swap(a->w[i], b->w[i]);
    }
    for (size_t i = cross_pos; i < 6; ++i) {
      std::swap(a->w[i], b->w[i]);
    }
    a->ComputeLength();
    b->ComputeLength();
  }
  static void Mutation(Individual* a) {
    CHECK_NOTNULL(a);
    // Distribution choosing which weight is mutated
    std::uniform_int_distribution<size_t> mutp_dist(0, 5);
    // Distribution choosing which bit is mutated (sign bit excluded)
    std::uniform_int_distribution<size_t> bitp_dist(0, 30);
    // Distribution choosing which value will take the mutated bit
    std::uniform_int_distribution<size_t> bitv_dist(0, 1);
    const size_t mut_pos = mutp_dist(PRNG);
    const size_t bit_pos = bitp_dist(PRNG);
    a->w[mut_pos] ^= (0x01 << bit_pos);
    /*uint32_t* fltp = (uint32_t*)(a->w + mut_pos);
    *fltp ^= 0x01 << bit_pos;*/
    a->ComputeLength();
  }
  void Randomize() {
    std::uniform_int_distribution<size_t> udist(0, ~0);
    for (size_t i = 0; i < 3; ++i) {
      /*uint32_t* wp = (uint32_t*)(w + i);
      *wp = udist(PRNG) & 0x7FFFFFFF;*/
      w[i] = udist(PRNG) & 0x7FFFFFFF;
    }
    for (size_t i = 3; i < 6; ++i) {
      /*uint32_t* wp = (uint32_t*)(w + i);
      *wp = udist(PRNG) | 0x80000000;*/
      w[i] = udist(PRNG) | 0x80000000;
    }
    ComputeLength();
  }
  bool IsFinite() const {
    return true;
    /*return std::isfinite(w[0]) && std::isfinite(w[1]) && std::isfinite(w[2]) &&
      std::isfinite(w[3]) && std::isfinite(w[4]) && std::isfinite(w[5]);*/
  }
  friend std::ostream& operator << (std::ostream& os, const Individual& i) {
    os << "(" << i.w[0] << ", " << i.w[1] << ", " << i.w[2]
       << ", " << i.w[3] << ", " << i.w[4] << ", " << i.w[5] << ")";
    return os;
  }
  /*friend std::ostream& operator << (std::ostream& os, const Individual& i) {
    const uint32_t* iw = (const uint32_t*)(i.w);
    os << std::hex << "(" << iw[0] << ", " << iw[1] << ", " << iw[2]
       << ", " << iw[3] << ", " << iw[4] << ", " << iw[5] << ")";
    return os;
  }*/
  bool operator < (const Individual& o) const {
    return l < o.l;
  }
  bool operator == (const Individual& o) const {
    return w[0] == o.w[0] && w[1] == o.w[1] && w[2] == o.w[2] &&
        w[3] == o.w[3] && w[4] == o.w[4] && w[5] == o.w[5];
  }
  bool operator != (const Individual& o) const {
    return w[0] != o.w[0] || w[1] != o.w[1] || w[2] != o.w[2] ||
        w[3] != o.w[3] || w[4] != o.w[4] || w[5] != o.w[5];
  }
  const int32_t* Weights() const {
    return w;
  }
 private:
  int32_t w[6];
  float l;
  void ComputeLength() {
    /*const int exp0 = ((f2i(w[0]) & 0x7f800000) >> 23) - 127;
    const int exp1 = ((f2i(w[1]) & 0x7f800000) >> 23) - 127;
    const int exp2 = ((f2i(w[2]) & 0x7f800000) >> 23) - 127;
    const int exp3 = ((f2i(w[3]) & 0x7f800000) >> 23) - 127;
    const int exp4 = ((f2i(w[4]) & 0x7f800000) >> 23) - 127;
    const int exp5 = ((f2i(w[5]) & 0x7f800000) >> 23) - 127;
    l = exp0 * exp0 + exp1 * exp1 + exp2 * exp2 +
        exp3 * exp3 + exp4 * exp4 + exp5 * exp5;*/
    l = (float)(w[0] * w[0]) + (float)(w[1] * w[1]) + (float)(w[2] * w[2]) +
        (float)(w[3] * w[3]) + (float)(w[4] * w[4]) + (float)(w[5] * w[5]);
  }
};

void PlayGame(const int32_t wa[6], const int32_t wb[6], const uint16_t cols,
              const uint16_t rows, int* winner, int* round) {
  Board board(cols, rows);
  uint8_t ids[2][2] = {{'O','X'},{'X','O'}};
  const float waf[6] = {(float)wa[0], (float)wa[1], (float)wa[2], (float)wa[3], (float)wa[4], (float)wa[5]};
  const float wbf[6] = {(float)wb[0], (float)wb[1], (float)wb[2], (float)wb[3], (float)wb[4], (float)wb[5]};
  Heuristic01_MinimaxPlayer players[2] = {
    Heuristic01_MinimaxPlayer(ids[0], FLAGS_max_depth, waf),
    Heuristic01_MinimaxPlayer(ids[1], FLAGS_max_depth, wbf)};
  *round = 0;
  *winner = 0;
  size_t curr_player = 0;
  for (; !board.CheckFull(); ++(*round), curr_player = (curr_player + 1) % 2) {
    CHECK(board.Move(players[curr_player].Move(board), players[curr_player].Id()));
    Winner win = board.CheckWinner();
    if (win.player == 'O') { *winner = -1; break; }
    else if (win.player == 'X') { *winner = 1; break; }
  }
}

int main(int argc, char** argv) {
  // Google tools initialization
  google::InitGoogleLogging(argv[0]);
  google::SetUsageMessage(
      "Tool for selecting the best weights");
  google::ParseCommandLineFlags(&argc, &argv, true);
  // Random seed
  PRNG.seed(FLAGS_seed);

  std::uniform_real_distribution<double> dist(0.0, 1.0);
  std::vector<std::pair<Badness,Individual> > population(FLAGS_population);
  std::vector<std::pair<Badness,Individual> > nbest(FLAGS_nbest);
  // Random initialization of population
  for (size_t i = 0; i < FLAGS_population; ++i) {
    population[i].second.Randomize();
  }
  // Initial ranom nbest
  for (size_t i = 0; i < FLAGS_nbest; ++i) {
    nbest[i].second = population[i].second;
  }
  const size_t half_pop = FLAGS_population / 2;
  for (size_t g = 1; g <= FLAGS_generations; ++g) {
    // Perform crossover and mutations
    std::shuffle(population.begin(), population.end(), PRNG);
    for (size_t i = 0; i < half_pop; ++i) {
      if (dist(PRNG) < FLAGS_crossover) {
        Individual::Crossover(
            &population[i].second, &population[i + half_pop].second);
      }
      if (dist(PRNG) < FLAGS_mutation) {
        Individual::Mutation(&population[i].second);
      }
      if (dist(PRNG) < FLAGS_mutation) {
        Individual::Mutation(&population[i + half_pop].second);
      }
    }
    // Filter non-valid individuals
    remove_if(population.begin(), population.end(),
              [] (const std::pair<Badness, Individual>& e) {
                return !e.second.IsFinite();
              });
    // Add previous nbest individuals
    for (size_t i = 0; i < nbest.size(); ++i) {
      population.push_back(nbest[i]);
    }
    // Avoid repeated individuals
    std::sort(population.begin(), population.end());
    std::unique(population.begin(), population.end());
    // Perform evaluation of each individual (old and new)
    for (size_t i = 0; i < population.size(); ++i) {
      float w = 0.0f; float r = 0.0f;
      for (size_t j = 0; j < FLAGS_nbest; ++j) {
        int w0, w1;
        int r0, r1;
        PlayGame(population[i].second.Weights(), nbest[j].second.Weights(), 7, 6, &w0, &r0);
        PlayGame(nbest[j].second.Weights(), population[i].second.Weights(), 7, 6, &w1, &r1);
        w += w0 + w1;
        r += r0 + r1;
      }
      r /= 2.0f * FLAGS_nbest;
      w /= 2.0f * FLAGS_nbest;
      population[i].first = Badness(w, w < 0 ? r : -r);
    }
    // Sort individuals in order of increasing badness
    std::sort(population.begin(), population.end());
    population.resize(FLAGS_population);
    for (size_t i = 0; i < FLAGS_nbest; ++i) {
      nbest[i] = population[i];
      std::cout << i+1 << "-best = " << nbest[i].second << " (" <<  nbest[0].first << ")" << std::endl;
    }
    std::cout << "Generation " << g << " = " << nbest[0].second << " (" << nbest[0].first << ")" << std::endl;
  }
  return 0;
}

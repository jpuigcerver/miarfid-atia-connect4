#include <glog/logging.h>
#include <google/gflags.h>
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>

#include "Board.hpp"
#include "Player.hpp"
#include "Utils.hpp"

#include <thread>
#include <future>

typedef int16_t Wtype;

std::default_random_engine PRNG;

DEFINE_uint64(seed, 0, "Random seed");
DEFINE_uint64(generations, 1000, "Number of generations");
DEFINE_uint64(population, 1000, "Population size");
DEFINE_uint64(max_depth, 4, "Max depth");
DEFINE_double(mutation, 0.02, "Bit mutation probability");
DEFINE_double(crossover, 0.80, "Crossover probability");
DEFINE_uint64(nbest, 5, "N-best");
DEFINE_uint64(nthreads, 1, "Num threads");
DEFINE_uint64(rows, 6, "Board rows");
DEFINE_uint64(cols, 7, "Board columns");
DEFINE_bool(random, true, "Non-deterministic Negamax algorithm");

struct Badness {
  int lost;
  int rounds;
  Badness() : lost(0), rounds(0.0f) {}
  Badness(int l, int r) : lost(l), rounds(r) {}
  bool operator < (const Badness& o) const {
    return (lost < o.lost || (lost == o.lost && rounds < o.rounds));
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
    a->ComputeLength();
    b->ComputeLength();
  }
  static void Mutation(Individual* a, const float p) {
    CHECK_NOTNULL(a);
    // Probability of bit mutation
    std::uniform_real_distribution<float> mut_dist(0.0f, 1.0f);
    for (size_t i = 0; i < 6; ++i) {
      for (size_t j = 0; j < sizeof(Wtype) * 8 - 1; ++j) {
        if (mut_dist(PRNG) < p) {
          a->w[i] ^= (0x01 << j);
        }
      }
    }
    a->ComputeLength();
  }
  void Randomize() {
    std::uniform_int_distribution<uint16_t> udist(0, ~0);
    const Wtype sign_bit = 0x01 << (sizeof(Wtype) * 8 - 1);
    for (size_t i = 0; i < 3; ++i) {
      w[i] = udist(PRNG) & ~sign_bit;
    }
    for (size_t i = 3; i < 6; ++i) {
      w[i] = udist(PRNG) | sign_bit;
    }
    ComputeLength();
  }
  friend std::ostream& operator << (std::ostream& os, const Individual& i) {
    os << "(" << (int)i.w[0] << " " << (int)i.w[1] << " " << (int)i.w[2]
       << " " << (int)i.w[3] << " " << (int)i.w[4] << " " << (int)i.w[5]
       << ")";
    return os;
  }
  /*friend std::ostream& operator << (std::ostream& os, const Individual& i) {
    const uint32_t* iw = (const uint32_t*)(i.w);
    os << std::hex << "(" << iw[0] << ", " << iw[1] << ", " << iw[2]
       << ", " << iw[3] << ", " << iw[4] << ", " << iw[5] << ")";
    return os;
  }*/
  bool WeightsLower(const Individual& o) const {
    for (int i = 0; i < 6; ++i) {
      if (w[i] != o.w[i]) return w[i] < o.w[i];
    }
    return false;
  }
  bool WeightsLengthLower(const Individual& o) const {
    return l < o.l;
  }
  bool operator < (const Individual& o) const {
    return WeightsLengthLower(o);
  }
  bool operator == (const Individual& o) const {
    return w[0] == o.w[0] && w[1] == o.w[1] && w[2] == o.w[2] &&
        w[3] == o.w[3] && w[4] == o.w[4] && w[5] == o.w[5];
  }
  bool operator != (const Individual& o) const {
    return w[0] != o.w[0] || w[1] != o.w[1] || w[2] != o.w[2] ||
        w[3] != o.w[3] || w[4] != o.w[4] || w[5] != o.w[5];
  }
  const Wtype* Weights() const {
    return w;
  }
 private:
  Wtype w[6];
  float l;
  void ComputeLength() {
    l = 0.0f;
    for (int i = 0; i < 6; ++i) {
      const float a = w[i];
      l += a * a;
    }
  }
};

void PlayGame(const Wtype wa[6], const Wtype wb[6], const uint16_t cols,
              const uint16_t rows, int* winner, int* round) {
  Board board(cols, rows);
  uint8_t ids[2][2] = {{'O','X'},{'X','O'}};
  const float waf[6] = {(float)wa[0], (float)wa[1], (float)wa[2], (float)wa[3], (float)wa[4], (float)wa[5]};
  const float wbf[6] = {(float)wb[0], (float)wb[1], (float)wb[2], (float)wb[3], (float)wb[4], (float)wb[5]};
  WeightHeuristic_NegamaxAlphaBetaPlayer players[2] = {
    WeightHeuristic_NegamaxAlphaBetaPlayer(ids[0], FLAGS_max_depth, waf, FLAGS_random),
    WeightHeuristic_NegamaxAlphaBetaPlayer(ids[1], FLAGS_max_depth, wbf, FLAGS_random)};
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
      Individual::Mutation(&population[i].second, FLAGS_mutation);
      Individual::Mutation(&population[i + half_pop].second, FLAGS_mutation);
    }
    // Add previous nbest individuals
    for (size_t i = 0; i < nbest.size(); ++i) {
      population.push_back(nbest[i]);
    }
    // Avoid repeated individuals
    std::sort(population.begin(), population.end(),
              [] (const std::pair<Badness, Individual>& a,
                  const std::pair<Badness, Individual>& b) {
                return a.second.WeightsLower(b.second);
              });
    population.resize(std::distance(population.begin(), std::unique(
        population.begin(), population.end(),
        [] (const std::pair<Badness, Individual>& a,
            const std::pair<Badness, Individual>& b) {
          return a.second == b.second;
        })));
    // Perform evaluation of each individual (old and new)
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    std::vector<std::thread> threads(FLAGS_nthreads);
    for (size_t t = 0; t < FLAGS_nthreads; ++t) {
      threads[t] = std::thread(
          [](std::vector<std::pair<Badness,Individual>>& population,
             const std::vector<std::pair<Badness,Individual>>& nbest,
             const size_t th) {
               for (size_t i = th; i < population.size(); i += FLAGS_nthreads) {
                 int w = 0; int r = 0;
                 for (size_t j = 0; j < FLAGS_nbest; ++j) {
                   int w0 = 0, w1 = 0, r0 = 0, r1 = 0;
                   PlayGame(population[i].second.Weights(), nbest[j].second.Weights(), FLAGS_cols, FLAGS_rows, &w0, &r0);
                   PlayGame(nbest[j].second.Weights(), population[i].second.Weights(), FLAGS_cols, FLAGS_rows, &w1, &r1);
                   w += w0 - w1;
                   r += r0 + r1;
                 }
                 population[i].first = Badness(w, w < 0 ? r : -r);
               }
             }, std::ref(population), std::cref(nbest), t);
    }
    for (size_t t = 0; t < FLAGS_nthreads; ++t) {
      threads[t].join();
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    // Sort individuals in order of increasing badness
    std::sort(population.begin(), population.end());
    population.resize(FLAGS_population);
    for (size_t i = 0; i < FLAGS_nbest; ++i) {
      nbest[i] = population[i];
    }
    std::chrono::duration<float> ts = t2 - t1;
    std::cout << "Generation " << g << " = " << nbest[0].second << " " << nbest[0].first << " (Time = " << ts.count() << ")" << std::endl;
  }
  return 0;
}

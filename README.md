miarfid-atia-connect4
=====================

A Connect Four game based on Negamax with Alpha-Beta prunning.

Author
------
Joan Puigcerver i Pérez (joapuipe@upv.es)

Requirements
------------
- C++ compiler with support for C++11 version (for instance: g++-4.7)
- google-gflags library (https://code.google.com/p/gflags/)
- google-glog library (https://code.google.com/p/google-glog/)

Installation
------------

Once you have installed the required libraries, just type `make` in your
favorite terminal. You may need to adjust the compilation options. Just
edit the Makefile if you need to.

Usage
-----
There are two main binaries that will be built using `make`. 

### connect4
`connect4` is the main program which implements the Connect Four game,
use it to play against an onther human or try to beat my AI. Unfortunately,
`connect4` does not have a GUI, so you will have to play using your keyboard
and (again) your favorite terminal.

```
$ ./connect4 -helpshort
connect4: A Connect Four game based on Minimax with Alpha-Beta prunning.

  Flags from connect4.cpp:
    -ai (Valid intelligences: Human | Random | SimpleNegamax | SimpleAlphaBeta
      | WeightNegamax | WeightAlphaBeta) type: string default: "Human:Human"
    -cols (Board columns) type: uint64 default: 7
    -max_depth (Max. depth for Minimax algorithm) type: string default: "5:5"
    -o (Output filename. Use '-' for stdout) type: string default: ""
    -random (Non-deterministic Negamax algorithm) type: string default: "0:0"
    -rows (Board rows) type: uint64 default: 6
    -seed (Random seed) type: uint64 default: 0
    -wh (Values for weight heuristic) type: string
      default: "4;13;121;-10;-31;-128:4;13;121;-10;-31;-128"
```

### weight_tunning
`weight_tunning` is used to find a good set of parameters for my AI. It runs
a Genetic Algorithm which will play a bunch of games using different heurisitcs,
and will performe some modifications among these heuristics (mutations and
crossovers) and will consider only the best heuristics for the next iteration.
Do this for a couple of hundreds of iterations (a.k.a. generations) with a
big-enough population size and you will get a pretty good paramaters for the 
heuristic. However, I should warn you that this procedure will take A LOT OF 
TIME.

```
$ ./weight_tunning -helpshort
weight_tunning: Tool for selecting the best weights

  Flags from weight_tunning.cpp:
    -cols (Board columns) type: uint64 default: 7
    -crossover (Crossover probability) type: double
      default: 0.80000000000000004
    -generations (Number of generations) type: uint64 default: 1000
    -max_depth (Max depth) type: uint64 default: 4
    -mutation (Bit mutation probability) type: double default: 0.02
    -nbest (N-best) type: uint64 default: 5
    -nthreads (Num threads) type: uint64 default: 1
    -population (Population size) type: uint64 default: 1000
    -random (Non-deterministic Negamax algorithm) type: bool default: true
    -rows (Board rows) type: uint64 default: 6
```

For both programs, you can use the `-help` option to get the full set of
options, but you probably won't need those.
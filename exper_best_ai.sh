#!/bin/bash

set -e
export LC_NUMERIC=C
function rand() {
    od -An -N4 -D /dev/random
}

NAME=(RND Simple WHeur1 WHeur2)
AI=(Random SimpleAlphaBeta WeightAlphaBeta WeightAlphaBeta)
WH=("0 0 0 0 0 0" "0 0 0 0 0 0" "1 10 100 -2 -20 -200" \
    "4 13 121 -10 -31 -128")
MAX_DEPTH=5
REPS=500
NPLAYERS=${#NAME[@]}

R=( );
for i in `seq 0 $[NPLAYERS-1]`; do
    for j in `seq 0 $[NPLAYERS-1]`; do
	R=(${R[@]} 0 0 0);
	n=$[i * NPLAYERS * 3 + j * 3];
	for r in `seq $REPS`; do
	    W=$(./connect4 -ai ${AI[$i]}:${AI[$j]} \
		-wh "${WH[$i]}:${WH[$j]}" \
		-o /dev/null -random 1:1 \
		-max_depth ${MAX_DEPTH}:${MAX_DEPTH} \
		-seed $(rand) | awk '{print $2}')
	    case "$W" in
		O)
		    el=$n; nw=$[${R[$n]} + 1];
		    eval R[$el]=$nw
		    ;;
		X)
		    el=$[n+1]; nw=$[${R[$el]} + 1];
		    eval R[$el]=$nw
		    ;;
		*)
		    el=$[n+2]; nw=$[${R[$el]} + 1];
		    eval R[$el]=$nw
	    esac
	done
	W=$(echo "${R[n]} / $REPS" | bc -l | awk '{printf("%1.2f\n", $1)}')
	L=$(echo "${R[n+1]} / $REPS" | bc -l | awk '{printf("%1.2f\n", $1)}')
	T=$(echo "${R[n+2]} / $REPS" | bc -l | awk '{printf("%1.2f\n", $1)}')
	W2=$(echo "1.96 * sqrt(${R[n]} / $REPS - $W * $W) / sqrt($REPS)" | bc -l)
	L2=$(echo "1.96 * sqrt(${R[n+1]} / $REPS - $L * $L) / sqrt($REPS)" | bc -l)
	T2=$(echo "1.96 * sqrt(${R[n+2]} / $REPS - $T * $T) / sqrt($REPS)" | bc -l)
	W=$(echo "$W * 100" | bc -l); L=$(echo "$L * 100" | bc -l); T=$(echo "$T * 100" | bc -l)
	W2=$(echo "$W2 * 100" | bc -l); L2=$(echo "$L2 * 100" | bc -l); T2=$(echo "$T2 * 100" | bc -l)
	printf "%s vs. %s: %.2f (+-%.2f) %.2f (+-%.2f) %.2f (+-%.2f)\n" ${NAME[$i]} ${NAME[$j]} $W $W2 $L $L2 $T $T2
	n=$[n+1];
    done
done

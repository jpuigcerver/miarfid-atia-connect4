#!/bin/bash

set -e
export LC_NUMERIC=C
function rand() {
    od -An -N4 -D /dev/random
}

NAME=(Simple WHeur1 WHeur2)
AI1=(SimpleAlphaBeta WeightAlphaBeta WeightAlphaBeta)
AI2=(SimpleNegamax WeightNegamax WeightNegamax)
WH=("0 0 0 0 0 0" "1 10 100 -2 -20 -200" "4 13 121 -10 -31 -128")
MAX_DEPTH=5
REPS=500
NPLAYERS=${#NAME[@]}

function game () {
    ai1=$1
    ai2=$2
    na1=$3
    na2=$4
    wh="$5"
    R=(0 0 0 0 0 0);
    R2=(0 0 0 0 0 0);
    for r in `seq $REPS`; do
	./connect4 -ai ${ai1}:${ai2} \
	    -wh "${wh}:${wh}" \
	    -o /dev/null -random 1:1 \
	    -max_depth ${MAX_DEPTH}:${MAX_DEPTH} \
	    -seed $(rand) -logtostderr &> $$.tmp
	RO=( `grep "Player = O:" $$.tmp | grep "Nodes =" | awk -F'[, ]' 'BEGIN{n=0;t=0.0;r=0}
        {sub("sec.", "",$14); n+=$10; t+=$14; ++r;}END{print n, t, r}'` )
	RX=( `grep "Player = X:" $$.tmp | grep "Nodes =" | awk -F'[, ]' 'BEGIN{n=0;t=0.0;r=0}
        {sub("sec.", "",$14); n+=$10; t+=$14; ++r;}END{print n, t, r}'` )
	    #echo ${RO[@]} ${RX[@]}
	eval R[0]=$[${R[0]} + ${RO[0]}]
	eval R[1]=$(python -c "print ${R[1]} + (${RO[1]} * 1000.0)")
	eval R[2]=$[${R[2]} + ${RO[2]}]
	eval R[3]=$[${R[3]} + ${RX[0]}]
	eval R[4]=$(python -c "print ${R[4]} + (${RX[1]} * 1000.0)")
	eval R[5]=$[${R[5]} + ${RX[2]}]
	eval R2[0]=$[${R2[0]} + ${RO[0]} * ${RO[0]}]
	eval R2[1]=$(python -c "print ${R2[1]} + (${RO[1]} * 1000.0 * ${RO[1]} * 1000.0)")
	eval R2[3]=$[${R2[3]} + ${RX[0]} * ${RX[0]}]
	eval R2[4]=$(python -c "print ${R2[4]} + (${RX[1]} * 1000.0 * ${RX[1]}) * 1000.0")
    done
    AVGN_O=$(python -c "print float(${R[0]}) / float(${R[2]})")
    ICFN_O=$(python -c "from math import sqrt; print 1.96 * sqrt(float(${R2[0]}) / float(${R[2]}) - ${AVGN_O} * ${AVGN_O}) / sqrt(${R[2]})")
    AVGT_O=$(python -c "print float(${R[1]}) / float(${R[2]})")
    ICFT_O=$(python -c "from math import sqrt; print 1.96 * sqrt(float(${R2[1]}) / float(${R[2]}) - ${AVGT_O} * ${AVGT_O}) / sqrt(${R[2]})")
    AVGN_X=$(python -c "print float(${R[3]}) / float(${R[5]})")
    ICFN_X=$(python -c "from math import sqrt; print 1.96 * sqrt(float(${R2[3]}) / float(${R[5]}) - ${AVGN_X} * ${AVGN_X}) / sqrt(${R[5]})")
    AVGT_X=$(python -c "print float(${R[4]}) / float(${R[5]})")
    ICFT_X=$(python -c "from math import sqrt; print 1.96 * sqrt(float(${R2[4]}) / float(${R[5]}) - ${AVGT_X} * ${AVGT_X}) / sqrt(${R[5]})")
    printf "%s vs %s: %.2f (+- %.2f) %.2E (+- %.2E) %.2f (+- %.2f) %.2E (+- %.2E)\n" \
	$na1 $na2 $AVGN_O $ICFN_O $AVGT_O $ICFT_O $AVGN_X $ICFN_X $AVGT_X $ICFT_X
}

for i in `seq 0 $[NPLAYERS-1]`; do
    game "${AI1[i]}" "${AI2[i]}" "${NAME[i]}_AlphaBeta" "${NAME[i]}_Negamax" "${WH[i]}"
    game "${AI2[i]}" "${AI1[i]}" "${NAME[i]}_Negamax" "${NAME[i]}_AlphaBeta" "${WH[i]}"
done
rm -f $$.tmp

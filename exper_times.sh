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

R=( ); R2=( );
for i in `seq 0 $[NPLAYERS-1]`; do
    for j in `seq 0 $[NPLAYERS-1]`; do
	R=(${R[@]} 0 0 0 0 0 0);
	R2=(${R2[@]} 0 0 0 0 0 0);
	n=$[i * NPLAYERS * 6 + j * 6];
	for r in `seq $REPS`; do
	    ./connect4 -ai ${AI[$i]}:${AI[$j]} \
		-wh "${WH[$i]}:${WH[$j]}" \
		-o /dev/null -random 1:1 \
		-max_depth ${MAX_DEPTH}:${MAX_DEPTH} \
		-seed $(rand) -logtostderr &> $$.tmp
	    RO=( `grep "Player = O:" $$.tmp | grep "Nodes =" | awk -F'[, ]' 'BEGIN{n=0;t=0.0;r=0}
                  {sub("sec.", "",$14); n+=$10; t+=$14; ++r;}END{print n, t, r}'` )
	    RX=( `grep "Player = X:" $$.tmp | grep "Nodes =" | awk -F'[, ]' 'BEGIN{n=0;t=0.0;r=0}
                  {sub("sec.", "",$14); n+=$10; t+=$14; ++r;}END{print n, t, r}'` )
	    #echo ${RO[@]} ${RX[@]}
	    eval R[$n]=$[${R[$n]} + ${RO[0]}]
	    eval R[$[$n+1]]=$(python -c "print ${R[$[$n+1]]} + (${RO[1]} * 1000.0)")
	    eval R[$[n+2]]=$[${R[$[n+2]]} + ${RO[2]}]
	    eval R[$[n+3]]=$[${R[$[n+3]]} + ${RX[0]}]
	    eval R[$[$n+4]]=$(python -c "print ${R[$[$n+4]]} + (${RX[1]} * 1000.0)")
	    eval R[$[n+5]]=$[${R[$[n+5]]} + ${RX[2]}]
	    eval R2[$n]=$[${R2[$n]} + ${RO[0]} * ${RO[0]}]
	    eval R2[$[$n+1]]=$(python -c "print ${R2[$[$n+1]]} + (${RO[1]} * 1000.0 * ${RO[1]} * 1000.0)")
	    eval R2[$[n+3]]=$[${R2[$[n+3]]} + ${RX[0]} * ${RX[0]}]
	    eval R2[$[$n+4]]=$(python -c "print ${R2[$[$n+4]]} + (${RX[1]} * 1000.0 * ${RX[1]}) * 1000.0")
	done
	AVGN_O=$(python -c "print float(${R[$n]}) / float(${R[$[n+2]]})")
	ICFN_O=$(python -c "from math import sqrt; print 1.96 * sqrt(float(${R2[$n]}) / float(${R[$[n+2]]}) - ${AVGN_O} * ${AVGN_O}) / sqrt(${R[$[n+2]]})")
	AVGT_O=$(python -c "print float(${R[$[n+1]]}) / float(${R[$[n+2]]})")
	ICFT_O=$(python -c "from math import sqrt; print 1.96 * sqrt(float(${R2[$[n+1]]}) / float(${R[$[n+2]]}) - ${AVGT_O} * ${AVGT_O}) / sqrt(${R[$[n+2]]})")
	AVGN_X=$(python -c "print float(${R[$[n+3]]}) / float(${R[$[n+5]]})")
	ICFN_X=$(python -c "from math import sqrt; print 1.96 * sqrt(float(${R2[$[n+3]]}) / float(${R[$[n+5]]}) - ${AVGN_X} * ${AVGN_X}) / sqrt(${R[$[n+5]]})")
	AVGT_X=$(python -c "print float(${R[$[n+4]]}) / float(${R[$[n+5]]})")
	ICFT_X=$(python -c "from math import sqrt; print 1.96 * sqrt(float(${R2[$[n+4]]}) / float(${R[$[n+5]]}) - ${AVGT_X} * ${AVGT_X}) / sqrt(${R[$[n+5]]})")
	#echo ${R[$n]} ${R[$[n+1]]} ${R[$[n+2]]} ${R[$[n+3]]} ${R[$[n+4]]} ${R[$[n+5]]}
	printf "%s vs %s: %.2f (+- %.2f) %.2E (+- %.2E) %.2f (+- %.2f) %.2E (+- %.2E)\n" \
	    ${NAME[i]} ${NAME[j]} $AVGN_O $ICFN_O $AVGT_O $ICFT_O $AVGN_X $ICFN_X $AVGT_X $ICFT_X
	#echo $AVGN_O $ICFN_O $AVGT_O $AVGN_X $AVGT_X
    done
done
rm -f $$.tmp

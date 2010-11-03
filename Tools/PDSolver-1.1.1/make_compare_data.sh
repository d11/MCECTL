#!/bin/sh

run_time="86400"
base_dir="/home/matt/work/PushdownParity/data2"
reps="2"
mu_min="5"
mu_max="50"
g_min="5"
g_max="50"
timeout="10"


ulimit -t $run_time

for i in {1..1000} 
do
    echo
    echo "Mu both tests."

    ./src/main/pdsolver.opt -v -no -o $base_dir/muboth$i -gd-inf -gs -dmin $mu_min -dmax $mu_max -dstep 5 -dreps $reps -rt pds_mucalc -ct both -to $timeout

    echo 
    echo "G both tests."

    ./src/main/pdsolver.opt -v -no -o $base_dir/gboth$i -gd-inf -gs -dmin $g_min -dmax $g_max -dstep 5 -dreps $reps -rt game -ct both -to $timeout
done

ulimit -t unlimited

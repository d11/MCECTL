#!/bin/sh

run_time="86400"
base_dir="/home/matt/work/data"
reps="2"
mu_min="5"
mu_max="150"
g_min="5"
g_max="150"
timeout="30"

ulimit -t $run_time

for i in {1..1000} 
do
    echo
    echo "Mu lone tests."

    ./src/main/pdsolver.opt -v -no -o $base_dir/mulone$i -gd-inf -gs -dmin $mu_min -dmax $mu_max -dstep 5 -dreps $reps -rt pds_mucalc -ct mucalc -to $timeout

    echo 
    echo "G both tests."

    ./src/main/pdsolver.opt -v -no -o $base_dir/glone$i -gd-inf -gs -dmin $g_min -dmax $g_max -dstep 5 -dreps $reps -rt game -ct game -to $timeout
done

ulimit -t unlimited

#!/bin/bash

set -x

./FEvoRslab.py 

flowline.py -o pism-in.nc --expand -d y fevor-slab-in.nc

pismr -surface given -boot_file pism-in.nc -periodicity xy \
 -Mx 21 -My 11 -Lx 10.05 -Mz 11 -Lz 210 -y 1 \
 -bed_smoother_range 0 \
 -stress_balance sia_fevor \
 -sia_fevor_use_constant_slope \
 -sia_fevor_surface_slope_degrees -0.1 \
 -extra_file ex.nc -extra_times 0.1 \
 -extra_vars distributions,recrystallizations,thk,flux_mag,velsurf_mag,h_x,h_y,taud_mag,enhancement_factor \
 -o pism-out.nc

flowline.py -o fevor-slab-out.nc --collapse -d y pism-out.nc

#!/bin/bash

PISM_PATH=$1
MPIEXEC=$2

# Test name:
echo "Test #17: verif test G regression: thermo SIA w. time-dependent SMB."

output=`mktemp pism-test-G.XXXX` || exit 1

# run test G
OPTS="-test G -Mbz 1 -Mz 31 -y 1000years -max_dt 60years -o_size none -verbose 1 -time_stepping.resolution 0"
$MPIEXEC -n 4 $PISM_PATH/pismv -Mx 31 -My 31 $OPTS   > ${output}
$MPIEXEC -n 4 $PISM_PATH/pismv -Mx 41 -My 41 $OPTS  >> ${output}

# compare results
diff ${output} -  <<END-OF-OUTPUT
NUMERICAL ERRORS evaluated at final time (relative to exact solution):
geometry  :    prcntVOL        maxH         avH   relmaxETA
               0.780389   32.443559    7.148986    0.016300
temp      :        maxT         avT    basemaxT     baseavT
               0.834661    0.249470    0.747461    0.151304
Sigma     :      maxSig       avSig
               7.208339    0.955275
surf vels :     maxUvec      avUvec        maxW         avW
               0.945140    0.199934    0.028361    0.004064
NUM ERRORS DONE
NUMERICAL ERRORS evaluated at final time (relative to exact solution):
geometry  :    prcntVOL        maxH         avH   relmaxETA
               0.782888   30.772156    7.242030    0.015477
temp      :        maxT         avT    basemaxT     baseavT
               0.894135    0.248278    0.746663    0.155383
Sigma     :      maxSig       avSig
               7.185588    0.913990
surf vels :     maxUvec      avUvec        maxW         avW
               0.886013    0.194881    0.027400    0.004233
NUM ERRORS DONE
END-OF-OUTPUT

if [ $? != 0 ];
then
  cat ${output}
  exit 1
fi

exit 0

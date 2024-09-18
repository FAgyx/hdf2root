export PATH=$(pwd)/build:$PATH


# set up ROOT

export ALRB_rootVersion=6.30.02-x86_64-el9-gcc13-opt
LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
export ATLAS_LOCAL_ROOT_BASE=$LOCAL_ROOT_BASE
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh --rootVersion ${ALRB_rootVersion}


# set up CMAKE
lsetup "views LCG_104d_ATLAS_20 x86_64-el9-gcc13-opt"
lsetup cmake
~                  


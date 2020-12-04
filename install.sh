#!/bin/bash

export URL_TO_MG5="https://launchpad.net/mg5amcnlo/2.0/2.7.x/+download/MG5_aMC_v2.7.3.tar.gz"
export MG5_DIRNAME="MG5_aMC_v2_7_3"

#set environment for installation - consistant with HTCondor setup
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh

wget ${URL_TO_MG5}
tar -zxvf `basename ${URL_TO_MG5}`

cd ./${MG5_DIRNAME}/models

wget http://feynrules.irmp.ucl.ac.be/raw-attachment/wiki/DMsimp/DMsimp_s_spin1_v2.0.zip
unzip DMsimp_s_spin1_v2.0.zip

cd ..

exec 3<> install.txt
echo "install pythia8" >&3
echo "install collier" >&3
echo "install ninja" >&3
echo "install MadAnalysis5" >&3

#run nlo the first time - compile HEP tools
echo "import model DMsimp_s_spin1" >&3
echo "generate p p > xd xd~ j [QCD]" >&3
echo "output mytest" >&3
exec 3>&-

./bin/mg5_aMC install.txt

rm -rf install.txt
rm -rf mytest

#run lo the first time - setup PDF
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/HEPTools/lhapdf6/lib
exec 3<> install.txt
echo "import model DMsimp_s_spin1" >&3
echo "generate p p > xd xd~ j" >&3
echo "output mytest" >&3
echo "launch" >&3
echo "set pdlabel lhapdf" >&3
echo "set lhaid 260000" >&3
echo "set iseed 1234" >&3
exec 3>&-

./bin/mg5_aMC install.txt

rm -rf install.txt

cd ..

rm -rf MG5_aMC_v2.7.3.tar.gz

echo "done"

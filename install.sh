#!/bin/bash

export URL_TO_MG5="https://launchpad.net/mg5amcnlo/2.0/2.7.x/+download/MG5_aMC_v2.7.3.tar.gz"
export MG5_DIRNAME="MG5_aMC_v2_7_3"

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
exec 3>&-

./bin/mg5_aMC install.txt

rm -rf install.txt

echo "done"

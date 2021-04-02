# MCsetup_monojet

This git repository gives instructions on getting mono-jet cut flow as well as MET kinematic distribution for a truth level simulation of Collider Phenomenology following the ATLAS analysis (arxiv: 2102.10874). We provide MG5 LO + Pythia8 control code with our validated Rivet analysis. `launch_many.py` runs at lxplus using HTCondor (hence a CERN account is required).

## MG5 LO + Pythia8

### Installation

To install MG5 and Pythia8, simply type:

`./install.sh`

This will also download MadAnalysis5 and our validated s-channel dmsimp spin-1 Madgraph UFO.

### Launch Single Job

MG5 simulation requires setup on model parameters. In this case, one should set mDM, Mmed, mediator type, gDM, gl, gq and the mediator width, where functions encoded in `exclCalc.py` will handle the width calculation using equations from arXiv: 1507.00966.

Run parameters should also be specified. One can very beam energy, nevents (total events number) and xqcut (merging scale, LO). Non-value xqcut will be regarded as 1 jet multiplicity only simulation without merging.

Therefore, one runs the code following:

`python mg5lo_launch_single.py {mDM} {Mmed} {med_type} {gDM} {gq} {gl} {model_tag} {Ebeam_single} {nevents} {xqcut: provide value if do two processes} {repeat}`

Here, the masses and single beam energy are in the unit of GeV. Due to significant loss of events during the merging and monojet event cut / veto, and in light of the total time consumed for a single job, it is statistically necessary to repeat the simulation for a point in parameter space using lunch many jobs feature.

We will use MLM algorithm (ickkw = 1) for LO event merging, and the corresponding xqcut is in the unit of GeV. The merging scale can be validated via pt distributions of the leading, sub-leading and third jet as well as the number of jets distribution.

### Launch Many Jobs

Launch Many Jobs is achieved by distributing different commands to clusters using HTCondor. One sets MG5 / lhapdf paths and job parameters in `launch_many_parameters.dat` then launch the jobs by:

`python launch_many.py`

For each job, the log file and hepmc file will be collected automatically to the `./results/` folder.

## Rivet

If you want to check things quickly, the validated Rivet analysis code for monojet can be found in `./rivet_monojet`. Due to the large software dependence in Rivet, we suggest using Docker container where a detailed description for the setup can be found on:

[https://github.com/DarkJets-hep/excitedquarks-rivet](https://github.com/DarkJets-hep/excitedquarks-rivet)

Once the Rivet environment is ready. The whole procedure on setup, jointing separate runs and doing analysis can be done by:

`python rivet_launch.py {path/to/hepmc/dir}`

# MCsetup_monojet

This git repository gives instructions on plotting kinematic distributions at Collider for monojet analysis. We provide MG5 NLO/LO + Pythia8 control code with our validated Rivet analysis for MG5 NLO. The CERN account is required in this tutorial in order to run `launch_many.py` at lxplus using HTCondor.

## MG5 NLO/LO + Pythia8

It is not suggested to run `mg5nlo_launch_single.py` directly since the NLO process will take a few days.

### Installation

To install MG5 and Pythia8, simply type:

`./install.sh`

This will also download our validated s-channel dmsimp spin-1 Madgraph UFO (recommended in this analysis).

### Launch Single Job

MG5 simulation requires setup on model parameters. In this case, one should set mDM, Mmed, gl, gq, gDM and the mediator width, where functions encoded in `exclCalc.py` will handle the width calculation using equations from arXiv: 1507.00966.

Run parameters should also be specified. One can very beam energy, nevents (total events number), xqcut (merging scale, LO), ptj (minimum transverse momentum cut on the jets, NLO) and Qcut (merging scale, NLO).

Therefore, one runs the code following:

`python mg5nlo_launch_single.py {mDM} {Mmed} {model} {Ebeam_single} {nevents} {ptj} {Qcut} {repeat}`

or

`python mg5lo_launch_single.py {mDM} {Mmed} {model} {Ebeam_single} {nevents} {xqcut} {repeat}`

Here, the masses and single beam energy are in the unit of GeV. The model will be four s-channel dmsimp benchmark models (A1, A2, V1 and V2, see arXiv: 1703.05703). Due to significant loss of events during the merging (NLO) and monojet event cut / veto, and in light of the total time consumed for a single job, it is statistically necessary to repeat the simulation for a point in parameter space where the nevents is recommended to be ~15K with repeat ~15.

We will use the FxFx algorithm (ickkw = 3) for NLO event merging and MLM algorithm (ickkw = 1) for LO event merging. The Qcut, ptj and xqcut are in the unit of GeV, where Qcut should be larger than twice the value of ptj. The merging scale can be validated via pt distribution of the leading, sub-leading and third jet. The relevant plots will be included in our Rivet analysis code.

### Launch Many Jobs

Launch Many Jobs is achieved by distributing different commands to clusters using HTCondor. One sets MG5 / lhapdf paths and job parameters in `launch_many_parameters.dat` then launch the jobs by:

`python launch_many.py`

For each job, the log file and hepmc file will be collected automatically to the `./results/` folder. The Madanalysis5 results for LO will also be collected.

## Rivet

If you want to check things quickly, the validated Rivet analysis code for monojet can be found in `./rivet_monojet`. Due to the large software dependence in Rivet, we suggest using Docker container where a detailed description for the setup can be found on:

[https://github.com/DarkJets-hep/excitedquarks-rivet](https://github.com/DarkJets-hep/excitedquarks-rivet)

Once the Rivet environment is ready. The whole procedure on building analysis, jointing separate runs and making merging / MET histograms can be done by:

`python rivet_launch_analysis.py {path/to/hepmc/dir}`

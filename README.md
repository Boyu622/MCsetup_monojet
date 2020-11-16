# MCsetup_monojet

This git repository gives instructions on plotting kinematic distributions at Collider using MG5_NLO + Pythia8 + Rivet software chain for monojet analysis. The CERN account is required in this tutorial in order to run MG5_NLO at lxplus using HTCondor.

## MG5_NLO + Pythia8

It is not suggested to run `mg5nlo_launch_single.py` directly since the NLO process will take a few days.

### Installation

To install MG5 and Pythia8, simply type:

`./install.sh`

This will also download our validated s-channel dmsimp spin-1 Madgraph UFO (recommended in this analysis).

### Launch Single Job

MG5 simulation requires setup on model parameters. In this case, one should set mDM, Mmed, gl, gq, gDM and the mediator width, where functions encoded in `exclCalc.py` will handle the width calculation using equations from arXiv: 1507.00966.

Run parameters should also be specified. One can very beam energy, nevents (total events number), ptj (minimum transverse momentum cut on the jets) and Qcut (merging scale).

Therefore, one runs the code following:

`python mg5nlo_launch_single.py {mDM} {Mmed} {model} {single-beam-energy} {nevents} {ptj} {Qcut}`

Here, the masses and single beam energy are in the unit of GeV. The model will be four s-channel dmsimp benchmark models (A1, A2, V1 and V2, see arXiv: 1703.05703). The nevents are recommended to be larger than 100K in order for enough events after merging and monojet event selection. 

We will use the FxFx algorithm (ickkw = 3) for event merging. The Qcut is in the unit of GeV which controls the merging scale when showering the events. It should be larger than twice the value of ptj. The merging scale can be validated via pt distribution of the leading, sub-leading and third jet. The relevant plots will be included in our Rivet analysis code.

### Launch Many Jobs

Launch Many Jobs is achieved by distributing different commands to clusters using HTCondor. One sets MG5 / lhapdf paths and scan variables in `paths_and_parameters.dat` then launch the jobs by:

`python mg5nlo_launch_many.py`

For each job, the log file and hepmc file will be collected automatically to the `./results/` folder.

## Rivet

We recommend using Rivet for event analysis. Due to the large software dependence in Rivet, we suggest using Docker container where a detailed description for the setup can be found on:

[https://github.com/DarkJets-hep/excitedquarks-rivet](https://github.com/DarkJets-hep/excitedquarks-rivet)

Following the above document, the `rivet_monojet.cc` and `rivet_monojet.plot` will plot the 1st, 2nd and 3rd jet pt distributions before the monojet event selection. It will also show MET distribution after the event selection.

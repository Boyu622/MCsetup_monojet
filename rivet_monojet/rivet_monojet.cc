#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/IdentifiedFinalState.hh"
#include "Rivet/Projections/VisibleFinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Projections/MissingMomentum.hh"
#include "Rivet/Projections/SmearedParticles.hh"
#include "Rivet/Projections/SmearedJets.hh"
#include "Rivet/Projections/SmearedMET.hh"

namespace Rivet {


  class rivet_monojet : public Analysis {
  public:

    DEFAULT_RIVET_ANALYSIS_CTOR(rivet_monojet);
    void init() {
      const FinalState cnfs;
      declare(FastJets(cnfs, FastJets::ANTIKT, 0.4), "JS");

      FastJets jets(FinalState(Cuts::abseta < 4.9), FastJets::ANTIKT, 0.4);
      SmearedJets recojets(jets, JET_SMEAR_ATLAS_RUN1);
      declare(recojets, "Jets");

      FinalState electrons(Cuts::abspid == PID::ELECTRON && Cuts::abseta < 2.47 && Cuts::pT > 20*GeV);
      SmearedParticles recoelectrons(electrons, ELECTRON_IDEFF_ATLAS_RUN1_MEDIUM);
      declare(recoelectrons, "Electrons");

      FinalState muons(Cuts::abspid == PID::MUON && Cuts::abseta < 2.50 && Cuts::pT > 10*GeV);
      SmearedParticles recomuons(muons, MUON_EFF_ATLAS_RUN1);
      declare(recomuons, "Muons");

      VisibleFinalState calofs(Cuts::abseta < 4.9 && Cuts::abspid != PID::MUON);
      MissingMomentum met(calofs);
      SmearedMET recomet(met, MET_SMEAR_ATLAS_RUN1);
      declare(recomet, "MET");

      book(_histnjets ,"njets", 60, 0, 300);
      book(_histjetpt0 ,"leading_jet_pt", 236, 20, 1200);
      book(_histjetpt1 ,"subleading_jet_pt", 126, 20, 600);
      book(_histjetpt2 ,"third_jet_pt", 76, 20, 400);
      book(_histem1 ,"EM1", 1, 250, 300);
      book(_histem2 ,"EM2", 1, 300, 350);
      book(_histem3 ,"EM3", 1, 350, 400);
      book(_histem4 ,"EM4", 1, 400, 500);
      book(_histem5 ,"EM5", 1, 500, 600);
      book(_histem6 ,"EM6", 1, 600, 700);
      book(_histem7 ,"EM7", 1, 700, 800);
      book(_histem8 ,"EM8", 1, 800, 900);
      book(_histem9 ,"EM9", 1, 900, 1000);
      book(_histem10 ,"EM10", 1, 1000, 1200);
    }


    void analyze(const Event& event) {
      const Jets js = apply<JetAlg>(event, "JS").jetsByPt();
      const Jets jets = apply<JetAlg>(event, "Jets").jetsByPt(Cuts::pT > 20*GeV && Cuts::abseta < 2.8);
      _histnjets->fill(js.size());
      _histjetpt0->fill(js[0].pT());
      _histjetpt1->fill(js[1].pT());
      _histjetpt2->fill(js[2].pT());
      const Particles elecs = apply<ParticleFinder>(event, "Electrons").particlesByPt();
      const Particles mus = apply<ParticleFinder>(event, "Muons").particlesByPt();
      MSG_DEBUG("Number of raw jets, electrons, muons = "
                << jets.size() << ", " << elecs.size() << ", " << mus.size());

      // Discard jets very close to electrons, or with low track multiplicity and close to muons
      const Jets isojets = filter_discard(jets, [&](const Jet& j) {
          if (any(elecs, deltaRLess(j, 0.2))) return true;
          if (j.particles(Cuts::abscharge > 0 && Cuts::pT > 0.4*GeV).size() < 3 &&
              any(mus, deltaRLess(j, 0.4))) return true;
          return false;
        });

      // Discard electrons close to remaining jets
      const Particles isoelecs = filter_discard(elecs, [&](const Particle& e) {
          return any(isojets, deltaRLess(e, 0.4));
        });

      // Discard muons close to remaining jets
      const Particles isomus = filter_discard(mus, [&](const Particle& m) {
          for (const Jet& j : isojets) {
            if (deltaR(j,m) > 0.4) continue;
            if (j.particles(Cuts::abscharge > 0 && Cuts::pT > 0.4*GeV).size() > 3) return true;
          }
          return false;
        });

      // Calculate ETmiss
      const Vector3& vet = apply<SmearedMET>(event, "MET").vectorEt();
      const double etmiss = vet.perp();


      // Event selection cuts
      if (etmiss < 250*GeV) vetoEvent;
      // Require at least one jet with pT > 250 GeV and |eta| < 2.4
      if (filter_select(isojets, Cuts::pT > 250*GeV && Cuts::abseta < 2.4).empty()) vetoEvent;
      // Require at most 4 jets with pT > 30 GeV and |eta| < 2.8
      if (filter_select(isojets, Cuts::pT > 30*GeV).size() > 4) vetoEvent;
      // Require no isolated jets within |dphi| < 0.4 of the MET vector
      if (any(isojets, deltaPhiLess(-vet, 0.4))) vetoEvent;
      // Require no isolated electrons or muons
      if (!isoelecs.empty() || !isomus.empty()) vetoEvent;
      
      if (etmiss > 250*GeV && etmiss < 300*GeV) _histem1->fill(etmiss);
      if (etmiss > 300*GeV && etmiss < 350*GeV) _histem2->fill(etmiss);
      if (etmiss > 350*GeV && etmiss < 400*GeV) _histem3->fill(etmiss);
      if (etmiss > 400*GeV && etmiss < 500*GeV) _histem4->fill(etmiss);
      if (etmiss > 500*GeV && etmiss < 600*GeV) _histem5->fill(etmiss);
      if (etmiss > 600*GeV && etmiss < 700*GeV) _histem6->fill(etmiss);
      if (etmiss > 700*GeV && etmiss < 800*GeV) _histem7->fill(etmiss);
      if (etmiss > 800*GeV && etmiss < 900*GeV) _histem8->fill(etmiss);
      if (etmiss > 900*GeV && etmiss < 1000*GeV) _histem9->fill(etmiss);
      if (etmiss > 1000*GeV && etmiss < 1200*GeV) _histem10->fill(etmiss);


    }

      void finalize() {
      const double norm = 1.0*crossSection()/femtobarn; //no data information - pick 1.0
      scale(_histem1, norm/sumOfWeights());
      scale(_histem2, norm/sumOfWeights());
      scale(_histem3, norm/sumOfWeights());
      scale(_histem4, norm/sumOfWeights());
      scale(_histem5, norm/sumOfWeights());
      scale(_histem6, norm/sumOfWeights());
      scale(_histem7, norm/sumOfWeights());
      scale(_histem8, norm/sumOfWeights());
      scale(_histem9, norm/sumOfWeights());
      scale(_histem10, norm/sumOfWeights());
      scale(_histnjets, norm/sumOfWeights());
      scale(_histjetpt0, norm/sumOfWeights());
      scale(_histjetpt1, norm/sumOfWeights());
      scale(_histjetpt2, norm/sumOfWeights());
    }
      
      Histo1DPtr _histnjets, _histjetpt0, _histjetpt1, _histjetpt2, _histem1, _histem2, _histem3, _histem4, _histem5, _histem6, _histem7, _histem8, _histem9, _histem10;

  };

  DECLARE_RIVET_PLUGIN(rivet_monojet);

}

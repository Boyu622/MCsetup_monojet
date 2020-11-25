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
        
      book(_histmet ,"MET", 10, 200, 1200);
    }


    void analyze(const Event& event) {
      
      const Jets jets = apply<JetAlg>(event, "Jets").jetsByPt(Cuts::pT > 20*GeV && Cuts::abseta < 2.8);
      _histnjets->fill(jets.size());
      _histjetpt0->fill(jets[0].pT());
      _histjetpt1->fill(jets[1].pT());
      _histjetpt2->fill(jets[2].pT());
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
        
      _histmet ->fill(etmiss);

    }
      
      Histo1DPtr _histnjets, _histjetpt0, _histjetpt1, _histjetpt2, _histmet;

  };

  DECLARE_RIVET_PLUGIN(rivet_monojet);

}

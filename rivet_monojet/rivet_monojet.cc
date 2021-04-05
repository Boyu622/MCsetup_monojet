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

      double met_larger_than_150, after_lep_veto, nj_four, min_phi, lead_j_quality, lead_j_pt_eta, met_larger_than_200, met_em0, met_em1, met_em2, met_em3, met_em4, met_em5, met_em6, met_em7, met_em8, met_em9, met_em10, met_em11, met_em12;

  class rivet_monojet : public Analysis {
  public:

    DEFAULT_RIVET_ANALYSIS_CTOR(rivet_monojet);
    void init() {
      const FinalState cnfs;
      declare(FastJets(cnfs, FastJets::ANTIKT, 0.4), "JS");
          
      FastJets jets(FinalState(Cuts::abseta < 4.9), FastJets::ANTIKT, 0.4);
      SmearedJets recojets(jets, JET_SMEAR_ATLAS_RUN1);
      declare(recojets, "Jets");

      FinalState electrons(Cuts::abspid == PID::ELECTRON && Cuts::abseta < 2.47 && Cuts::pT > 7*GeV);
      SmearedParticles recoelectrons(electrons, ELECTRON_IDEFF_ATLAS_RUN1_MEDIUM);
      declare(recoelectrons, "Electrons");

      FinalState muons(Cuts::abspid == PID::MUON && Cuts::abseta < 2.50 && Cuts::pT > 7*GeV);
      SmearedParticles recomuons(muons, MUON_EFF_ATLAS_RUN1);
      declare(recomuons, "Muons");

      VisibleFinalState calofs(Cuts::abseta < 4.5 && Cuts::pT > 20*GeV);
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
          if (j.pT() > 30*GeV && j.particles(Cuts::pT > 0.5*GeV).size() < 3 &&
              any(mus, deltaRLess(j, 0.4))) return true;
          return false;
        });

      // Remove electrons deltaR:0.2-0.4 to remaining jets
      const Particles isoelecs = filter_discard(elecs, [&](const Particle& e) {
          for (const Jet& j : isojets) {
            if (deltaR(j,e) > 0.2 && deltaR(j,e) < 0.4) return true;
          }
          return false;
        });

      // Discard muons close to remaining jets
      const Particles isomus = filter_discard(mus, [&](const Particle& m) {
          for (const Jet& j : isojets) {
            if (deltaR(j,m) > 0.4) continue;
            if (j.pT() > 30*GeV && j.particles(Cuts::pT > 0*GeV).size() > 3) return true;
          }
          return false;
        });

      // Calculate ETmiss
      const Vector3& vet = apply<SmearedMET>(event, "MET").vectorEt();
      const double etmiss = vet.perp();

      // cut flow
      if (etmiss < 150*GeV) vetoEvent;
      met_larger_than_150++;
      
      if (!isoelecs.empty() || !isomus.empty()) vetoEvent;
      after_lep_veto++;
      
      if (isojets.size() > 4) vetoEvent;
      nj_four++;
      
      if (any(isojets, deltaPhiLess(-vet, 0.4))) vetoEvent;
      min_phi++;
      
      if (filter_select(isojets, Cuts::pT > 150*GeV && Cuts::abseta < 2.4).empty()) vetoEvent;
      lead_j_pt_eta++;
      
      if (etmiss < 200*GeV) vetoEvent;
      met_larger_than_200++;
       
      if (etmiss > 200*GeV && etmiss < 250*GeV) met_em0++;
      if (etmiss > 250*GeV && etmiss < 300*GeV) {
      _histem1->fill(etmiss);
      met_em1++;}
      if (etmiss > 300*GeV && etmiss < 350*GeV) {
      _histem2->fill(etmiss);
      met_em2++;}
      if (etmiss > 350*GeV && etmiss < 400*GeV) {
      _histem3->fill(etmiss);
      met_em3++;}
      if (etmiss > 400*GeV && etmiss < 500*GeV) {
      _histem4->fill(etmiss);
      met_em4++;}
      if (etmiss > 500*GeV && etmiss < 600*GeV) {
      _histem5->fill(etmiss);
      met_em5++;}
      if (etmiss > 600*GeV && etmiss < 700*GeV) {
      _histem6->fill(etmiss);
      met_em6++;}
      if (etmiss > 700*GeV && etmiss < 800*GeV) {
      _histem7->fill(etmiss);
      met_em7++;}
      if (etmiss > 800*GeV && etmiss < 900*GeV) {
      _histem8->fill(etmiss);
      met_em8++;}
      if (etmiss > 900*GeV && etmiss < 1000*GeV) {
      _histem9->fill(etmiss);
      met_em9++;}
      if (etmiss > 1000*GeV && etmiss < 1100*GeV) {
      _histem10->fill(etmiss);
      met_em10++;}
      if (etmiss > 1100*GeV && etmiss < 1200*GeV) met_em11++;
      if (etmiss > 1200*GeV) met_em12++;
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
      
      cout << "met_larger_than_150: " << met_larger_than_150 << ", ratio: " << met_larger_than_150/met_larger_than_150 << endl;
      cout << "after_lep_veto: " << after_lep_veto << ", ratio: " << after_lep_veto/met_larger_than_150 << endl;
      cout << "nj_four: " << nj_four << ", ratio: " << nj_four/met_larger_than_150 << endl;
      cout << "min_phi: " << min_phi << ", ratio: " << min_phi/met_larger_than_150 << endl;
      cout << "lead_j_pt_eta: " << lead_j_pt_eta << ", ratio: " << lead_j_pt_eta/met_larger_than_150 << endl;
      cout << "met_larger_than_200: " << met_larger_than_200 << ", ratio: " << met_larger_than_200/met_larger_than_150 << endl;
      cout << "met_em0: " << met_em0 << ", ratio: " << met_em0/met_larger_than_150 << endl;
      cout << "met_em1: " << met_em1 << ", ratio: " << met_em1/met_larger_than_150 << endl;
      cout << "met_em2: " << met_em2 << ", ratio: " << met_em2/met_larger_than_150 << endl;
      cout << "met_em3: " << met_em3 << ", ratio: " << met_em3/met_larger_than_150 << endl;
      cout << "met_em4: " << met_em4 << ", ratio: " << met_em4/met_larger_than_150 << endl;
      cout << "met_em5: " << met_em5 << ", ratio: " << met_em5/met_larger_than_150 << endl;
      cout << "met_em6: " << met_em6 << ", ratio: " << met_em6/met_larger_than_150 << endl;
      cout << "met_em7: " << met_em7 << ", ratio: " << met_em7/met_larger_than_150 << endl;
      cout << "met_em8: " << met_em8 << ", ratio: " << met_em8/met_larger_than_150 << endl;
      cout << "met_em9: " << met_em9 << ", ratio: " << met_em9/met_larger_than_150 << endl;
      cout << "met_em10: " << met_em10 << ", ratio: " << met_em10/met_larger_than_150 << endl;
      cout << "met_em11: " << met_em11 << ", ratio: " << met_em11/met_larger_than_150 << endl;
      cout << "met_em12: " << met_em12 << ", ratio: " << met_em12/met_larger_than_150 << endl;      
    }
      Histo1DPtr _histnjets, _histjetpt0, _histjetpt1, _histjetpt2, _histem1, _histem2, _histem3, _histem4, _histem5, _histem6, _histem7, _histem8, _histem9, _histem10;

  };

  DECLARE_RIVET_PLUGIN(rivet_monojet);
      
}

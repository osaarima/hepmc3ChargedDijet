#include  "JHepMC3DataManager.h"


//______________________________________________________________________________
JHepMC3DataManager::JHepMC3DataManager(TString hepMCFile)
{
	// constructor
    // old:
	//fascii_in = new HepMC3::IO_GenEvent(hepMCFile,std::ios::in);
    // new:
    ftext_input = new HepMC3::ReaderAscii(hepMCFile.Data());
    finternalEvtCounter = 0;
}

/*//______________________________________________________________________________
JHepMC3DataManager::~JHepMC3DataManager(){
	//if( ftext_input ) delete ftext_input;
	//if( fevt ) delete fevt;
}
*/

//______________________________________________________________________________
/*
bool JHepMC3DataManager::next_event(){    
    bool breturn;
    if(fevt) delete fevt;
    fevt = fascii_in->read_next_event();
    if(fevt) breturn = true;
    else breturn = false;
    return breturn; 
}
*/


//______________________________________________________________________________
bool JHepMC3DataManager::getList(TClonesArray* listToFill) { 

    listToFill->Clear("C");
    int counter = 0;

    HepMC3::GenEvent evt(HepMC3::Units::GEV,HepMC3::Units::MM);
    ftext_input->read_event(evt);
    if( ftext_input->failed() ) return false;
    const std::vector<HepMC3::ConstGenParticlePtr> hadrons = GetHadrons(evt);
    if( finternalEvtCounter == 0 ) {
        cout << " First event: " << endl;
        HepMC3::Print::listing(evt);
        HepMC3::Print::content(evt);

        cout << " Testing attribute reading for the first event: " << endl;

        shared_ptr<HepMC3::GenCrossSection> cs = evt.attribute<HepMC3::GenCrossSection>("GenCrossSection");
        shared_ptr<HepMC3::GenHeavyIon>     hi = evt.attribute<HepMC3::GenHeavyIon>("GenHeavyIon");
        shared_ptr<HepMC3::GenPdfInfo>      pi = evt.attribute<HepMC3::GenPdfInfo>("GenPdfInfo");

        if(cs) {
            cout << " Has GenCrossSection:   ";
            HepMC3::Print::line(cs);
        }
        else cout << " No GenCrossSection " << endl;

        if(pi) {
            cout << " Has GenPdfInfo:        ";
            HepMC3::Print::line(pi);
        }
        else cout << " No GenPdfInfo " << endl;

        if(hi) {
            cout << " Has GenHeavyIon:       ";
            HepMC3::Print::line(hi);
        }
        else cout << " No GenHeavyIon " << endl;
    }
    for (auto p : hadrons) {
        TLorentzVector lParticle(p->momentum().px(), p->momentum().py(), p->momentum().pz(), p->momentum().e());
        //cout << "Particle: " << p->momentum().px() << ", " <<  p->momentum().py() << ", " << p->momentum().pz() << ", " << p->momentum().e() << endl;
        AliJBaseTrack track( lParticle );
        track.SetID(p->pid());
        track.SetTrackEff(1.);
        new ((*listToFill)[listToFill->GetEntriesFast()]) AliJBaseTrack(track);
        //}
    } // end of finalparticles


    finternalEvtCounter++;
    return true;

    /*
   for ( HepMC3::GenEvent::particle_const_iterator p = fevt->particles_begin(); p != fevt->particles_end(); ++p ){
   TLorentzVector lParticle((*p)->momentum().px(), (*p)->momentum().py(), (*p)->momentum().pz(), (*p)->momentum().e());
   AliJBaseTrack track( lParticle );
   track.SetID((*p)->pdg_id());
   track.SetTrackEff(1.);
   new ((*listToFill)[counter++]) AliJBaseTrack(track);
   } // end of finalparticles
   */
}





// From JETSCAPE-analysis c++ branch:
// Courtesy of James Mulligan
//-----------------------------------------------------------------
// Get list of hadrons.
// Final state hadrons (from jet + bulk) are stored as outgoing particles in a disjoint vertex with t = 100
std::vector<HepMC3::ConstGenParticlePtr> JHepMC3DataManager::GetHadrons(const HepMC3::GenEvent &event) {
  
  std::vector<HepMC3::ConstGenParticlePtr> final_state_particles;
  for (auto vertex : event.vertices()) {
    
    double vertexTime = vertex->position().t();
    if ( abs(vertexTime - 100) < 1e-3 ) {
      final_state_particles = vertex->particles_out();
    }
    
  }
  
  // Remove neutrinos
  std::vector<HepMC3::ConstGenParticlePtr> hadrons;
  for (auto particle : final_state_particles) {
    
    int pid = particle->pid();
    if (pid!=12 && pid!=14 && pid!=16) {
      hadrons.push_back(particle);
    }
    
  }
  
  return hadrons;
  
}

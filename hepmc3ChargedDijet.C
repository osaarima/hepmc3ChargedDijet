#include "TH1D.h"
#include "TProfile.h"
#include "TMath.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include <TStopwatch.h>
//#include <HepMC/IO_GenEvent.h>
//#include <HepMC/GenEvent.h>

//#include "HepMC/GenEvent.h"
//#include "HepMC/ReaderAscii.h"
//#include "HepMC/Print.h"
//#include "HepMC/GenCrossSection.h"

#include "HepMC3/GenEvent.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/GenParticle_fwd.h"
#include "HepMC3/Print.h"
#include "HepMC3/GenCrossSection.h"

//#include <HepPID/ParticleName.hh>
#include <HepPDT/defs.h>
#include <HepPDT/TableBuilder.hh>
#include <HepPDT/TempParticleData.hh>
#include <HepPDT/ParticleDataTable.hh>

#include "fastjet/config.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
/*
#include "fastjet/config.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/AreaDefinition.hh"
#include <fastjet/SISConePlugin.hh>
#include <fastjet/CDFMidPointPlugin.hh>
#ifdef FASTJET_VERSION
#include <fastjet/Selector.hh>
#include <fastjet/FunctionOfPseudoJet.hh>
#include <fastjet/tools/BackgroundEstimatorBase.hh>
#include <fastjet/tools/Subtractor.hh>
*/
#include <iostream>

#include <TClonesArray.h>
#include "src/AliJCDijetHistos.h"
#include "src/AliJCDijetAna.h"
#include "src/AliJBaseTrack.h"
//#include "src/JTreeDataManager.h"
//#include "src/AliJCard.h"
//#include "src/iaaAnalysis/AliJIaaAna.h"
//#include "src/JHistos.h"
//#include "src/AliJCard.h"

#define DEBUG 0

using namespace HepMC3;
using namespace fastjet;
using namespace std;
std::vector<HepMC3::ConstGenParticlePtr> GetHadrons(const HepMC3::GenEvent &event);


class AliJCDijetHistos;

AliJCDijetHistos *fhistos;


int main(int argc, char **argv) {

	if(argc<5){
		cout<<"usage: " << argv[0] << " <input.hepmc> <output.root> <particle.tbl> dijetLeadingPt"<<endl;exit(1);
	}
	TStopwatch timer; 
	timer.Start();   

	char* hepmcFilename  = argv[1];
	TString outputs = argv[2];
	TString sParticle = argv[3];
    double dijetLeadingPt = atof(argv[4]);

	TFile *fout = new TFile(outputs.Data(),"RECREATE");
	fout->cd();//opening of the output file
    TDirectoryFile *fdir = new TDirectoryFile( "JCDijetBaseTask","JCDijetBaseTask" );
    fdir->cd();

	//HepMC::IO_GenEvent ascii_in(hepmcFilename,std::ios::in);
    //Old:
    //ReaderAscii text_input (hepmcFilename);
    //New:
    HepMC3::ReaderAscii text_input(hepmcFilename);
    //HepMC3::ReaderAscii reader(hepmcFile.c_str());

    // The HepPDT initialization.
    // Initialize HepPDF so that particle ID's can be used to determine charge and hadrons
    std::ifstream pdfile( sParticle.Data() );
    if( !pdfile ) { 
      std::cerr << "cannot open " << sParticle.Data() << std::endl;
      exit(-1);
    }
    HepPDT::ParticleDataTable datacol( "Generic Particle Table" );
    {
        // Construct table builder
        HepPDT::TableBuilder  tb(datacol);
        // bool  addParticleTable( std::istream&, TableBuilder&,
        //                         bool validate = false );
        // where:  validate=true => verify that the ParticleID is valid
        if( !addParticleTable( pdfile, tb, true ) ) { 
            std::cout << "error reading PDG pdt file " << std::endl; 
        }
    } // the tb destructor fills datacol

    //-------------------------------------------------------
    // Histograms and tools
    //-------------------------------------------------------
    //
    //
    fhistos = new AliJCDijetHistos();
    vector<double> centbins = {0.0, 100.0};
    TString sDijetMBins = "0, 20, 40, 45, 55, 65, 75, 85, 100, 120, 150, 250, 400, 500, 100000";
    fhistos->SetCentralityBinsHistos(centbins);
    fhistos->CreateEventTrackHistos();
    fhistos->SetDijetMBinsHistos(sDijetMBins);

    fhistos->fHMG->Print();

    TH1D *hCrossSectionInfo = new TH1D("hCrossSection","CrossSectionInfo",8,0,8);

    AliJCDijetAna* fana = new AliJCDijetAna();

    /*
    // ======================== JIaa Ana =========================

    //TString cardName  = Form("%s%s",gSystem->Getenv("ALICE_PHYSICS"),"/PWGCF/Correlations/macros/jcorran/cardAlice_IAA_pp.input");
    TString cardName  = Form("%s%s","/n/work00/osanmasa/alice/AliPhysics","/PWGCF/Correlations/macros/jcorran/cardAlice_IAA_AA.input");
    TString jtrigg= "hadron";
    TString jassoc="hadron";
    TString cardSetting = "AnalyseMCTruth=1;EfficiencyMode=0;TriggerMask=-1;CentBinBorders=0,10;zVertBins=-10,10;HadronSelectionCut=0;EventPoolDepth=100";

    // === Set up JCard ====
    AliJCard *card = new AliJCard(cardName.Data());
    card->PrintOut();
    card->ReadLine( cardSetting.Data() );
    card->ReCompile();
    card->PrintOut();

    // === Create analysis object ===

    AliJIaaAna *fAna;
    fAna = new AliJIaaAna( kFALSE );

    fAna->SetCard( card );
    fAna->SetTrigger( jtrigg.Data() );
    fAna->SetAssoc( jassoc.Data() );

    fAna->UserCreateOutputObjects();
    */


    //------------------------------------------------------------------
    // Define jet reconstruction
    //------------------------------------------------------------------
    TClonesArray *inputList = new TClonesArray("AliJBaseTrack",1500);

    double partMinPtCut         = 0.15;// atlas 0.5 cms/alice 0.15
    double partMinEtaCut        = 0.9;
    double coneR                = 0.4; // atlas 0.6, cms 0.7 alice 0.4
	double ktconeR              = 0.4;
	double fusePionMassInktjets = false;
    double fuseDeltaPhiBGSubtr  = false;
    double jetConstituentCut    = 5.0;
    double dijetSubleadingPt    = 20.0;
    double dijetDeltaPhiCut     = 2.0; // Cut is pi/dijetDeltaPhiCut
    int centBin=0;
    int fktScheme               = 1;

    TString sktScheme;
    switch (fktScheme) {
        case 0:  sktScheme = "E_scheme";
                 break;
        case 1:  sktScheme = "pt_scheme";
                 break;
        case 2:  sktScheme = "pt2_scheme";
                 break;
        case 3:  sktScheme = "Et_scheme";
                 break;
        case 4:  sktScheme = "Et2_scheme";
                 break;
        case 5:  sktScheme = "BIpt_scheme";
                 break;
        case 6:  sktScheme = "BIpt2_scheme";
                 break;
        default: sktScheme = "Unknown, check macro arguments!";
                 break;
    }

    cout << endl;
    cout << "============= Settings =============" << endl;
    cout << "cent bin:                   " << centBin << endl;
    cout << "particle eta cut:           " << partMinEtaCut << endl;
    cout << "particle pt cut:            " << Form("%.2f",partMinPtCut) << endl;
    cout << "jet cone size:              " << coneR << endl;
    cout << "kt-jet cone size:           " << ktconeR << endl;
    cout << "Using pion mass in kt-jets: " << fusePionMassInktjets << endl;
    cout << "Using DeltaPhi in BG subtr: " << fuseDeltaPhiBGSubtr << endl;
    cout << "Using kt-jet scheme:        " << sktScheme.Data() << endl;
    cout << "jet costituent cut:         " << jetConstituentCut << endl;
    cout << "dijet leading pt cut:       " << dijetLeadingPt << endl;
    cout << "dijet subleading pt cut:    " << dijetSubleadingPt << endl;
    cout << "dijet DeltaPhi cut:         pi/" << dijetDeltaPhiCut << endl;
    cout << endl;

    if(fusePionMassInktjets && fktScheme!=0) {
        cout << "Warning: Using pion mass for kt-jets but not using E_scheme!" << endl;
        cout << endl;
    }

    // Save information about the settings used.
    fhistos->fh_info->Fill("Count", 1.0);
    fhistos->fh_info->Fill("MC", 1.0);
    fhistos->fh_info->Fill("Cent bin border 00", 0.0);
    fhistos->fh_info->Fill("Cent bin border 01", 5.0);
    fhistos->fh_info->Fill("Jet cone", coneR);
    fhistos->fh_info->Fill("kt-jet cone", ktconeR);
    fhistos->fh_info->Fill("Scheme", fktScheme);
    fhistos->fh_info->Fill("Use pion mass", fusePionMassInktjets);
    fhistos->fh_info->Fill("Particle eta cut", partMinEtaCut);
    fhistos->fh_info->Fill("Particle pt cut", partMinPtCut);
    fhistos->fh_info->Fill("Leading jet cut", dijetLeadingPt);
    fhistos->fh_info->Fill("Subleading jet cut", dijetSubleadingPt);
    fhistos->fh_info->Fill("Const. cut", jetConstituentCut);
    fhistos->fh_info->Fill("Delta phi cut pi/",dijetDeltaPhiCut);

    // Initialize fh_events so that the bin order is correct
     /*
    fhistos->fh_events[0]->Fill("events",0.0);
    fhistos->fh_events[0]->Fill("particles",0.0);
    fhistos->fh_events[0]->Fill("acc. particles",0.0);
    fhistos->fh_events[0]->Fill("no rho calc. events",0.0);
    fhistos->fh_events[0]->Fill("rho calc. events",0.0);
    fhistos->fh_events[0]->Fill("jets",0.0);
    fhistos->fh_events[0]->Fill("acc. jets",0.0);
    fhistos->fh_events[0]->Fill("const. cut jets",0.0);
    fhistos->fh_events[0]->Fill("bg. subtr. jets",0.0);
    fhistos->fh_events[0]->Fill("bg. subtr. const. cut jets",0.0);
    fhistos->fh_events[0]->Fill("kt-jets",0.0);
    fhistos->fh_events[0]->Fill("acc. kt-jets",0.0);
    fhistos->fh_events[0]->Fill("leading jet drop",0.0);
    fhistos->fh_events[0]->Fill("subleading jet drop",0.0);
    fhistos->fh_events[0]->Fill("raw dijets",0.0);
    fhistos->fh_events[0]->Fill("raw dijets leading cut",0.0);
    fhistos->fh_events[0]->Fill("raw acc. dijets",0.0);
    fhistos->fh_events[0]->Fill("raw deltaphi cut dijets",0.0);
    fhistos->fh_events[0]->Fill("bg. subtr. dijets",0.0);
    fhistos->fh_events[0]->Fill("bg. subtr. dijets leading cut",0.0);
    fhistos->fh_events[0]->Fill("bg. subtr. acc. dijets",0.0);
    fhistos->fh_events[0]->Fill("bg. subtr. deltaphi cut dijets",0.0);
    fhistos->fh_events[0]->Fill("bg. subtr. const. cut dijets",0.0);
    fhistos->fh_events[0]->Fill("bg. subtr. const. cut dijets leading cut",0.0);
    fhistos->fh_events[0]->Fill("bg. subtr. const. cut acc. dijets",0.0);
    fhistos->fh_events[0]->Fill("bg. subtr. const. cut deltaphi cut dijets",0.0);
    fhistos->fh_events[0]->Fill("const. cut dijets",0.0);
    fhistos->fh_events[0]->Fill("const. cut dijets leading cut",0.0);
    fhistos->fh_events[0]->Fill("const. cut acc. dijets",0.0);
    fhistos->fh_events[0]->Fill("const. cut deltaphi cut dijets",0.0);
    fhistos->fh_events[0]->Fill("kt dijets",0.0);
    fhistos->fh_events[0]->Fill("kt dijets leading cut",0.0);
    fhistos->fh_events[0]->Fill("kt acc. dijets",0.0);
    fhistos->fh_events[0]->Fill("kt deltaphi cut dijets",0.0);

    */

    fana->SetSettings(5,
                      partMinEtaCut,
                      partMinPtCut,
                      DBL_MAX, //particle pt cut max
                      coneR,
                      ktconeR,
                      fktScheme,
                      fktScheme, //antikt
                      fusePionMassInktjets,
                      fuseDeltaPhiBGSubtr,
                      jetConstituentCut,
                      dijetLeadingPt,
                      dijetSubleadingPt,
                      0, //jet min pt cut
                      dijetDeltaPhiCut,
                      0.3, //Matching
                      0.0,
                      true,
                      true);
    fana->InitHistos(fhistos, true, 2);


    //--------------------------------------------------------
    //         B e g i n    e v e n t    l o o p.
    //--------------------------------------------------------
    cout<<"Let's start" <<endl; 
    //int ieout = nEvent/20;
    //if (ieout<1) ieout=1;
    int EventCounter = 0;
    //Float_t sigmaGen = 0.0;
    Float_t ebeweight = 1.0;
    double crossSec = 0.0;
    double pt, eta;

    //HepMC::GenEvent* evt = ascii_in.read_next_event();

    //int iEvent=0;
    if(EventCounter%1000==0) cout << "evt: " << EventCounter << endl;
    while ( !text_input.failed() ) {
        HepMC3::GenEvent evt(HepMC3::Units::GEV,HepMC3::Units::MM);
        text_input.read_event(evt);
        if( text_input.failed() ) break;
        const std::vector<HepMC3::ConstGenParticlePtr> hadrons = GetHadrons(evt);
        if( EventCounter == 0 ) {
            cout << " First event: " << endl;
            HepMC3::Print::listing(evt);
            HepMC3::Print::content(evt);

            cout << " Testing attribute reading for the first event: " << endl;

            shared_ptr<GenCrossSection> cs = evt.attribute<GenCrossSection>("GenCrossSection");
            shared_ptr<GenHeavyIon>     hi = evt.attribute<GenHeavyIon>("GenHeavyIon");
            shared_ptr<GenPdfInfo>      pi = evt.attribute<GenPdfInfo>("GenPdfInfo");

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
        //iEvent++;
        inputList->Clear("C");
        ebeweight = 1.0; //no event-by-event weight at all. //sigmaGen/nTrial;
        hCrossSectionInfo->Fill(7.5,ebeweight);
        //if(iEvent % ieout == 0) cout << iEvent << "\t" << int(float(iEvent)/nEvent*100) << "%, nTried:" << nTried << ", nTrial:" << nTrial << ", sigma:" << sigmaGen << endl;
        
        // http://hepmc.web.cern.ch/hepmc/classHepMC3_1_1GenCrossSection.html
        // Is not implemented in 3.0.0
        //shared_ptr<GenCrossSection> cs = evt.attribute<GenCrossSection>("GenCrossSection");
        //cs->set_cross_section(-1.0,0.0);
        //double xSec = cs->xsec("cross_section");
        //double xSec = cs->xsec (0);
        //crossSec += xSec*1e-9; // From pb to mb
        //cout << "cross section: " << xSec*1e-9 << endl;

        int iParticles = 0;
        //for ( HepMC::GenEvent::particle_const_iterator p = evt.particles_begin(); p != evt.particles_end(); ++p ){
        //FOREACH ( GenParticlePtr &p, evt.particles() ) { // For this method, no need for (* ) around p.
        //for (auto p: evt.particles()) { // This method is recommended by hepmc: http://hepmc.web.cern.ch/hepmc/differences.html
        for (auto p : hadrons) {
            //cout << iParticles << " in_event: " << p->in_event() << endl;
            //Print::line(p);
            //
            //Need to skip all with status == 0 ?
            //if(p->pid()==0) continue;
            //cout << iParticles << " status: " << p->status() << ", pid: " << p->pid() << endl; //", charge: " << datacol.particle(HepPDT::ParticleID(p->pid()))->charge() << ", isHadron: " << datacol.particle(HepPDT::ParticleID(p->pid()))->isHadron() << endl;
            //if(p->status()>1 && p->pid()!=0 && !datacol.particle(HepPDT::ParticleID(p->pid()))->isHadron()) cout << "A non-hadron: " << iParticles << " status: " << p->status() << ", pid: " << p->pid() << endl;
            //if(p->status()>1 && p->status()<81 && p->status()!=62) cout << "Special status: " << iParticles << " status: " << p->status() << ", pid: " << p->pid() << endl;
            //if (    p->status() > 79 && true){                                              // Check if particle is final
            if ( datacol.particle(HepPDT::ParticleID(p->pid()))->charge() != 0 ) { // Only charged particles are used 
                        TLorentzVector lParticle(p->momentum().px(), p->momentum().py(), p->momentum().pz(), p->momentum().e());
                        //cout << "Particle: " << p->momentum().px() << ", " <<  p->momentum().py() << ", " << p->momentum().pz() << ", " << p->momentum().e() << endl;
                        AliJBaseTrack track( lParticle );
                        pt = track.Pt();
                        eta = track.Eta();
                        if (pt>partMinPtCut && TMath::Abs(eta) < partMinEtaCut){
                            track.SetID(p->pid());
                            track.SetTrackEff(1.);
                            new ((*inputList)[inputList->GetEntriesFast()]) AliJBaseTrack(track);
                        }
                        iParticles++;
                    }
            } // end of finalparticles

        // Here I call my function
        fana->CalculateJets(inputList, fhistos, 0, ebeweight);
        fana->FillJetsDijets(fhistos, 0, ebeweight);

        // Next Iaa analysis
        /*
        fAna->SetTrackList(inputList);
        //fAna->GetCard()->WriteCard(fOutput); // fOutput is what exactly?
        fAna->SetRunNumber(EventCounter);
        fAna->SetCentrality(5);
        fAna->SetZVertex(0.0); // same
        fAna->UserExec();
        */


        //delete evt;
        //ascii_in >> evt;
        EventCounter++;
        //if(EventCounter == 1) break;
        //if(iEvent == nEvent-1) cout << nEvent << "\t" << "100%, nTried:" << pythia.info.nTried() << ", sigma:" << pythia.info.sigmaGen() << endl ;
    }//event loop

    fhistos->fh_events[0]->Fill("events",EventCounter);
    cout << "Total cross sec: " << crossSec << ", total events: " << EventCounter << ", ratio: " << crossSec/EventCounter << endl;
    hCrossSectionInfo->Fill(2.5,crossSec/EventCounter);
    hCrossSectionInfo->Fill(3.5,crossSec/EventCounter);
    hCrossSectionInfo->Fill(5.5,1); // for counting # of merged

    /*
       nTried = pythia.info.nTried();
       nAccepted = pythia.info.nAccepted();
       sigmaGen = pythia.info.sigmaGen();
       hCrossSectionInfo->Fill(0.5,nTried);
       cout << "nTried after loop:" << nTried << endl;// print also inside event loop and in the macro.
       hCrossSectionInfo->Fill(1.5,nAccepted);
    //cout << "nAccepted after loop:" << nAccepted << endl;
    hCrossSectionInfo->Fill(2.5,sigmaGen);
    cout << "sigma after loop:" << sigmaGen << endl;
    hCrossSectionInfo->Fill(3.5,EventCounter);
    hCrossSectionInfo->Fill(4.5,energy);
    hCrossSectionInfo->Fill(5.5,1); // for counting # of merged
    hCrossSectionInfo->Fill(6.5,pythia.info.weightSum()); // for counting # of merged
    */

    fout->Write();
    fout->Close();
    cout << EventCounter << " events are analyzed successfully."<< endl;
    timer.Print(); 
    return 0;
}



// From JETSCAPE-analysis c++ branch:
// Courtesy of James Mulligan
//-----------------------------------------------------------------
// Get list of hadrons.
// Final state hadrons (from jet + bulk) are stored as outgoing particles in a disjoint vertex with t = 100
std::vector<HepMC3::ConstGenParticlePtr> GetHadrons(const HepMC3::GenEvent &event) {
  
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
    //cout << pid << endl;
    if (pid!=12 && pid!=14 && pid!=16) {
      hadrons.push_back(particle);
    }
    
  }
  
  return hadrons;
  
}

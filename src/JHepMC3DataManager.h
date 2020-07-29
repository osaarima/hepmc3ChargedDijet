#ifndef JHEPMCDATAMANAGER_H
#define JHEPMCDATAMANAGER_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

#ifndef ROOT_TObject
#include <TObject.h>
#endif

#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>
//#include <HepMC3/IO_GenEvent.h>
//#include <HepMC3/GenEvent.h>

#include <HepMC3/GenEvent.h>
#include <HepMC3/ReaderAscii.h>
#include <HepMC3/GenParticle_fwd.h>
#include <HepMC3/Print.h>
#include <HepMC3/GenCrossSection.h>

#include  "AliJBaseTrack.h"

class JHepMC3DataManager  {

	public:
		//JHepMC3DataManager(char* hepMCFile);
        JHepMC3DataManager(TString hepMCFile);

		virtual ~JHepMC3DataManager(){;}		                    //destructor

        //virtual bool next_event();
		virtual bool getList(TClonesArray* listToFill);

	protected:
        //HepMC3::IO_GenEvent *fascii_in;
        HepMC3::ReaderAscii *ftext_input;
        //HepMC3::GenEvent* fevt;
        int finternalEvtCounter;

        std::vector<HepMC3::ConstGenParticlePtr> GetHadrons(const HepMC3::GenEvent &event);
};

#endif


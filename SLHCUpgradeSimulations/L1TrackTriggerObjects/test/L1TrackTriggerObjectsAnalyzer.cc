// -*- C++ -*-
//
// Package:    L1TrackTriggerObjectsAnalyzer
// Class:      L1TrackTriggerObjectsAnalyzer
// 
/**\class L1TrackTriggerObjectsAnalyzer L1TrackTriggerObjectsAnalyzer.cc SLHCUpgradeSimulations/L1TrackTriggerObjectsAnalyzer/src/L1TrackTriggerObjectsAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Emmanuelle Perez,40 1-A28,+41227671915,
//         Created:  Thu Nov 14 11:22:13 CET 2013
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"


// Gen-level stuff:
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include "DataFormats/L1Trigger/interface/L1TrackPrimaryVertex.h"
#include "DataFormats/L1Trigger/interface/L1TrackEtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1TrackEtMissParticleFwd.h"


#include "TFile.h"
#include "TH1F.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

using namespace l1extra;



//
// class declaration
//

class L1TrackTriggerObjectsAnalyzer : public edm::EDAnalyzer {
   public:
      explicit L1TrackTriggerObjectsAnalyzer(const edm::ParameterSet&);
      ~L1TrackTriggerObjectsAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      //virtual void endRun(edm::Run const&, edm::EventSetup const&);
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

      // ----------member data ---------------------------

	// to test the L1TrackPrimaryVertex :
	edm::InputTag L1VtxLabel;
	TH1F* h_zgen;
	TH1F* h_dz1;
	TH1F* h_dz2;

	// for L1TrackEtmiss:
	edm::InputTag L1EtMissLabel ;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
L1TrackTriggerObjectsAnalyzer::L1TrackTriggerObjectsAnalyzer(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed

  edm::Service<TFileService> fs;
  int nbins = 25;
  float x1=-25.;
  float x2 = 25.;
   h_zgen = fs -> make<TH1F>("h_zgen",";generated z_{vtx} (cm); Evts",nbins, x1,x2);
   nbins=100;
   x1 = -2;
   x2 = 2;
   h_dz1 = fs -> make<TH1F>("h_dz1",";z_{L1} - z_{gen} (cm); Evts",nbins,x1,x2);
   h_dz2 = fs -> make<TH1F>("h_dz2",";z_{L1} - z_{gen} (cm); Evts",nbins, x1, x2);

  L1VtxLabel = iConfig.getParameter<edm::InputTag>("L1VtxLabel") ;
  L1EtMissLabel = iConfig.getParameter<edm::InputTag>("L1EtMissLabel");

}


L1TrackTriggerObjectsAnalyzer::~L1TrackTriggerObjectsAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
L1TrackTriggerObjectsAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;


	// First, retrieve the generated primary vertex


  edm::Handle<edm::HepMCProduct> HepMCEvt;
  iEvent.getByLabel("generator",HepMCEvt);

 const HepMC::GenEvent* MCEvt = HepMCEvt->GetEvent();
 const double mm=0.1;

 float zvtx_gen = -999;
 for ( HepMC::GenEvent::vertex_const_iterator ivertex = MCEvt->vertices_begin(); ivertex != MCEvt->vertices_end(); ++ivertex )
         {
             bool hasParentVertex = false;
 
             // Loop over the parents looking to see if they are coming from a production vertex
             for (
                 HepMC::GenVertex::particle_iterator iparent = (*ivertex)->particles_begin(HepMC::parents);
                 iparent != (*ivertex)->particles_end(HepMC::parents);
                 ++iparent
             )
                 if ( (*iparent)->production_vertex() )
                 {
                     hasParentVertex = true;
                     break;
                }
 
             // Reject those vertices with parent vertices
             if (hasParentVertex) continue;
 
             // Get the position of the vertex
             HepMC::FourVector pos = (*ivertex)->position();
	     zvtx_gen = pos.z()*mm; 

	     break;  // there should be one single primary vertex

          }  // end loop over gen vertices

 h_zgen -> Fill( zvtx_gen );


	// ----------------------------------------------------------------------
	//
        // retrieve the L1 vertices
        
 edm::Handle<L1TrackPrimaryVertexCollection> L1VertexHandle;
 iEvent.getByLabel(L1VtxLabel,L1VertexHandle);
 
 std::vector<L1TrackPrimaryVertex>::const_iterator vtxIter;
 
 int ivtx = 0;
	// several algorithms have been run in the L1TrackPrimaryVertexProducer
	// hence there is a collection of L1 primary vertices.
	// the first one is probably the most reliable.

 for (vtxIter = L1VertexHandle->begin(); vtxIter != L1VertexHandle->end(); ++vtxIter) {
    float z = vtxIter -> getZvertex();
    float sum = vtxIter -> getSum();
    std::cout << " a vertex with  z = sum " << z << " " << sum << std::endl;
    ivtx ++;
    if (ivtx == 1) h_dz1 -> Fill( z - zvtx_gen) ;
    if (ivtx == 2) h_dz2 -> Fill( z - zvtx_gen);
 }  

	//
        // ----------------------------------------------------------------------
	// retrieve the EtMiss objects
	//

 edm::Handle<L1TrackEtMissParticleCollection> L1TrackEtMissHandle;
 iEvent.getByLabel(L1EtMissLabel, L1TrackEtMissHandle);

 std::vector<L1TrackEtMissParticle>::const_iterator etmIter;
 for (etmIter = L1TrackEtMissHandle -> begin(); etmIter != L1TrackEtMissHandle->end(); ++etmIter) {
	float etmis = etmIter -> et();
	const edm::Ref< L1TrackPrimaryVertexCollection > vtxRef = etmIter -> getVtxRef();
	float zvtx = vtxRef -> getZvertex();
	std::cout << " ETmiss = " << etmis << " for zvtx = " << zvtx << std::endl;
 }

}


// ------------ method called once each job just before starting event loop  ------------
void 
L1TrackTriggerObjectsAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
L1TrackTriggerObjectsAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
L1TrackTriggerObjectsAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
L1TrackTriggerObjectsAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
L1TrackTriggerObjectsAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
L1TrackTriggerObjectsAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
L1TrackTriggerObjectsAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(L1TrackTriggerObjectsAnalyzer);

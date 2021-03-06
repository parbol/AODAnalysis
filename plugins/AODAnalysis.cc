#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//framework includes
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/DTWireId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"
#include "DataFormats/SiStripDetId/interface/SiStripDetId.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/IsolatedTrack.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/PFIsolation.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetEnumerators.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "RecoMuon/DetLayers/interface/MuonDetLayerGeometry.h"
#include "RecoMuon/TransientTrackingRecHit/interface/MuonTransientTrackingRecHitBuilder.h"
#include "RecoMuon/TransientTrackingRecHit/interface/MuonTransientTrackingRecHit.h"
#include "RecoMuon/Records/interface/MuonRecoGeometryRecord.h"

// muon info
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"

#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetType.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetType.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "RecoMuon/TrackingTools/interface/MuonServiceProxy.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include "TLorentzVector.h"
#include "TTree.h"
#include "TFile.h"

//=======================================================================================================================================================================================================================//

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// FUNCTIONS ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// DATA DEFINITION //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////// BRANCHES /////////////////////////////////////

//-> EVENT INFO
Int_t Event_event;
Int_t Event_luminosityBlock;
Int_t Event_run;


//-> MUON INFO
Int_t   nMuons;

std::vector<float> Muon_pt;
std::vector<float> Muon_eta;
std::vector<float> Muon_phi;
std::vector<int>   nHits;
std::vector<int> Hit_muonLink;
std::vector<float> Hit_lpX; //local point
std::vector<float> Hit_lpY;
std::vector<float> Hit_lpZ;
std::vector<float> Hit_gpX; //global point
std::vector<float> Hit_gpY;
std::vector<float> Hit_gpZ;

/////////////////////////////////////// OUTPUT //////////////////////////////////////

TFile *file_out;
TTree *tree_out;

//=======================================================================================================================================================================================================================//
class AODAnalysis : public edm::one::EDAnalyzer<edm::one::SharedResources>  {

   public:
      explicit AODAnalysis(const edm::ParameterSet&);
      ~AODAnalysis();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      std::string output_filename;
      edm::ParameterSet parameters;

      edm::EDGetTokenT<edm::View<reco::Muon> > theMuonCollection;
      // edm::EDGetTokenT<edm::View<reco::Track> > thePickyTrackCollection;
      // edm::EDGetTokenT<edm::View<reco::Track> > theTPFMSTrackCollection;
      // edm::EDGetTokenT<edm::View<reco::Track> > thedytTrackCollection;
      // edm::EDGetTokenT<edm::View<reco::Track> > theGlobalTrackCollection;
  
};

//=======================================================================================================================================================================================================================//
AODAnalysis::AODAnalysis(const edm::ParameterSet& iConfig)
{

   usesResource("TFileService");
   
   parameters = iConfig;

   theMuonCollection = consumes<edm::View<reco::Muon> >  (parameters.getParameter<edm::InputTag>("MuonCollection"));
   // thePickyTrackCollection = consumes<edm::View<reco::Track> >  (parameters.getParameter<edm::InputTag>("PickyTrackCollection"));
   // theTPFMSTrackCollection = consumes<edm::View<reco::Track> >  (parameters.getParameter<edm::InputTag>("TPFMSTrackCollection"));
   // thedytTrackCollection = consumes<edm::View<reco::Track> >  (parameters.getParameter<edm::InputTag>("dytTrackCollection"));
   // theGlobalTrackCollection = consumes<edm::View<reco::Track> >  (parameters.getParameter<edm::InputTag>("GlobalTrackCollection"));

   
}

//=======================================================================================================================================================================================================================//
AODAnalysis::~AODAnalysis()
{

}

//=======================================================================================================================================================================================================================//
void AODAnalysis::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
   /////////////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////// MAIN CODE /////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////////

   //////////////////////////////// GET THE COLLECTIONS ////////////////////////////////
   
   edm::Handle<edm::View<reco::Muon> > muons;
   iEvent.getByToken(theMuonCollection, muons);


   edm::ESHandle<TrackerGeometry> geom;
   iSetup.get<TrackerDigiGeometryRecord>().get( geom );
   const TrackerGeometry& theTracker(*geom);



   // edm::Handle<edm::View<reco::Track> > pickytrack;
   // iEvent.getByToken(thePickyTrackCollection, pickytrack);

   // edm::Handle<edm::View<reco::Track> > TPFMStrack;
   // iEvent.getByToken(theTPFMSTrackCollection, TPFMStrack);

   // edm::Handle<edm::View<reco::Track> > dyttrack;
   // iEvent.getByToken(thedytTrackCollection, dyttrack);

   // edm::Handle<edm::View<reco::Track> > globaltrack;
   // iEvent.getByToken(theGlobalTrackCollection, globaltrack);

   /////////////////////////////////// EVENT INFO //////////////////////////////////////

   Event_event = iEvent.id().event();
   Event_run = iEvent.id().run();
   Event_luminosityBlock = iEvent.id().luminosityBlock();

   ////////////////////////////// MUON FEATURES //////////////////////////////

   nMuons = muons->size();
 
   /////////////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////// GET MUON VARIABLES ////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////////

   bool useGlobal = false;
   bool useInner = false;
   bool usePicky = false;
   bool useDYT = true;
   
   for (auto itmuon=muons->begin(); itmuon != muons->end(); itmuon++){

     int iMuon = itmuon - muons->begin();
     
     //if(itmuon->isHighPtMuon() != true){iMuon--;continue;}
     //if(itmuon->isGlobalMuon() != true){iMuon--;continue;}

     std::cout << "HOLA MUON " << iMuon << std::endl;

     //bool globalExists=false; 
     //bool innerExists=false; 
     //bool pickyExists=false; 
     //bool dytExists=false; 

     if(itmuon->globalTrack().isNonnull()){
       std::cout << "Global dpt/pt = " << itmuon->globalTrack()->ptError()/itmuon->globalTrack()->pt() << std::endl;
       std::cout << "chi2/ndof = " << itmuon->globalTrack()->chi2()/(float)itmuon->globalTrack()->ndof() << std::endl;
       //globalExists=true;
     }
     
     if(itmuon->innerTrack().isNonnull()){
       std::cout << "Inner dpt/pt = " << itmuon->innerTrack()->ptError()/itmuon->innerTrack()->pt() << std::endl;
       std::cout << "chi2/ndof = " << itmuon->innerTrack()->chi2()/(float)itmuon->innerTrack()->ndof() << std::endl;
       //innerExists=true;
     }

     if(itmuon->pickyTrack().isNonnull()) {
       std::cout << "Picky dpt/pt = " << itmuon->pickyTrack()->ptError()/itmuon->pickyTrack()->pt() << std::endl; 
       std::cout << "chi2/ndof = " << itmuon->pickyTrack()->chi2()/(float)itmuon->pickyTrack()->ndof() << std::endl;
       //pickyExists=true;
     }


     if(itmuon->dytTrack().isNonnull()) {
       std::cout << "DYT dpt/pt = " << itmuon->dytTrack()->ptError()/itmuon->dytTrack()->pt() << std::endl; 
       std::cout << "chi2/ndof = " << itmuon->dytTrack()->chi2()/(float)itmuon->dytTrack()->ndof() << std::endl;
       //dytExists=true;
     }
     

     Muon_pt.push_back(itmuon->pt());
     Muon_eta.push_back(itmuon->eta());
     Muon_phi.push_back(itmuon->phi());
   

   
     //int nhits = 0;
     if(itmuon->innerTrack().isNonnull()) {
       for (auto itHit = itmuon->innerTrack()->recHitsBegin(); itHit != itmuon->innerTrack()->recHitsEnd(); itHit++) {

	 DetId detid = (*itHit)->geographicalId(); 

	 LocalPoint lp = (*itHit)->localPosition();
	 //LocalError le = (*itHit)->localPositionError();

	 Hit_lpX.push_back(lp.x());
	 Hit_lpY.push_back(lp.y());
	 Hit_lpZ.push_back(lp.z());

	 
	 if (detid.det() == DetId::Tracker) {

	   const PixelGeomDetUnit* theGeomDet = dynamic_cast<const PixelGeomDetUnit*> (theTracker.idToDet(detid) );

	    //const StripGeomDetUnit* theGeomDet = dynamic_cast<const StripGeomDetUnit*>( theTracker.idToDet(detid) );
	   //GlobalPoint GP = theGeomDet->surface().toGlobal(Local3DPoint(lp));
	   GlobalPoint gp = theGeomDet->surface().toGlobal(Local3DPoint(lp.x(),lp.y(),lp.z()));

	   Hit_gpX.push_back(gp.x());
	   Hit_gpY.push_back(gp.y());
	   Hit_gpZ.push_back(gp.z());

	   std::cout << "Tracker" << std::endl;
	 }else if (detid.det() == DetId::Muon) {
	   std::cout << "MuonSys" << std::endl;
	 }else {
	   continue;
	 }


       }
     }
   }
   // for (auto ittrack=dyttrack->begin(); ittrack != dyttrack->end(); ittrack++){
   //   for (auto itHit = ittrack->recHitsBegin(); itHit != ittrack->recHitsEnd(); itHit++) {
   //     std::cout << "HOLAAA" << std::endl;
   //   }
   // }

       
       // Hit_muonLink.push_back(iMuon);

       // int iHit = itHit - itmuon->dytTrack()->recHitsBegin();

       // DetId detid = (*itHit)->geographicalId(); 

       // //REMOVE MUON ID = TRACKER
       // // if (detid.det() != DetId::Muon) {
       // // 	 continue;
       // // }

       // LocalPoint lp = (*itHit)->localPosition();
       // float lpX= lp.x();
       // float lpY = lp.y();
       // float lpZ = lp.z();
       // Hit_lpX.push_back(lpX);
       // Hit_lpY.push_back(lpY);
       // Hit_lpZ.push_back(lpZ);

       // //float gpX; //FIXME!!!
       // //float gpY;
       // //float gpZ;

       // std::cout << " - HOLA HIT " <<  iHit << std::endl;
       // std::cout << "   THIS DETECTOR: " << detid.det() << " (MuonDet=2)" << std::endl;
       // std::cout << "   THIS SUBDETECTOR: " << detid.subdetId() << " (DT=1, CSC=2, RPC=3)" << std::endl;
       // std::cout << "   HITS LOCAL COORDS (x,y,z): " << "(" << lpX << "," << lpY << "," << lpZ << ")" << std::endl; 

       // if (detid.det() == DetId::Muon) {

       // 	 nhits++;	 
       // 	 int systemMuon  = detid.subdetId(); // 1 DT; 2 CSC; 3 RPC

       // 	 if ( systemMuon == MuonSubdetId::CSC) {
       // 	   CSCDetId id(detid.rawId());

       // 	   //edm::ESHandle<CSCGeometry> theCSCGeometry;
       // 	   //iSetup.get<MuonGeometryRecord>().get(theCSCGeometry);

       // 	   //const CSCGeometry& theCSCMuon(*theCSCGeometry);
       // 	   //DetId theDetUnitId((*itHit)->detUnitId());
       // 	   // const GeomDetUnit *theDet = theCSCMuon.idToDetUnit(theDetUnitId);
       // 	   //const GeomDetUnit *theDet = theCSCMuon.idToDetUnit(systemMuon);
       // 	   //const BoundPlane& bSurface = theDet->surface();
       // 	   // const Surface& surface = (*itHit)->detUnit()->surface();
       // 	   // gpX = surface.toGlobal((*itHit)->localPosition()).x();
       // 	   // gpY = surface.toGlobal((*itHit)->localPosition()).y();
       // 	   // gpZ = surface.toGlobal((*itHit)->localPosition()).z();

       // 	   printf("   CSC\t[endcap][station][ringN][chamber][layer]:[%d][%d][%d][%d][%d]\n",
       // 		  id.endcap(), id.station(), id.ring(), id.chamber(), id.layer());

       // 	 }

       // 	 else if ( systemMuon == MuonSubdetId::DT ) {
       // 	   DTWireId id(detid.rawId());
       // 	   printf("   DT \t[station][layer][superlayer][wheel][sector]:[%d][%d][%d][%d][%d]\n", id.station(),id.layer(),id.superLayer(), id.wheel(), id.sector());
	   
       // 	 }
       // 	 else if ( systemMuon == MuonSubdetId::RPC) {
       // 	   RPCDetId id(detid.rawId());
       // 	   printf("   RPC\t[station]:[%d]\n", id.station());
       // 	 }
       
       // 	 Hit_gpX.push_back(0.0); //FIXME!!!
       // 	 Hit_gpY.push_back(0.0);
       // 	 Hit_gpZ.push_back(0.0);
	 	 
       // }
       // else{continue;} //HIT NOT IN MUON DETECTOR  

     //}

   nHits.push_back(0);

     
   /////////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////// FILL THE TREE ///////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////////
   tree_out->Fill();
   Muon_pt.clear();
   Muon_eta.clear();
   Muon_phi.clear();
   nHits.clear();
   Hit_muonLink.clear();
   Hit_lpX.clear();
   Hit_lpY.clear();
   Hit_lpZ.clear();
   Hit_gpX.clear();
   Hit_gpY.clear();
   Hit_gpZ.clear();
     
}

//=======================================================================================================================================================================================================================//
void AODAnalysis::beginJob()
{

    // Output file definition
    output_filename = parameters.getParameter<std::string>("nameOfOutput");
    file_out = new TFile(output_filename.c_str(), "RECREATE");
    file_out->cd();

    std::cout << "the file is created" << std::endl;
    
    // Output Tree definition
    tree_out = new TTree("Events", "Events");

    // ///////////////////////////////// EVENT INFO BRANCHES ///////////////////////////////

    tree_out->Branch("Event_event", &Event_event, "Event_event/I");
    tree_out->Branch("Event_run", &Event_run, "Event_run/I");
    tree_out->Branch("Event_luminosityBlock", &Event_luminosityBlock, "Event_luminosityBlock/I");

    // ////////////////////////////// PRIMARY VERTEX BRANCHES //////////////////////////////

    tree_out->Branch("nMuons", &nMuons, "nMuons/I");
    tree_out->Branch("Muon_pt", "vector<float>", &Muon_pt);
    tree_out->Branch("Muon_eta", "vector<float>", &Muon_eta);
    tree_out->Branch("Muon_phi", "vector<float>", &Muon_phi);
    tree_out->Branch("nHits", "vector<int>", &nHits);
    tree_out->Branch("Hit_muonLink", "vector<int>", &Hit_muonLink);
    tree_out->Branch("Hit_lpX", "vector<float>", &Hit_lpX);
    tree_out->Branch("Hit_lpY", "vector<float>", &Hit_lpY);
    tree_out->Branch("Hit_lpZ", "vector<float>", &Hit_lpZ);
    tree_out->Branch("Hit_gpX", "vector<float>", &Hit_gpX);
    tree_out->Branch("Hit_gpY", "vector<float>", &Hit_gpY);
    tree_out->Branch("Hit_gpZ", "vector<float>", &Hit_gpZ);

}

//=======================================================================================================================================================================================================================//
void AODAnalysis::endJob() 
{

    std::cout << "The event is writen" << std::endl;
    file_out->cd();
    tree_out->Write();
    file_out->Close();

}

//=======================================================================================================================================================================================================================//
void AODAnalysis::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(AODAnalysis);

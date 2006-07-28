/*
 *  $Date: 2006/07/17 23:17:15 $
 *  $Revision: 1.10 $
 *  \author Julia Yarba
 */

#include <ostream>

#include "IOMC/ParticleGuns/interface/FlatRandomPtGunSource.h"

#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// #include "FWCore/Utilities/interface/Exception.h"

// #include "CLHEP/Random/RandFlat.h"

using namespace edm;
using namespace std;

FlatRandomPtGunSource::FlatRandomPtGunSource(const ParameterSet& pset,
                                             const InputSourceDescription& desc) : 
   BaseFlatGunSource(pset, desc)
{


   ParameterSet defpset ;
   ParameterSet pgun_params = 
      pset.getUntrackedParameter<ParameterSet>("PGunParameters",defpset) ;
  
   fMinPt = pgun_params.getUntrackedParameter<double>("MinPt",0.99);
   fMaxPt = pgun_params.getUntrackedParameter<double>("MaxPt",1.01);
  
  produces<HepMCProduct>();

  cout << "Internal FlatRandomPtGun is initialzed" << endl ;
  cout << "It is going to generate " << remainingEvents() << " events" << endl ;
   
}

FlatRandomPtGunSource::~FlatRandomPtGunSource()
{
   // no need to cleanup GenEvent memory - done in HepMCProduct
}

bool FlatRandomPtGunSource::produce(Event &e) 
{

   if ( fVerbosity > 0 )
   {
      cout << " FlatRandomPtGunSource : Begin New Event Generation" << endl ; 
   }
   // event loop (well, another step in it...)
          
   // no need to clean up GenEvent memory - done in HepMCProduct
   // 
   
   // here re-create fEvt (memory)
   //
   fEvt = new HepMC::GenEvent() ;
   
   // now actualy, cook up the event from PDGTable and gun parameters
   //
   // 1st, primary vertex
   //
   HepMC::GenVertex* Vtx = new HepMC::GenVertex(CLHEP::HepLorentzVector(0.,0.,0.));

   // loop over particles
   //
   int barcode = 1 ;
   for (unsigned int ip=0; ip<fPartIDs.size(); ++ip)
   {

       //double pt     = RandFlat::shoot(fMinPt, fMaxPt) ;
       //double eta    = RandFlat::shoot(fMinEta, fMaxEta) ;
       //double phi    = RandFlat::shoot(fMinPhi, fMaxPhi) ;
       double pt     = fRandomGenerator->fire(fMinPt, fMaxPt) ;
       double eta    = fRandomGenerator->fire(fMinEta, fMaxEta) ;
       double phi    = fRandomGenerator->fire(fMinPhi, fMaxPhi) ;
       int PartID = fPartIDs[ip] ;
       DefaultConfig::ParticleData* PData = fPDGTable->particle(HepPDT::ParticleID(abs(PartID))) ;
       double mass   = PData->mass().value() ;
       double theta  = 2.*atan(exp(-eta)) ;
       double mom    = pt/sin(theta) ;
       double px     = pt*cos(phi) ;
       double py     = pt*sin(phi) ;
       double pz     = mom*cos(theta) ;
       double energy2= mom*mom + mass*mass ;
       double energy = sqrt(energy2) ; 
       CLHEP::Hep3Vector p(px,py,pz) ;
       HepMC::GenParticle* Part = 
           new HepMC::GenParticle(CLHEP::HepLorentzVector(p,energy),PartID,1);
       Part->suggest_barcode( barcode ) ;
       barcode++ ;
       Vtx->add_particle_out(Part);

       if ( fAddAntiParticle )
       {
          CLHEP::Hep3Vector ap(-px,-py,-pz) ;
	  int APartID = -PartID ;
	  if ( PartID == 22 || PartID == 23 )
	  {
	     APartID = PartID ;
	  }	  
	  HepMC::GenParticle* APart =
	     new HepMC::GenParticle(CLHEP::HepLorentzVector(ap,energy),APartID,1);
	  APart->suggest_barcode( barcode ) ;
	  barcode++ ;
	  Vtx->add_particle_out(APart) ;
       }

   }

   fEvt->add_vertex(Vtx) ;
   fEvt->set_event_number(event()) ;
   fEvt->set_signal_process_id(20) ; 
        
   if ( fVerbosity > 0 )
   {
      fEvt->print() ;  
   }

   auto_ptr<HepMCProduct> BProduct(new HepMCProduct()) ;
   BProduct->addHepMCData( fEvt );
   e.put(BProduct);
    
   if ( fVerbosity > 0 )
   {
      // for testing purpose only
      // fEvt->print() ; // prints empty info after it's made into edm::Event
      cout << " FlatRandomPtGunSource : Event Generation Done " << endl;
   }

   return true;
}


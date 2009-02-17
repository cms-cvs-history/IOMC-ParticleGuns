#ifndef FlatBaseThetaGunProducer_H
#define FlatBaseThetaGunProducer_H

#include <string>

#include "HepPDT/defs.h"
#include "HepPDT/TableBuilder.hh"
#include "HepPDT/ParticleDataTable.hh"

#include "HepMC/GenEvent.h"

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Run.h"

#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandFlat.h"

#include <memory>
#include "boost/shared_ptr.hpp"

namespace edm {
  
class FlatBaseThetaGunProducer : public EDProducer {
  
  public:
    
    FlatBaseThetaGunProducer(const ParameterSet &);
    virtual ~FlatBaseThetaGunProducer();
    void beginJob( const edm::EventSetup& ) ;
    void endRun( edm::Run& r, const edm::EventSetup& ) ;

  private:
    
  protected :
  
    // non-virtuals ! this and only way !
    //
    // data members
    
    // gun particle(s) characteristics
    std::vector<int> fPartIDs ;
    double           fMinTheta ;
    double           fMaxTheta ;
    double           fMinPhi ;
    double           fMaxPhi ;

    // the event format itself
    HepMC::GenEvent* fEvt;

    // HepMC/HepPDT related things 
    // (for particle/event construction)
    ESHandle<HepPDT::ParticleDataTable> fPDGTable ;
            	    	
    int                     fVerbosity ;

    CLHEP::HepRandomEngine& fRandomEngine ;
    CLHEP::RandFlat*        fRandomGenerator; 
    
    bool                    fAddAntiParticle;
    
  };
} 

#endif

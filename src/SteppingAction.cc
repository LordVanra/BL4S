#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

// Initialize static members
G4int SteppingAction::fTotalHits = 0;
std::vector<SteppingAction::HitData> SteppingAction::fHitData;

SteppingAction::SteppingAction(const DetectorConstruction* detectorConstruction)
: G4UserSteppingAction(),
  fDetConstruction(detectorConstruction)
{ }

SteppingAction::~SteppingAction()
{ }

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  G4VPhysicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()
                                  ->GetVolume();
  
  if (!volume) return;
  
  if (volume->GetName() == "Scoring") {
    G4Track* track = step->GetTrack();
    
    // Only count protons
    if (track->GetDefinition()->GetParticleName() != "proton") return;

    // Only count primaries, not secondaries from hadronic interactions
    if (track->GetParentID() != 0) return;
    
    G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
    G4double kineticEnergy = step->GetPreStepPoint()->GetKineticEnergy();
    
    fTotalHits++;
    
    HitData hit;
    hit.x = position.x() / cm;
    hit.y = position.y() / cm;
    hit.z = position.z() / cm;
    hit.kineticEnergy = kineticEnergy / GeV;
    fHitData.push_back(hit);
    
    track->SetTrackStatus(fStopAndKill);
  }
}
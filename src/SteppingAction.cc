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
  // Get volume of the current step
  G4VPhysicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()
                                  ->GetVolume();
  
  if (!volume) return;
  
  // Check if we're in the scoring volume
  if (volume->GetName() == "Scoring") {
    // Get particle information
    G4Track* track = step->GetTrack();
    
    // Only count protons
    if (track->GetDefinition()->GetParticleName() != "proton") return;
    
    // Get position at the scoring plane
    G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
    
    // Get kinetic energy
    G4double kineticEnergy = step->GetPreStepPoint()->GetKineticEnergy();
    
    // Record the hit
    fTotalHits++;
    
    // Store detailed hit data
    HitData hit;
    hit.x = position.x() / cm;
    hit.y = position.y() / cm;
    hit.z = position.z() / cm;
    hit.kineticEnergy = kineticEnergy / GeV;
    fHitData.push_back(hit);
    
    // Kill the track so we don't count it multiple times
    track->SetTrackStatus(fStopAndKill);
  }
}

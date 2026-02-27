#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include <vector>

class DetectorConstruction;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(const DetectorConstruction* detectorConstruction);
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step* step);

    // Access to scoring data
    static G4int GetTotalHits() { return fTotalHits; }
    static void ResetTotalHits() { fTotalHits = 0; }
    
    // For detailed analysis
    struct HitData {
      G4double x;
      G4double y;
      G4double z;
      G4double kineticEnergy;
    };
    
    static const std::vector<HitData>& GetHitData() { return fHitData; }
    static void ResetHitData() { fHitData.clear(); }

  private:
    const DetectorConstruction* fDetConstruction;
    static G4int fTotalHits;
    static std::vector<HitData> fHitData;
};

#endif

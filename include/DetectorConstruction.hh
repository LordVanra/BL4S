#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4FieldManager;
class G4UniformMagField;
class DetectorMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

    void SetMagneticField(G4bool enable);
    void SetMaterialSlab(G4bool enable);
    
    G4bool GetMagneticFieldEnabled() const { return fMagneticFieldEnabled; }
    G4bool GetMaterialSlabEnabled() const { return fMaterialSlabEnabled; }

  private:
    G4LogicalVolume* fMagnetLogical;
    G4LogicalVolume* fSlabLogical;
    G4LogicalVolume* fScoringLogical;
    G4VPhysicalVolume* fWorldPhysical;
    G4FieldManager* fFieldManager;
    G4UniformMagField* fMagneticField;
    G4bool fMagneticFieldEnabled;
    G4bool fMaterialSlabEnabled;
    DetectorMessenger* fMessenger;
};

#endif
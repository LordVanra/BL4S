#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4FieldManager;
class G4UniformMagField;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

    // Methods to enable/disable components
    void SetMagneticField(G4bool enable);
    void SetMaterialSlab(G4bool enable);
    
    G4bool GetMagneticFieldEnabled() const { return fMagneticFieldEnabled; }
    G4bool GetMaterialSlabEnabled() const { return fMaterialSlabEnabled; }

  private:
    // Logical volumes
    G4LogicalVolume* fMagnetLogical;
    G4LogicalVolume* fSlabLogical;
    G4LogicalVolume* fScoringLogical;
    
    // Physical volumes
    G4VPhysicalVolume* fWorldPhysical;
    
    // Field manager
    G4FieldManager* fFieldManager;
    G4UniformMagField* fMagneticField;
    
    // Configuration flags
    G4bool fMagneticFieldEnabled;
    G4bool fMaterialSlabEnabled;
};

#endif

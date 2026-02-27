#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4TransportationManager.hh"
#include "G4Material.hh"

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
  fMagnetLogical(nullptr),
  fSlabLogical(nullptr),
  fScoringLogical(nullptr),
  fWorldPhysical(nullptr),
  fFieldManager(nullptr),
  fMagneticField(nullptr),
  fMagneticFieldEnabled(true),
  fMaterialSlabEnabled(true),
  fMessenger(nullptr)
{
  fMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{
  if (fMagneticField) delete fMagneticField;
  delete fMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double world_sizeXY = 5*m;
  G4double world_sizeZ  = 10*m;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

  G4Box* solidWorld =
    new G4Box("World",
             0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);

  G4LogicalVolume* logicWorld =
    new G4LogicalVolume(solidWorld,
                        world_mat,
                        "World");

  fWorldPhysical =
    new G4PVPlacement(0,                     // no rotation
                      G4ThreeVector(),       // at (0,0,0)
                      logicWorld,            // its logical volume
                      "World",               // its name
                      0,                     // its mother  volume
                      false,                 // no boolean operation
                      0,                     // copy number
                      checkOverlaps);        // overlaps checking

  //
  // Magnetic Field Volume (z = 0.5m to 1.5m)
  //
  G4Material* magnet_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidMagnet =
    new G4Box("Magnet",
             0.15*m,    // 30 cm wide (half-width = 15 cm)
             0.15*m,    // 30 cm tall (half-height = 15 cm)
             0.5*m);    // 1 m long (half-length = 50 cm)

  fMagnetLogical =
    new G4LogicalVolume(solidMagnet,
                        magnet_mat,
                        "Magnet");

  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, 1.0*m),  // centered at z = 1.0m
                    fMagnetLogical,
                    "Magnet",
                    logicWorld,
                    false,
                    0,
                    checkOverlaps);

  // Create magnetic field (1.0 Tesla in y-direction)
  fMagneticField = new G4UniformMagField(G4ThreeVector(0., 1.0*tesla, 0.));
  fFieldManager = new G4FieldManager();
  fFieldManager->SetDetectorField(fMagneticField);
  fFieldManager->CreateChordFinder(fMagneticField);
  
  if (fMagneticFieldEnabled) {
    fMagnetLogical->SetFieldManager(fFieldManager, true);
  }

  //
  // Material Slab - Polyethylene (z = 2.0m to 2.1m)
  //
  // Define polyethylene (HDPE)
  G4Material* polyethylene = new G4Material("Polyethylene", 0.94*g/cm3, 2);
  polyethylene->AddElement(nist->FindOrBuildElement("H"), 4);
  polyethylene->AddElement(nist->FindOrBuildElement("C"), 2);
  
  G4Material* slab_mat = fMaterialSlabEnabled ? polyethylene : world_mat;

  G4Box* solidSlab =
    new G4Box("Slab",
             0.5*m,     // 1 m wide
             0.5*m,     // 1 m tall
             0.05*m);   // 10 cm thick

  fSlabLogical =
    new G4LogicalVolume(solidSlab,
                        slab_mat,
                        "Slab");

  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, 2.05*m),  // centered at z = 2.05m
                    fSlabLogical,
                    "Slab",
                    logicWorld,
                    false,
                    0,
                    checkOverlaps);

  //
  // Scoring Plane (z = 3.5m)
  //
  G4Material* scoring_mat = nist->FindOrBuildMaterial("G4_AIR");

  G4Box* solidScoring =
    new G4Box("Scoring",
             1.0*m,      // 2 m wide
             1.0*m,      // 2 m tall
             0.001*m);   // 2 mm thick (thin detector)

  fScoringLogical =
    new G4LogicalVolume(solidScoring,
                        scoring_mat,
                        "Scoring");

  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, 3.5*m),
                    fScoringLogical,
                    "Scoring",
                    logicWorld,
                    false,
                    0,
                    checkOverlaps);

  return fWorldPhysical;
}

void DetectorConstruction::SetMagneticField(G4bool enable)
{
  fMagneticFieldEnabled = enable;
  if (fMagnetLogical && fFieldManager) {
    if (enable) {
      fMagnetLogical->SetFieldManager(fFieldManager, true);
    } else {
      fMagnetLogical->SetFieldManager(nullptr, true);
    }
  }
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void DetectorConstruction::SetMaterialSlab(G4bool enable)
{
  fMaterialSlabEnabled = enable;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
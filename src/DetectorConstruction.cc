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
  fSlabMaterialName("Aluminum6061"),  // default
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
  G4NistManager* nist = G4NistManager::Instance();
  G4bool checkOverlaps = true;

  // World
  G4double world_sizeXY = 5*m;
  G4double world_sizeZ  = 10*m;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

  G4Box* solidWorld =
    new G4Box("World", 0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);
  G4LogicalVolume* logicWorld =
    new G4LogicalVolume(solidWorld, world_mat, "World");
  fWorldPhysical =
    new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World",
                      0, false, 0, checkOverlaps);

  // Magnetic Field Volume
  G4Material* magnet_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Box* solidMagnet =
    new G4Box("Magnet", 0.15*m, 0.15*m, 0.5*m);
  fMagnetLogical =
    new G4LogicalVolume(solidMagnet, magnet_mat, "Magnet");
  new G4PVPlacement(0, G4ThreeVector(0, 0, 1.0*m),
                    fMagnetLogical, "Magnet", logicWorld, false, 0, checkOverlaps);

  fMagneticField = new G4UniformMagField(G4ThreeVector(0., 1.0*tesla, 0.));
  fFieldManager = new G4FieldManager();
  fFieldManager->SetDetectorField(fMagneticField);
  fFieldManager->CreateChordFinder(fMagneticField);
  if (fMagneticFieldEnabled)
    fMagnetLogical->SetFieldManager(fFieldManager, true);

  // Define polycarbonate (C16H14O3)
  G4Material* polycarbonate = nullptr;
  if (!G4Material::GetMaterial("Polycarbonate", false)) {
    polycarbonate = new G4Material("Polycarbonate", 1.20*g/cm3, 3);
    polycarbonate->AddElement(nist->FindOrBuildElement("C"), 16);
    polycarbonate->AddElement(nist->FindOrBuildElement("H"), 14);
    polycarbonate->AddElement(nist->FindOrBuildElement("O"), 3);
  } else {
    polycarbonate = G4Material::GetMaterial("Polycarbonate");
  }

  // Define Aluminum 6061
  G4Material* aluminum6061 = nullptr;
  if (!G4Material::GetMaterial("Aluminum6061", false)) {
    aluminum6061 = new G4Material("Aluminum6061", 2.70*g/cm3, 5);
    aluminum6061->AddElement(nist->FindOrBuildElement("Al"), 97.9*perCent);
    aluminum6061->AddElement(nist->FindOrBuildElement("Mg"), 1.0*perCent);
    aluminum6061->AddElement(nist->FindOrBuildElement("Si"), 0.6*perCent);
    aluminum6061->AddElement(nist->FindOrBuildElement("Cu"), 0.3*perCent);
    aluminum6061->AddElement(nist->FindOrBuildElement("Cr"), 0.2*perCent);
  } else {
    aluminum6061 = G4Material::GetMaterial("Aluminum6061");
  }

  // Select slab material by name
  G4Material* slab_material = G4Material::GetMaterial(fSlabMaterialName, false);
  if (!slab_material) {
    G4cerr << "WARNING: Material '" << fSlabMaterialName
           << "' not found, defaulting to Aluminum6061" << G4endl;
    slab_material = aluminum6061;
    fSlabMaterialName = "Aluminum6061";
  }

  G4Material* slab_mat = fMaterialSlabEnabled ? slab_material : world_mat;

  G4Box* solidSlab =
    new G4Box("Slab", 0.5*m, 0.5*m, 0.05*m);
  fSlabLogical =
    new G4LogicalVolume(solidSlab, slab_mat, "Slab");
  new G4PVPlacement(0, G4ThreeVector(0, 0, 2.05*m),
                    fSlabLogical, "Slab", logicWorld, false, 0, checkOverlaps);

  // Scoring Plane
  G4Material* scoring_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Box* solidScoring =
    new G4Box("Scoring", 2.0*m, 2.0*m, 0.001*m);
  fScoringLogical =
    new G4LogicalVolume(solidScoring, scoring_mat, "Scoring");
  new G4PVPlacement(0, G4ThreeVector(0, 0, 3.5*m),
                    fScoringLogical, "Scoring", logicWorld, false, 0, checkOverlaps);

  return fWorldPhysical;
}

void DetectorConstruction::SetSlabMaterial(const G4String& materialName)
{
  fSlabMaterialName = materialName;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetectorConstruction::SetMagneticField(G4bool enable)
{
  fMagneticFieldEnabled = enable;
  if (fMagnetLogical && fFieldManager) {
    fMagnetLogical->SetFieldManager(enable ? fFieldManager : nullptr, true);
  }
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void DetectorConstruction::SetMaterialSlab(G4bool enable)
{
  fMaterialSlabEnabled = enable;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
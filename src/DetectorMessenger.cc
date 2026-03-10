#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction* det)
: G4UImessenger(), fDetector(det)
{
  fDetDir = new G4UIdirectory("/detector/");
  fDetDir->SetGuidance("Detector control commands");

  fMagFieldCmd = new G4UIcmdWithABool("/detector/setMagneticField", this);
  fMagFieldCmd->SetGuidance("Enable or disable the magnetic field");
  fMagFieldCmd->SetParameterName("enable", false);
  fMagFieldCmd->AvailableForStates(G4State_Idle);

  fSlabCmd = new G4UIcmdWithABool("/detector/setMaterialSlab", this);
  fSlabCmd->SetGuidance("Enable or disable the material slab");
  fSlabCmd->SetParameterName("enable", false);
  fSlabCmd->AvailableForStates(G4State_Idle);

  fSlabMaterialCmd = new G4UIcmdWithAString("/detector/setSlabMaterial", this);
  fSlabMaterialCmd->SetGuidance("Set slab material: Aluminum6061 or Polycarbonate");
  fSlabMaterialCmd->SetParameterName("material", false);
  fSlabMaterialCmd->AvailableForStates(G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
  delete fMagFieldCmd;
  delete fSlabCmd;
  delete fSlabMaterialCmd;
  delete fDetDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fMagFieldCmd)
    fDetector->SetMagneticField(fMagFieldCmd->GetNewBoolValue(newValue));
  else if (command == fSlabCmd)
    fDetector->SetMaterialSlab(fSlabCmd->GetNewBoolValue(newValue));
  else if (command == fSlabMaterialCmd)
    fDetector->SetSlabMaterial(newValue);
}
#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction* det)
: G4UImessenger(), fDetector(det)
{
  fDetDir = new G4UIdirectory("/mydet/");
  fDetDir->SetGuidance("Detector control commands");

  fMagFieldCmd = new G4UIcmdWithABool("/mydet/setMagneticField", this);
  fMagFieldCmd->SetGuidance("Enable or disable the magnetic field");
  fMagFieldCmd->SetParameterName("enable", false);
  fMagFieldCmd->AvailableForStates(G4State_Idle);

  fSlabCmd = new G4UIcmdWithABool("/mydet/setMaterialSlab", this);
  fSlabCmd->SetGuidance("Enable or disable the material slab");
  fSlabCmd->SetParameterName("enable", false);
  fSlabCmd->AvailableForStates(G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
  delete fMagFieldCmd;
  delete fSlabCmd;
  delete fDetDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fMagFieldCmd)
    fDetector->SetMagneticField(fMagFieldCmd->GetNewBoolValue(newValue));
  else if (command == fSlabCmd)
    fDetector->SetMaterialSlab(fSlabCmd->GetNewBoolValue(newValue));
}
#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithAString;

class DetectorMessenger : public G4UImessenger
{
  public:
    DetectorMessenger(DetectorConstruction* det);
    virtual ~DetectorMessenger();

    virtual void SetNewValue(G4UIcommand* command, G4String newValue);

  private:
    DetectorConstruction*  fDetector;
    G4UIdirectory*         fDetDir;
    G4UIcmdWithABool*      fMagFieldCmd;
    G4UIcmdWithABool*      fSlabCmd;
    G4UIcmdWithAString*    fSlabMaterialCmd;
};

#endif
//
// Main program for Proton Transmission Simulation
//

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "FTFP_BERT.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv)
{
  // Construct the default run manager
  G4RunManager* runManager = new G4RunManager;

  // Set mandatory initialization classes
  DetectorConstruction* detector = new DetectorConstruction();
  runManager->SetUserInitialization(detector);

  // Physics list
  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  runManager->SetUserInitialization(physicsList);

  // User action initialization
  runManager->SetUserInitialization(new ActionInitialization(detector));

  // Initialize G4 kernel
  runManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (argc != 1) {
    // Batch mode - execute macro file
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }
  else {
    // Interactive mode - always start terminal UI
    G4UIExecutive* ui = new G4UIExecutive(argc, argv, "tcsh");
    ui->SessionStart();
    delete ui;
  }

  delete runManager;

  return 0;
}
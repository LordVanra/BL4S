#include "RunAction.hh"
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include <fstream>
#include <iomanip>
#include <sys/stat.h>

RunAction::RunAction(DetectorConstruction* detector)
: G4UserRunAction(), fDetector(detector)
{ }

RunAction::~RunAction()
{ }

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  SteppingAction::ResetHits();
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  G4int totalHits = SteppingAction::GetTotalHits();
  G4double transmission = (G4double)totalHits / (G4double)nofEvents;
  G4String materialName = fDetector->GetSlabMaterialName();

  G4cout << "End of Run [" << materialName << "]" << G4endl
         << " Number of events processed : " << nofEvents << G4endl
         << " Number of protons at scoring plane : " << totalHits << G4endl
         << " Transmission : " << std::fixed << std::setprecision(4) << transmission << G4endl;

  // transmission.csv — append with material column
  {
    const char* fname = "transmission.csv";
    struct stat st;
    bool isEmpty = (stat(fname, &st) != 0 || st.st_size == 0);
    std::ofstream summaryFile(fname, std::ios::app);
    if (isEmpty)
      summaryFile << "material,nEvents,nHits,transmission\n";
    summaryFile << materialName << "," << nofEvents << ","
                << totalHits << "," << std::fixed << std::setprecision(4)
                << transmission << "\n";
  }

  // hits.csv — named per material so both runs are preserved
  {
    G4String hitsFilename = "hits_" + materialName + ".csv";
    std::ofstream hitsFile(hitsFilename);
    hitsFile << "x_cm,y_cm,z_cm,kineticEnergy_GeV\n";
    const auto& hits = SteppingAction::GetHitData();
    for (const auto& hit : hits) {
      hitsFile << std::fixed << std::setprecision(4)
               << hit.x << "," << hit.y << "," << hit.z << ","
               << hit.kineticEnergy << "\n";
    }
    G4cout << " Hit data written to " << hitsFilename
           << " (" << hits.size() << " entries)" << G4endl;
  }
}
#include "RunAction.hh"
#include "SteppingAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include <fstream>
#include <sstream>
#include <iomanip>

RunAction::RunAction()
: G4UserRunAction()
{ }

RunAction::~RunAction()
{ }

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  
  // Reset hit data at the start of each run
  SteppingAction::ResetHits();
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  G4int totalHits = SteppingAction::GetTotalHits();
  G4double transmission = (G4double)totalHits / (G4double)nofEvents;

  G4cout << "End of Run" << G4endl
         << " Number of events processed : " << nofEvents << G4endl
         << " Number of protons at scoring plane : " << totalHits << G4endl
         << " Transmission : " << std::fixed << std::setprecision(4) << transmission << G4endl;

  {
    std::ofstream summaryFile("transmission.csv", std::ios::app);
    // Write header if file is new/empty
    summaryFile.seekp(0, std::ios::end);
    if (summaryFile.tellp() == 0) {
      summaryFile << "nEvents,nHits,transmission\n";
    }
    summaryFile << nofEvents << "," << totalHits << "," << transmission << "\n";
  }

  // Write per-hit data to hits.csv
  {
    std::ofstream hitsFile("hits.csv");
    hitsFile << "x_cm,y_cm,z_cm,kineticEnergy_GeV\n";
    const auto& hits = SteppingAction::GetHitData();
    for (const auto& hit : hits) {
      hitsFile << std::fixed << std::setprecision(4)
               << hit.x << "," << hit.y << "," << hit.z << ","
               << hit.kineticEnergy << "\n";
    }
    G4cout << " Hit data written to hits.csv (" << hits.size() << " entries)" << G4endl;
  }
}
#include "RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

RunAction::RunAction()
: G4UserRunAction()
{ }

RunAction::~RunAction()
{ }

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  G4cout << G4endl
         << "End of Run" << G4endl
         << " Number of events processed : " << nofEvents << G4endl
}

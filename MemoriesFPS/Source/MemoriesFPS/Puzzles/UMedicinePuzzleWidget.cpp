#include "UMedicinePuzzleWidget.h"
#include "AMedicinePuzzle.h"

void UMedicinePuzzleWidget::SetPuzzleActor(AMedicinePuzzle* Puzzle)
{
    PuzzleActor = Puzzle;

    // Initialise la séquence du joueur avec la même taille
    PlayerSequence = {1, 7, 3, 10, 4}; // temporaire
}

void UMedicinePuzzleWidget::SwapBottles(int32 IndexA, int32 IndexB)
{
    if (!PlayerSequence.IsValidIndex(IndexA) || !PlayerSequence.IsValidIndex(IndexB))
        return;

    int32 Temp = PlayerSequence[IndexA];
    PlayerSequence[IndexA] = PlayerSequence[IndexB];
    PlayerSequence[IndexB] = Temp;

    UE_LOG(LogTemp, Warning, TEXT("Swap %d <-> %d"), IndexA, IndexB);
}

void UMedicinePuzzleWidget::ConfirmPuzzle()
{
    if (PuzzleActor)
    {
        PuzzleActor->ValidatePuzzle(PlayerSequence);
    }
}

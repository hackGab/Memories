#include "UMedicinePuzzleWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "AMedicinePuzzle.h"

void UMedicinePuzzleWidget::SetPuzzleActor(AMedicinePuzzle* Puzzle)
{
    PuzzleActor = Puzzle;

    // On récupère la séquence initiale du puzzle
    if (PuzzleActor)
    {
        PlayerSequence = PuzzleActor->GetInitialSequence();
    }
}

void UMedicinePuzzleWidget::SwapBottles(int32 IndexA, int32 IndexB)
{
    if (!PlayerSequence.IsValidIndex(IndexA) || !PlayerSequence.IsValidIndex(IndexB))
        return;

    // Swap logique dans le tableau
    int32 Temp = PlayerSequence[IndexA];
    PlayerSequence[IndexA] = PlayerSequence[IndexB];
    PlayerSequence[IndexB] = Temp;

    UE_LOG(LogTemp, Warning, TEXT("Swap %d <-> %d"), IndexA, IndexB);

    // Swap visuel dans le GridPanel
    if (!BottleGrid)
    {
        UE_LOG(LogTemp, Error, TEXT("BottleGrid is NULL"));
        return;
    }

    UWidget* WidgetA = BottleGrid->GetChildAt(IndexA);
    UWidget* WidgetB = BottleGrid->GetChildAt(IndexB);

    if (!WidgetA || !WidgetB)
        return;

    UUniformGridSlot* SlotA = Cast<UUniformGridSlot>(WidgetA->Slot);
    UUniformGridSlot* SlotB = Cast<UUniformGridSlot>(WidgetB->Slot);

    if (!SlotA || !SlotB)
        return;

    // Récupérer les positions actuelles
    int32 RowA = SlotA->GetRow();
    int32 ColA = SlotA->GetColumn();

    int32 RowB = SlotB->GetRow();
    int32 ColB = SlotB->GetColumn();

    // Échanger les positions
    SlotA->SetRow(RowB);
    SlotA->SetColumn(ColB);

    SlotB->SetRow(RowA);
    SlotB->SetColumn(ColA);
}

void UMedicinePuzzleWidget::ConfirmPuzzle()
{
    if (!PuzzleActor)
        return;

    PuzzleActor->ValidateSequence(PlayerSequence);
}

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
    if (!BottleGrid) return;

    UWidget* WidgetA = BottleGrid->GetChildAt(IndexA);
    UWidget* WidgetB = BottleGrid->GetChildAt(IndexB);

    if (!WidgetA || !WidgetB) return;

    UUniformGridSlot* SlotA = Cast<UUniformGridSlot>(WidgetA->Slot);
    UUniformGridSlot* SlotB = Cast<UUniformGridSlot>(WidgetB->Slot);

    if (!SlotA || !SlotB) return;

    int32 RowA = SlotA->GetRow();
    int32 ColA = SlotA->GetColumn();
    int32 RowB = SlotB->GetRow();
    int32 ColB = SlotB->GetColumn();

    SlotA->SetRow(RowB);
    SlotA->SetColumn(ColB);
    SlotB->SetRow(RowA);
    SlotB->SetColumn(ColA);

    // 🔥 Ajoute ceci pour forcer le déplacement visuel
    BottleGrid->InvalidateLayoutAndVolatility();
    BottleGrid->ForceLayoutPrepass();
}


void UMedicinePuzzleWidget::ConfirmPuzzle()
{
    if (!PuzzleActor)
        return;

    PuzzleActor->ValidateSequence(PlayerSequence);
}

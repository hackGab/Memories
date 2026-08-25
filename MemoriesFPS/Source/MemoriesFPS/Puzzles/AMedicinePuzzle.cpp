#include "AMedicinePuzzle.h"
#include "UMedicinePuzzleWidget.h"
#include "Blueprint/UserWidget.h"

AMedicinePuzzle::AMedicinePuzzle()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMedicinePuzzle::BeginPlay()
{
    Super::BeginPlay();
}

void AMedicinePuzzle::Interact(APlayerController* PlayerController)
{
    if (!PuzzleWidgetClass) return;

    ActiveWidget = CreateWidget<UMedicinePuzzleWidget>(PlayerController, PuzzleWidgetClass);

    if (ActiveWidget)
    {
        ActiveWidget->AddToViewport();
        ActiveWidget->SetPuzzleActor(this);
    }
}

TArray<int32> AMedicinePuzzle::GetInitialSequence() const
{
    return CorrectSequence;
}

void AMedicinePuzzle::ValidateSequence(const TArray<int32>& PlayerSequence)
{
    CorrectCount = 0;

    for (int32 i = 0; i < PlayerSequence.Num(); i++)
    {
        if (PlayerSequence[i] == CorrectSequence[i])
        {
            CorrectCount++;
        }
    }

    bPuzzleCompleted = (CorrectCount == CorrectSequence.Num());

    if (bPuzzleCompleted)
    {
        UE_LOG(LogTemp, Warning, TEXT("Puzzle solved!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Incorrect sequence."));
    }
}

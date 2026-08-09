#include "AMedicinePuzzle.h"
#include "UMedicinePuzzleWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

AMedicinePuzzle::AMedicinePuzzle()
{
    PrimaryActorTick.bCanEverTick = false;

    // Exemple de séquence correcte
    CorrectSequence = {1, 7, 3, 10, 4};

    bPuzzleCompleted = false;
    CorrectCount = 0;
}

void AMedicinePuzzle::BeginPlay()
{
    Super::BeginPlay();
}

void AMedicinePuzzle::Interact(APlayerController* PlayerController)
{
    if (!PuzzleWidgetClass || !PlayerController)
        return;

    ActiveWidget = CreateWidget<UMedicinePuzzleWidget>(PlayerController, PuzzleWidgetClass);

    if (ActiveWidget)
    {
        ActiveWidget->AddToViewport();
        ActiveWidget->SetPuzzleActor(this);

        PlayerController->SetShowMouseCursor(true);
        PlayerController->SetInputMode(FInputModeUIOnly());
    }
}

void AMedicinePuzzle::ValidatePuzzle(const TArray<int32>& PlayerSequence)
{
    CorrectCount = 0;
    bPuzzleCompleted = true;

    for (int32 i = 0; i < CorrectSequence.Num(); i++)
    {
        if (PlayerSequence.IsValidIndex(i))
        {
            if (PlayerSequence[i] == CorrectSequence[i])
            {
                CorrectCount++;
            }
            else
            {
                bPuzzleCompleted = false;
            }
        }
    }

    if (bPuzzleCompleted)
    {
        UE_LOG(LogTemp, Warning, TEXT("Puzzle réussi !"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Puzzle raté. %d bouteilles bien placées."), CorrectCount);
    }
}

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AMedicinePuzzle.generated.h"

class UMedicinePuzzleWidget;

UCLASS()
class MEMORIESFPS_API AMedicinePuzzle : public AActor
{
    GENERATED_BODY()

public:
    AMedicinePuzzle();

    // Appelé par ton système d’interaction (E)
    UFUNCTION()
    void Interact(APlayerController* PlayerController);

    // Appelé par le widget quand le joueur clique sur "Confirmer"
    UFUNCTION(BlueprintCallable)
    void ValidatePuzzle(const TArray<int32>& PlayerSequence);

protected:
    virtual void BeginPlay() override;

    // La bonne séquence
    UPROPERTY(EditAnywhere, Category="Puzzle")
    TArray<int32> CorrectSequence;

    // UI
    UPROPERTY(EditAnywhere, Category="Puzzle")
    TSubclassOf<UMedicinePuzzleWidget> PuzzleWidgetClass;

    UPROPERTY()
    UMedicinePuzzleWidget* ActiveWidget;

    // Résultats
    UPROPERTY(BlueprintReadOnly)
    int32 CorrectCount;

    UPROPERTY(BlueprintReadOnly)
    bool bPuzzleCompleted;
};

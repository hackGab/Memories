#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"      
#include "UMedicinePuzzleWidget.generated.h"

class AMedicinePuzzle;

UCLASS()
class MEMORIESFPS_API UMedicinePuzzleWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // Ton GridPanel dans le widget Blueprint
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* BottleGrid;

    // Appelé par l’acteur quand le widget est créé
    UFUNCTION(BlueprintCallable)
    void SetPuzzleActor(AMedicinePuzzle* Puzzle);

    // Le joueur clique sur deux bouteilles → swap
    UFUNCTION(BlueprintCallable)
    void SwapBottles(int32 IndexA, int32 IndexB);

    // Bouton "Confirmer"
    UFUNCTION(BlueprintCallable)
    void ConfirmPuzzle();

protected:

    // Séquence actuelle du joueur
    UPROPERTY(BlueprintReadWrite)
    TArray<int32> PlayerSequence;

    // Référence vers l’acteur
    UPROPERTY()
    AMedicinePuzzle* PuzzleActor;
};

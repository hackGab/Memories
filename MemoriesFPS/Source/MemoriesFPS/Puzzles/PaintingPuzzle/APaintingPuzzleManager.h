// PaintingPuzzleManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintingSpot.h"
#include "PaintingPuzzleManager.generated.h"

UCLASS()
class YOURGAME_API APaintingPuzzleManager : public AActor
{
	GENERATED_BODY()

public:
	APaintingPuzzleManager();

protected:
	virtual void BeginPlay() override;

public:
	// Spots du puzzle (3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	TArray<APaintingSpot*> PaintingSpots;

	// Son à jouer quand puzzle complété
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	USoundBase* PuzzleSolvedSound;

	// Actor du tiroir
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	AActor* NightstandDrawerActor;

	// Validation
	UFUNCTION(BlueprintCallable, Category="Puzzle")
	void ValidatePuzzle();

	// Puzzle complété
	UFUNCTION(BlueprintCallable, Category="Puzzle")
	void OnPuzzleSolved();

	// Optionnel : randomisation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle|Random")
	TArray<int32> AllPossiblePaintingIDs;

	UFUNCTION(BlueprintCallable, Category="Puzzle|Random")
	void RandomizeSolution();
};

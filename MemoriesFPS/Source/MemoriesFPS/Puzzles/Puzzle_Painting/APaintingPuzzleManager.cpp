// PaintingPuzzleManager.cpp
#include "APaintingPuzzleManager.h"
#include "Kismet/GameplayStatics.h"
#include "ApaintingSpot.h"

APaintingPuzzleManager::APaintingPuzzleManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APaintingPuzzleManager::BeginPlay()
{
	Super::BeginPlay();

	// Optionnel : randomiser au début
	// RandomizeSolution();
}

void APaintingPuzzleManager::ValidatePuzzle()
{
	int32 CorrectCount = 0;

	for (APaintingSpot* Spot : PaintingSpots)
	{
		if (!Spot) continue;

		if (Spot->IsCorrect())
		{
			CorrectCount++;
		}
	}

	if (CorrectCount >= 3)
	{
		OnPuzzleSolved();
	}
}

void APaintingPuzzleManager::OnPuzzleSolved()
{
	if (PuzzleSolvedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PuzzleSolvedSound, GetActorLocation());
	}

	// Appeler une fonction sur le tiroir (via BP ou interface)
	if (NightstandDrawerActor)
	{
		// Exemple : appeler une fonction BlueprintImplementableEvent
		NightstandDrawerActor->CallFunctionByNameWithArguments(TEXT("OpenDrawer"), *GLog, nullptr, true);
	}
}

void APaintingPuzzleManager::RandomizeSolution()
{
	if (PaintingSpots.Num() == 0 || AllPossiblePaintingIDs.Num() < PaintingSpots.Num())
		return;

	// Shuffle simple
	TArray<int32> Shuffled = AllPossiblePaintingIDs;
	for (int32 i = 0; i < Shuffled.Num(); ++i)
	{
		int32 SwapIdx = FMath::RandRange(0, Shuffled.Num() - 1);
		Shuffled.Swap(i, SwapIdx);
	}

	for (int32 i = 0; i < PaintingSpots.Num(); ++i)
	{
		if (!PaintingSpots[i]) continue;
		PaintingSpots[i]->CorrectPaintingID = Shuffled[i];
		PaintingSpots[i]->RequiredRotationIndex = FMath::RandRange(0, 3);
	}
}

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UMedicinePuzzleSubsystem.generated.h"

UCLASS()
class MEMORIESFPS_API UMedicinePuzzleSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	// Compare deux séquences
	UFUNCTION(BlueprintCallable)
	bool ValidateSequence(const TArray<int32>& Correct, const TArray<int32>& Player, int32& OutCorrectCount);
};

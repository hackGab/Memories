#pragma once

#include "UObject/Interface.h"
#include "PuzzleUnlockInterface.generated.h"

UINTERFACE(BlueprintType)
class UPuzzleUnlockInterface : public UInterface
{
	GENERATED_BODY()
};

class IPuzzleUnlockInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Puzzle")
	void OnPuzzleUnlocked();
};

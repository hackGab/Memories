#include "UMedicinePuzzleSubsystem.h"

bool UMedicinePuzzleSubsystem::ValidateSequence(const TArray<int32>& Correct, const TArray<int32>& Player, int32& OutCorrectCount)
{
	OutCorrectCount = 0;
	bool bCompleted = true;

	for (int32 i = 0; i < Correct.Num(); i++)
	{
		if (Player.IsValidIndex(i))
		{
			if (Player[i] == Correct[i])
			{
				OutCorrectCount++;
			}
			else
			{
				bCompleted = false;
			}
		}
	}

	return bCompleted;
}

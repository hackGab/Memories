// By Gabriel Laplante on 2026-07-20

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MemoriesGameMode.generated.h"

UCLASS()
class MEMORIESFPS_API AMemoriesGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMemoriesGameMode();
	
protected:
	virtual void BeginPlay() override;
	
	/** La durant d'une partie, en secondes. 20min=1200sec */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Timer")
	int GameDuration = 1200;
	
	static void EndGame();
	
private:
	FTimerHandle GameTimerHandle;
	
	void TickGameTimer();
};

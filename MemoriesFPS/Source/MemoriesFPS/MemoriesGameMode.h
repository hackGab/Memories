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
	
	/** La durant d'une partie, en secondes. 45min=2700sec */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Timer")
	int GameDuration = 2700;
	
	/** Indice sonore à chaque tranche de X minutes. 5min=300sec (pour la dernière tranche, à chaque 30sec le son s'intensifie)  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Timer")
	int TimeWarningThreshold = 300; // 5 minutes
	
	
	static void EndGame();
	static void PlayTimeWarningSound(bool intensify);
	
	
	// TimeWarningSound properties
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeWarningSound")
	USoundBase* TimeWarningSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeWarningSound")
	float TimeWarningSoundPitch = 1.0f;
	
	UPROPERTY()
	UAudioComponent* TimeWarningAudioComponent;
	
	float CurrentVolume = 0.5f;
	
	void AudioProperties();*/
	
private:
	FTimerHandle GameTimerHandle;
	
	void TickGameTimer();
};

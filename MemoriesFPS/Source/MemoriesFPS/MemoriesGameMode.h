// Created By Gabriel Laplante on 2026-07-20

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MemoriesGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimeWarningEvent, bool, bIntensify, float, Intensity);

UCLASS()
class MEMORIESFPS_API AMemoriesGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMemoriesGameMode();
	
	void SetIsPuzzleCandleResolve(bool isResolve);
	bool GetIsPuzzleCandleResolve() const;
	
	void SetIsPuzzleHorlogeResolve(bool isResolve);
	bool GetIsPuzzleHorlogeResolve() const;
	
	UPROPERTY(BlueprintAssignable, Category ="Events")
	FOnTimeWarningEvent OnTimeWarningEvent;
	
	
protected:
	virtual void BeginPlay() override;
	
	/** La durant d'une partie, en secondes. 45min=2700sec */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Timer")
	int GameDuration = 2700;
	
	/** Indice sonore à chaque tranche de X minutes. 5min=300sec (pour la dernière tranche, à chaque 30sec le son s'intensifie)  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Timer")
	int TimeWarningThreshold = 300; // 5 minutes
	
	
	static void EndGame();
	void PlayTimeWarningSound(bool intensify) const;
	
	float GetIntensity(float TimeRemaining) const;
	
	
	// TimeWarningSound properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeWarningSound")
	USoundBase* TimeWarningSound;
	
	UPROPERTY()
	UAudioComponent* TimeWarningAudioComponent;
	
	void AudioProperties();
	
private:
	FTimerHandle GameTimerHandle;
	
	void TickGameTimer();	
	
	// Variable Puzzle résolut
	bool _isPuzzleCandleResolve;
	bool _isPuzzleHorlogeResolve;
};

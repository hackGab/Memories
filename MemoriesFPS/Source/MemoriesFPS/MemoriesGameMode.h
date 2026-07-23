// Created By Gabriel Laplante on 2026-07-20

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Puzzles/GamePuzzleChandelle.h"
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
	float GetIntensity(float TimeRemaining) const;
	void PlayTimeWarningSound(bool intensify) const;
	
	
	// TimeWarningSound properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeWarningSound")
	USoundBase* TimeWarningSound;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TimeWarningSound")
	float TimeWarningSoundPitch = 1.0f;
	
	// Plus le pitch augmente, plus le son est aigu. Seulement quand il reste moins de TimeWarningThreshold minutes, on peut jouer un son d'indice toutes les 30 secondes (en intensifiant le pitch)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeWarningSound")
	float PitchIntensity = 1.2f;
	
	
	UPROPERTY()
	UAudioComponent* TimeWarningAudioComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TimeWarningSound")
	float CurrentVolume = 0.5f;
	
	// Quand il reste moins de TimeWarningThreshold minutes, on peut jouer un son d'indice toutes les 30 secondes (en intensifiant le volume)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeWarningSound")
	float VolumeIntensity = 0.7f;
	
	void AudioProperties();
	
private:
	FTimerHandle GameTimerHandle;
	
	void TickGameTimer();
	
	/* Spawns all game puzzles in the level
	void SpawnGamePuzzles() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle Setup", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGamePuzzleChandelle> ChandellePuzzleClass; // Référence vers BP_GamePuzzleChandelle
	
	void SpawnChandellePuzzle(FActorSpawnParameters SpawnParams, FTransform SpawnTransform) const;
	*/
	
};

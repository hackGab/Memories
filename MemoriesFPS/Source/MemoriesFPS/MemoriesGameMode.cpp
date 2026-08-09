// Created By Gabriel Laplante on 2026-07-20

#include "MemoriesGameMode.h"
#include "MemoriesGameState.h"
#include "TimerManager.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Puzzles/GamePuzzleChandelle.h"


AMemoriesGameMode::AMemoriesGameMode()
{
	GameStateClass = AMemoriesGameState::StaticClass();
}


void AMemoriesGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	AMemoriesGameState* GS = GetGameState<AMemoriesGameState>();
	if (GS)
		GS->TimeRemaining = GameDuration;
	
	AudioProperties();
	
	GetWorld()->GetTimerManager().SetTimer(
		GameTimerHandle,
		this,
		&AMemoriesGameMode::TickGameTimer,
		1.0f,
		true
	);
	
	//SpawnGamePuzzles();
}

void AMemoriesGameMode::AudioProperties()
{
	if (TimeWarningSound)
	{
		TimeWarningAudioComponent = NewObject<UAudioComponent>(this);
		TimeWarningAudioComponent->bAutoActivate = false;
		TimeWarningAudioComponent->SetSound(TimeWarningSound);
		TimeWarningAudioComponent->SetPitchMultiplier(TimeWarningSoundPitch);
		TimeWarningAudioComponent->SetVolumeMultiplier(CurrentVolume);
		TimeWarningAudioComponent->RegisterComponent();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TimeWarningSound is not assigned in the MemoriesGameMode."));
	}
}

void AMemoriesGameMode::TickGameTimer()
{
	AMemoriesGameState* GS = GetGameState<AMemoriesGameState>();
	if (!GS) return;
	
	GS->TimeRemaining -= 1.0f;
	
	if (GS->TimeRemaining <= 0.0f)
	{
		//GameState->TimeRemaining = 0.0f;
		GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
		
		EndGame();
	}
	
	// Quand il reste moins de TimeWarningThreshold minutes, on peut jouer un son d'indice toutes les 30 secondes (en intensifiant le pitch)
	if (GS->TimeRemaining >= 0.0f && GS->TimeRemaining <= TimeWarningThreshold && FMath::Fmod(GS->TimeRemaining, 30.0f) == 0.0f)
		PlayTimeWarningSound(true);
	
	// Indice sonore à chaque tranche de X minutes (TimeWarningThreshold)
	else if (GS->TimeRemaining > 0.0f && FMath::Fmod(GS->TimeRemaining, TimeWarningThreshold) == 0.0f)
		PlayTimeWarningSound(false);
}


float AMemoriesGameMode::GetIntensity(float TimeRemaining) const
{
	// Normalise entre 0 et 1
	float Alpha = TimeRemaining / TimeWarningThreshold;
	Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

	// Courbe douce : plus on approche de 0, plus ça monte vite
	return 1.0f - FMath::Pow(Alpha, 2.0f);
}

void AMemoriesGameMode::PlayTimeWarningSound(bool intensify) const
{
	AMemoriesGameState* GS = GetGameState<AMemoriesGameState>();
	if (!GS || !TimeWarningAudioComponent) return;

	if (intensify)
	{
		float Intensity = GetIntensity(GS->TimeRemaining);

		float NewPitch = TimeWarningSoundPitch + Intensity * PitchIntensity;   
		float NewVolume = CurrentVolume + Intensity * VolumeIntensity;         

		TimeWarningAudioComponent->SetPitchMultiplier(NewPitch);
		TimeWarningAudioComponent->SetVolumeMultiplier(NewVolume);

		//UE_LOG(LogTemp, Warning, TEXT("Intensity=%f Pitch=%f Volume=%f"), Intensity, NewPitch, NewVolume);
	}

	TimeWarningAudioComponent->Play();
}


void AMemoriesGameMode::EndGame()
{
	UE_LOG(LogTemp, Log, TEXT("Game Over!"));
}



/* Spawns all game puzzles in the level.
void AMemoriesGameMode::SpawnGamePuzzles() const
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
	
	SpawnChandellePuzzle(SpawnParams, SpawnTransform);
}

void AMemoriesGameMode::SpawnChandellePuzzle(FActorSpawnParameters SpawnParams, FTransform SpawnTransform) const
{
	
	if (!ChandellePuzzleClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ChandellePuzzleClass is not assigned in MemoriesGameMode."));
		return;
	}
	
	AGamePuzzleChandelle* ChandellePuzzleInstance = GetWorld()->SpawnActor<AGamePuzzleChandelle>(
		ChandellePuzzleClass,
		SpawnTransform,
		SpawnParams
	);
	
	if (ChandellePuzzleInstance)
	{
		UE_LOG(LogTemp, Log, TEXT("Chandelle puzzle spawned successfully by MemoriesGameMode."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn the Chandelle puzzle."));
	}
}*/



// Puzzles
void AMemoriesGameMode::SetIsPuzzleCandleResolve(bool isResolve)
{
	this->_isPuzzleCandleResolve = isResolve;
}

bool AMemoriesGameMode::GetIsPuzzleCandleResolve() const
{
	return this->_isPuzzleCandleResolve;
}

void AMemoriesGameMode::SetIsPuzzleHorlogeResolve(bool isResolve)
{
	this->_isPuzzleHorlogeResolve = isResolve;
}

bool AMemoriesGameMode::GetIsPuzzleHorlogeResolve() const
{
	return this->_isPuzzleHorlogeResolve;
}

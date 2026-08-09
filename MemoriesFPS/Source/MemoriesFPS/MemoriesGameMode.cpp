// Created By Gabriel Laplante on 2026-07-20

#include "MemoriesGameMode.h"
#include "MemoriesGameState.h"
#include "TimerManager.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"


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
}

void AMemoriesGameMode::AudioProperties()
{
	if (TimeWarningSound)
	{
		TimeWarningAudioComponent = NewObject<UAudioComponent>(this);
		TimeWarningAudioComponent->bAutoActivate = false;
		TimeWarningAudioComponent->SetSound(TimeWarningSound);
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
	if (!GS) return;

	float Intensity = intensify ? GetIntensity(GS->TimeRemaining) : 0.0f;

	OnTimeWarningEvent.Broadcast(intensify, Intensity);
}




void AMemoriesGameMode::EndGame()
{
	UE_LOG(LogTemp, Log, TEXT("Game Over!"));
}

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

// By Gabriel Laplante on 2026-07-20

#include "MemoriesGameMode.h"
#include "MemoriesGameState.h"
#include "TimerManager.h"
//#include "Sound/SoundBase.h"


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
	
	GetWorld()->GetTimerManager().SetTimer(
		GameTimerHandle,
		this,
		&AMemoriesGameMode::TickGameTimer,
		1.0f,
		true
	);
	
	//AudioProperties();
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
	if (GS->TimeRemaining > 0.0f && GS->TimeRemaining <= TimeWarningThreshold && FMath::Fmod(GS->TimeRemaining, 30.0f) == 0.0f)
		PlayTimeWarningSound(true);
	
	// Indice sonore à chaque tranche de X minutes (TimeWarningThreshold)
	else if (GS->TimeRemaining > 0.0f && FMath::Fmod(GS->TimeRemaining, TimeWarningThreshold) == 0.0f)
		PlayTimeWarningSound(false);
}


void AMemoriesGameMode::EndGame()
{
	UE_LOG(LogTemp, Log, TEXT("Game Over!"));
}

void AMemoriesGameMode::PlayTimeWarningSound(bool intensify)
{
	
	// Implémentez la logique pour jouer un son d'indice ici
	
	if (intensify)
	{
		UE_LOG(LogTemp, Log, TEXT("Playing intensified time warning sound."));
		//TimeWarningAudioComponent->SetPitchMultiplier(TimeWarningSoundPitch * 1.5f); // Augmente le pitch pour intensifier le son
		//TimeWarningAudioComponent->Play();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Playing time warning sound."));
		//TimeWarningAudioComponent->SetPitchMultiplier(TimeWarningSoundPitch); // Réinitialise le pitch
		//TimeWarningAudioComponent->Play();
	}
}


/*void AMemoriesGameMode::AudioProperties()
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
}*/
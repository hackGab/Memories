// By Gabriel Laplante on 2026-07-20

#include "MemoriesGameMode.h"
#include "MemoriesGameState.h"
#include "TimerManager.h"

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
}


void AMemoriesGameMode::EndGame()
{
	UE_LOG(LogTemp, Log, TEXT("Game Over!"));
}
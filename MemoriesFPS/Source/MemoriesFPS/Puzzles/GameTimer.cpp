// Created By Gabriel Laplante on 2026-07-20

#include "GameTimer.h"
#include "Components/TextRenderComponent.h"
#include "../MemoriesGameState.h"

AGameTimer::AGameTimer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameTimer::BeginPlay()
{
	Super::BeginPlay();
	
	TimerText = Cast<UTextRenderComponent>(GetDefaultSubobjectByName(TEXT("TimerDisplay")));

	if (TimerText == nullptr)
		UE_LOG(LogTemp, Warning, TEXT("TimerText is not assigned in the GameTimer actor."));
}

void AGameTimer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AMemoriesGameState* GameState = GetWorld() ? GetWorld()->GetGameState<AMemoriesGameState>() : nullptr;
	if (GameState)
	{
		UpdateTimerText(GameState->TimeRemaining);
	}
}

void AGameTimer::UpdateTimerText(float TimeRemaining) const
{
	if (!TimerText) return;
	
	int32 Minutes = FMath::FloorToInt(TimeRemaining / 60.0f);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(TimeRemaining, 60.0f));
	
	TimerText->SetText(FText::FromString(
		FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
}

// By Gabriel Laplante on 2026-07-20

#include "GameTimer.h"
#include "Components/TextRenderComponent.h"


AGameTimer::AGameTimer()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGameTimer::BeginPlay()
{
	Super::BeginPlay();
	
	GameDuration = 300.0f; // 5 minutes
	TimeRemaining = GameDuration;
	
	TimerText = Cast<UTextRenderComponent>(GetDefaultSubobjectByName(TEXT("TimerDisplay")));

	if (TimerText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("TimerText is not assigned in the GameTimer actor."));
		return;
	}
	
	UpdateTimerText();
	StartTimer();
}

void AGameTimer::StartTimer()
{
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&AGameTimer::UpdateTimerText,
		1.0f,
		true
	);
}

void AGameTimer::UpdateTimerText()
{
	TimeRemaining -= 1.0f;

	if (TimeRemaining <= 0.0f)
	{
		TimeRemaining = 0.0f;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}

	if (TimerText)
	{
		int32 Minutes = FMath::FloorToInt(TimeRemaining / 60.0f);
		int32 Seconds = FMath::FloorToInt(FMath::Fmod(TimeRemaining, 60.0f));
		
		FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		
		TimerText->SetText(FText::FromString(TimeString));
	}
}


float AGameTimer::GetTimeRemaining() const
{
	return TimeRemaining;
}


// By Gabriel Laplante on 2026-07-20

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameTimer.generated.h"

class UTextRenderComponent;

UCLASS()
class MEMORIESFPS_API AGameTimer : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameTimer();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, Category = "Timer")
	UTextRenderComponent* TimerText;
	
	FTimerHandle TimerHandle;
	
	float GameDuration; // Probablement plus tard on va le récupérer du GameManager
	float TimeRemaining;
	float GetTimeRemaining() const;
	
	void UpdateTimerText();
	void StartTimer();
};

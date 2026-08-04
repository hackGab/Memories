// Created By Gabriel Laplante on 2026-07-20

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MemoriesGameState.generated.h"

UCLASS()
class MEMORIESFPS_API AMemoriesGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Game Timer")
	float TimeRemaining = 0.0f;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

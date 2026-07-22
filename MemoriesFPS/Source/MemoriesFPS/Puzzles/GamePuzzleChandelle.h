// Created By Gabriel Laplante on 2026-07-22

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GamePuzzleChandelle.generated.h"

UCLASS()
class MEMORIESFPS_API AGamePuzzleChandelle : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void GetAllCandles() const;

};

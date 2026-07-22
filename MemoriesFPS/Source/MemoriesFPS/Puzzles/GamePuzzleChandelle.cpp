// Created By Gabriel Laplante on 2026-07-22


#include "GamePuzzleChandelle.h"
#include "Kismet/GameplayStatics.h"


void AGamePuzzleChandelle::BeginPlay()
{
	Super::BeginPlay();
	
	GetAllCandles();
}


void AGamePuzzleChandelle::GetAllCandles() const
{
	FName CandleTag = FName(TEXT("Candle"));
	
	TArray<AActor*> FoundCandles;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), CandleTag, FoundCandles);
	
	UE_LOG(LogTemp, Log, TEXT("Found %d candles in the scene."), FoundCandles.Num());
	
	for (AActor* Candle : FoundCandles)
	{
		if (Candle)
		{
			UE_LOG(LogTemp, Log, TEXT("Candle found: %s"), *Candle->GetName());
		}
		
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Found a null candle actor."));
		}
	}
}
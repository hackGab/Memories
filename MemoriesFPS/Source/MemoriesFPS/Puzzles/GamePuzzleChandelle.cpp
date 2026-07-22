// Created By Gabriel Laplante on 2026-07-22


#include "GamePuzzleChandelle.h"
#include "Kismet/GameplayStatics.h"


void AGamePuzzleChandelle::BeginPlay()
{
	Super::BeginPlay();
	GetAllCandles();
}


void AGamePuzzleChandelle::OnCandleLitChanged(const FString& CandleSymbole, bool bIsLit)
{
	if (IsActorBeingDestroyed() || !IsValid(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnCandleLitChanged called on a destroyed or invalid actor."));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("BP called OnCandleLitChanged: CandleSymbole: %s, bIsLit: %s"),
		*CandleSymbole,
		bIsLit ? TEXT("True") : TEXT("False"));
	
	UpdateCandleStateFromEvent(CandleSymbole, bIsLit);
}


void AGamePuzzleChandelle::UpdateCandleStateFromEvent(const FString& CandleSymbole, bool bIsLit)
{
	UE_LOG(LogTemp, Log, TEXT("UpdateCandleStateFromEvent called for CandleSymbole: %s, bIsLit: %s"),
		*CandleSymbole,
		bIsLit ? TEXT("True") : TEXT("False"));
	
	if (int32* IndexPtr = CandleLookup.Find(CandleSymbole))
	{
		int32 Index = *IndexPtr;
		
		if (!FoundCandles.IsValidIndex(Index))
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateCandleStateFromEvent: FoundCandles is empty or invalid."));
			return;
		}
		
		FoundCandles[Index].bIsLit = bIsLit;
		OnCandleStateChanged.Broadcast(CandleSymbole, bIsLit);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Candle with symbole %s not found in FoundCandles."), *CandleSymbole);
}


void AGamePuzzleChandelle::GetAllCandles()
{
	FName CandleTag = FName(TEXT("Candle"));
	
	TArray<AActor*> TempFoundCandles;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), CandleTag, TempFoundCandles);
	
	UE_LOG(LogTemp, Log, TEXT("Found %d candles in the scene."), TempFoundCandles.Num());
	
	FoundCandles.Empty();
	CandleLookup.Empty();
		
	for (AActor* Candle : TempFoundCandles)
	{
		FCandleState NewCandleState;
		NewCandleState.CandleSymbole = Candle->GetActorLabel();
		
		// Utiliser la variable isLit du BP candle récupérée
		FProperty* IsLitProperty = Candle->GetClass()->FindPropertyByName("isLit");
		
		if (FBoolProperty* BoolProp = CastField<FBoolProperty>(IsLitProperty))
		{
			NewCandleState.bIsLit = BoolProp->GetPropertyValue_InContainer(Candle);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Candle %s does not have a public variable 'isLit'."), *NewCandleState.CandleSymbole);
			NewCandleState.bIsLit = false;
		}		

		int32 Index = FoundCandles.Add(NewCandleState);
		CandleLookup.Add(NewCandleState.CandleSymbole, Index);
		
		UE_LOG(LogTemp, Log, TEXT("Candle: %s, IsLit: %s"), 
			*NewCandleState.CandleSymbole,
			NewCandleState.bIsLit ? TEXT("True") : TEXT("False"));
	}
}


void AGamePuzzleChandelle::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogTemp, Log, TEXT("AGamePuzzleChandelle is being destroyed. Cleaning up."));

	FoundCandles.Empty();
	CandleLookup.Empty();
	Super::EndPlay(EndPlayReason);
}
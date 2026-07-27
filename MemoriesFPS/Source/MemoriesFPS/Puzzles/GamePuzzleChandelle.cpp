// Created By Gabriel Laplante on 2026-07-22


#include "GamePuzzleChandelle.h"
#include "Kismet/GameplayStatics.h"
#include "../MemoriesGameMode.h"


void AGamePuzzleChandelle::BeginPlay()
{
	Super::BeginPlay();
	GetAllCandles();
	
	CandleSolutionLookup.Empty();
	
	for (const FCandleSolutionConfig& Solution : CandleSolutions)
	{
		CandleSolutionLookup.Add(Solution.CandleSymbole, Solution.bShouldBeLit);
		
		if (Solution.bShouldBeLit)
			nbCandlesToLit++;
	}
}


void AGamePuzzleChandelle::OnCandleLitChanged(const FString& CandleSymbole, bool bIsLit)
{
	if (isPuzzleCandleResolve)
	{
		UE_LOG(LogTemp, Warning, TEXT("Yo le puzzle est déjà résolut, lâche le !"));
		return;
	}
	
	if (IsActorBeingDestroyed() || !IsValid(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnCandleLitChanged called on a destroyed or invalid actor."));
		return;
	}
	
	/*UE_LOG(LogTemp, Log, TEXT("BP called OnCandleLitChanged: CandleSymbole: %s, bIsLit: %s"),
		*CandleSymbole,
		bIsLit ? TEXT("True") : TEXT("False"));
	*/
	
	UpdateCandleStateFromEvent(CandleSymbole, bIsLit);
}

FString AGamePuzzleChandelle::NormalizeSymbol(const FString& RawSymbol)
{
	FString Normalized = RawSymbol;
	RawSymbol.Split(TEXT("_"), nullptr, &Normalized, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	return Normalized;
}


void AGamePuzzleChandelle::UpdateCandleStateFromEvent(const FString& CandleSymbole, bool bIsLit)
{
	/*UE_LOG(LogTemp, Log, TEXT("UpdateCandleStateFromEvent called for CandleSymbole: %s, bIsLit: %s"),
		*CandleSymbole,
		bIsLit ? TEXT("True") : TEXT("False"));
	*/
	
	FString NormalizedSymbol = NormalizeSymbol(CandleSymbole);
	
	if (int32* IndexPtr = CandleLookup.Find(CandleSymbole))
	{
		int32 Index = *IndexPtr;
		
		if (!FoundCandles.IsValidIndex(Index))
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateCandleStateFromEvent: FoundCandles is empty or invalid."));
			return;
		}
		
		// Évite de rappuyer sur un qui est déjà allumé
		if (FoundCandles[Index].bIsLit == bIsLit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tu ne peux pas allumé une qui est déjà allumé"));
			return;
		}
		
		FoundCandles[Index].bIsLit = bIsLit;
		OnCandleStateChanged.Broadcast(CandleSymbole, bIsLit);
		
		VerifyPuzzleSolution(NormalizedSymbol, bIsLit);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Candle with symbole %s not found in FoundCandles."), *CandleSymbole);
}


void AGamePuzzleChandelle::VerifyPuzzleSolution(const FString& CandleSymbole, bool bIsLit)
{
	bool* ShouldBeLitPtr = CandleSolutionLookup.Find(CandleSymbole);
	if (!ShouldBeLitPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Candle %s not found in solution."), *CandleSymbole);
		return;
	}

	bool bShouldBeLit = *ShouldBeLitPtr;
	
	// WIN 
	if (bIsLit == bShouldBeLit)
	{
		UE_LOG(LogTemp, Log, TEXT("Candle %s is in the correct state: %s"), 
			*CandleSymbole,
			bIsLit ? TEXT("Lit") : TEXT("Unlit"));
		
		
		nbCandlesAreLit++;
		
		if (nbCandlesAreLit == nbCandlesToLit)
		{
			isPuzzleCandleResolve = true;
			
			// Appeler la méthode SetIsPuzzleCandleResolve de MemoriesGameMode
			AMemoriesGameMode* GM = Cast<AMemoriesGameMode>(UGameplayStatics::GetGameMode(this));
			if (!GM)
			{
				UE_LOG(LogTemp, Error, TEXT("AMemoriesGameMode introuvable ou mauvais type !"));
				return;
			}
			
			GM->SetIsPuzzleCandleResolve(isPuzzleCandleResolve);
			UE_LOG(LogTemp, Display, TEXT("Tous les bonnes chandelles sont allumés ! Le AMemoriesGameMode recoit : %hhd"), GM->GetIsPuzzleCandleResolve());
			
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
				FString::Printf(TEXT("Puzzle chandelle : %s"),
					GM->GetIsPuzzleCandleResolve() ? TEXT("résolu") : TEXT("non résolu")));

		}
	}
	
	// RESET
	else
	{
		/*UE_LOG(LogTemp, Log, TEXT("Candle %s is NOT in the correct state. Expected: %s, Actual: %s"), 
			*CandleSymbole,
			bShouldBeLit ? TEXT("Lit") : TEXT("Unlit"),
			bIsLit ? TEXT("Lit") : TEXT("Unlit"));*/
		
		nbCandlesAreLit = 0;
		

		// Reset all the candles to their initial state (off)
		// pour chaque chandelle allumée, on l'éteint, et on met à jour FoundCandles et CandleLookup
		for (FCandleState& CandleState : FoundCandles)
		{
			if (CandleState.bIsLit)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Reset: broadcasting OFF for %s"), *CandleState.CandleSymbole);
				
				
				CandleState.bIsLit = false;
				OnCandleStateChanged.Broadcast(CandleState.CandleSymbole, false);
			}
		}
	}
	
}


void AGamePuzzleChandelle::GetAllCandles()
{
	FName CandleTag = FName(TEXT("Candle"));
	
	TArray<AActor*> TempFoundCandles;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), CandleTag, TempFoundCandles);
	
	//UE_LOG(LogTemp, Log, TEXT("Found %d candles in the scene."), TempFoundCandles.Num());
	
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
		
		/*UE_LOG(LogTemp, Log, TEXT("Candle: %s, IsLit: %s"), 
			*NewCandleState.CandleSymbole,
			NewCandleState.bIsLit ? TEXT("True") : TEXT("False"));*/
	}
}


void AGamePuzzleChandelle::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//UE_LOG(LogTemp, Log, TEXT("AGamePuzzleChandelle is being destroyed. Cleaning up."));

	FoundCandles.Empty();
	CandleLookup.Empty();
	Super::EndPlay(EndPlayReason);
}
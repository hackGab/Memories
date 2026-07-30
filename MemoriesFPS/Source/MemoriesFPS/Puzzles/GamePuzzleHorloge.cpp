// Created By Gabriel Laplante on 2026-07-30


#include "GamePuzzleHorloge.h"
#include "../MemoriesGameMode.h"
#include "Kismet/GameplayStatics.h"


void AGamePuzzleHorloge::BeginPlay()
{
	Super::BeginPlay();
	GetAllHorloges();
	
	HorlogeSolutionLookup.Empty();
	nbHorlogesToSolve = 0;
	
	for (const FHorlogeSolutionConfig& Solution : HorlogeSolutions)
	{
		HorlogeSolutionLookup.Add(Solution.HorlogeSymbole, Solution);
		nbHorlogesToSolve++;
	}
}


void AGamePuzzleHorloge::OnHoursTimeChanged(const FString& HorlogeSymbole, double timeHours)
{
	if (isPuzzleHorlogeResolve)
	{
		UE_LOG(LogTemp, Warning, TEXT("Yo le puzzle est déjà résolut, lâche le !"));
		return;
	}
	
	if (IsActorBeingDestroyed() || !IsValid(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnHoursStateChanged called on a destroyed or invalid actor."));
		return;
	}
	
	UpdateHoursStateFromEvent(HorlogeSymbole, timeHours);
}

void AGamePuzzleHorloge::OnMinutesTimeChanged(const FString& HorlogeSymbole, double timeMinutes)
{
	if (isPuzzleHorlogeResolve)
	{
		UE_LOG(LogTemp, Warning, TEXT("Yo le puzzle est déjà résolut, lâche le !"));
		return;
	}
	
	if (IsActorBeingDestroyed() || !IsValid(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnHoursStateChanged called on a destroyed or invalid actor."));
		return;
	}
	
	UpdateMinutesStateFromEvent(HorlogeSymbole, timeMinutes);
}

FString AGamePuzzleHorloge::NormalizeSymbol(const FString& RawSymbol)
{
	FString Normalized = RawSymbol;
	RawSymbol.Split(TEXT("_"), nullptr, &Normalized, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	return Normalized;
}


void AGamePuzzleHorloge::UpdateHoursStateFromEvent(const FString& HorlogeSymbole, double timeHours)
{
	FString NormalizedSymbol = NormalizeSymbol(HorlogeSymbole);
	
	if (int32* IndexPtr = HorlogeLookup.Find(HorlogeSymbole))
	{
		int32 Index = *IndexPtr;
		
		if (!FoundHorloges.IsValidIndex(Index))
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateHoursStateFromEvent: FoundHorloges is empty or invalid."));
			return;
		}
		
		// Évite de rotate une aiguille qui est déjà dans la bonne direction
		/*if (FoundHorloges[Index].timeHours == timeHours)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tu ne peux pas rotate une aiguille qui est déjà dans la bonne direction"));
			return;
		}*/
		
		FoundHorloges[Index].timeHours = timeHours;
		OnHoursStateChanged.Broadcast(HorlogeSymbole, timeHours);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Horloge with symbole %s not found in FoundHorloges."), *HorlogeSymbole);
}

void AGamePuzzleHorloge::UpdateMinutesStateFromEvent(const FString& HorlogeSymbole, double timeMinutes)
{
	FString NormalizedSymbol = NormalizeSymbol(HorlogeSymbole);
	
	if (int32* IndexPtr = HorlogeLookup.Find(HorlogeSymbole))
	{
		int32 Index = *IndexPtr;
		
		if (!FoundHorloges.IsValidIndex(Index))
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateMinutesStateFromEvent: FoundHorloges is empty or invalid."));
			return;
		}
		
		// Évite de rotate une aiguille qui est déjà dans la bonne direction
		/*if (FoundHorloges[Index].timeMinutes == timeMinutes)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tu ne peux pas rotate une aiguille qui est déjà dans la bonne direction"));
			return;
		}*/
		
		FoundHorloges[Index].timeMinutes = timeMinutes;
		OnHoursStateChanged.Broadcast(HorlogeSymbole, timeMinutes);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Horloge with symbole %s not found in FoundHorloges."), *HorlogeSymbole);
}


 void AGamePuzzleHorloge::VerifyPuzzleSolution(FHorlogeState HorlogeState)
{
	FHorlogeSolutionConfig* horlogeSolutionConfigPtr = HorlogeSolutionLookup.Find(HorlogeState.HorlogeSymbole);
	if (!horlogeSolutionConfigPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Horloge %s not found in solution."), *HorlogeState.HorlogeSymbole);
		return;
	}
	
	FHorlogeSolutionConfig horlogeSolutionConfig = *horlogeSolutionConfigPtr;
	
	// WIN
	if (horlogeSolutionConfig.ShouldBeTimeHours == HorlogeState.timeHours &&
		horlogeSolutionConfig.ShouldBeTimeMinutes == HorlogeState.timeMinutes)
	{
		UE_LOG(LogTemp, Log, TEXT("Horloge %s is in the correct state: [%f:%f]"), 
			*HorlogeState.HorlogeSymbole,
			HorlogeState.timeHours,
			HorlogeState.timeMinutes);
		
		nbHorlogesAreSolve++;
		
		// Envoyer l'info au GM
		if (nbHorlogesToSolve == nbHorlogesAreSolve)
		{
			isPuzzleHorlogeResolve = true;
			
			// Appeler la méthode SetIsPuzzleCandleResolve de MemoriesGameMode
			AMemoriesGameMode* GM = Cast<AMemoriesGameMode>(UGameplayStatics::GetGameMode(this));
			if (!GM)
			{
				UE_LOG(LogTemp, Error, TEXT("AMemoriesGameMode introuvable ou mauvais type !"));
				return;
			}
			
			GM->SetIsPuzzleCandleResolve(isPuzzleHorlogeResolve);
			UE_LOG(LogTemp, Display, TEXT("Tous les horloges sont set à la bonne heure ! Le AMemoriesGameMode recoit : %hhd"), GM->GetIsPuzzleHorlogeResolve());
			
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
				FString::Printf(TEXT("Puzzle horloge : %s"),
					GM->GetIsPuzzleHorlogeResolve() ? TEXT("résolu") : TEXT("non résolu")));

		}
	}
}


void AGamePuzzleHorloge::GetAllHorloges()
{
	FName HorlogeTag = FName(TEXT("Horloge"));
	
	TArray<AActor*> TempFoundHorloges;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), HorlogeTag, TempFoundHorloges);
	
	UE_LOG(LogTemp, Log, TEXT("Found %d horloges in the scene."), TempFoundHorloges.Num());
	
	FoundHorloges.Empty();
	HorlogeLookup.Empty();
	
	for (AActor* Horloge : TempFoundHorloges)
	{
		FHorlogeState NewHorlogeState;
		NewHorlogeState.HorlogeSymbole = Horloge->GetActorLabel();
		
		// Récupérer les variables times hours et minutes
		FProperty* timeHoursProperty = Horloge->GetClass()->FindPropertyByName("timeHours");
		FProperty* timeMinutesProperty = Horloge->GetClass()->FindPropertyByName("timeMinutes");
		
		if (FDoubleProperty* DoubleHoursProp = CastField<FDoubleProperty>(timeHoursProperty))
			NewHorlogeState.timeHours = DoubleHoursProp->GetPropertyValue_InContainer(Horloge);

		else
			UE_LOG(LogTemp, Warning, TEXT("Horloge %s does not have a public variable 'timeHours'."), *NewHorlogeState.HorlogeSymbole);
		
		if (FDoubleProperty* DoubleMinutesProp = CastField<FDoubleProperty>(timeMinutesProperty))
			NewHorlogeState.timeMinutes = DoubleMinutesProp->GetPropertyValue_InContainer(Horloge);

		else
			UE_LOG(LogTemp, Warning, TEXT("Horloge %s does not have a public variable 'timeMinutes'."), *NewHorlogeState.HorlogeSymbole);
		
		int32 Index = FoundHorloges.Add(NewHorlogeState);
		HorlogeLookup.Add(NewHorlogeState.HorlogeSymbole, Index);
	}
}


void AGamePuzzleHorloge::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//UE_LOG(LogTemp, Log, TEXT("AGamePuzzleHorloge is being destroyed. Cleaning up."));

	FoundHorloges.Empty();
	HorlogeLookup.Empty();
	Super::EndPlay(EndPlayReason);
}
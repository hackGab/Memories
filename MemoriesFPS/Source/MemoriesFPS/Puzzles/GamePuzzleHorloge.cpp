// Created By Gabriel Laplante on 2026-07-30


#include "GamePuzzleHorloge.h"
#include "../MemoriesGameMode.h"
#include "Kismet/GameplayStatics.h"


void AGamePuzzleHorloge::BeginPlay()
{
	Super::BeginPlay();
	
	HorlogeSolutionLookup.Empty();
	FoundHorloges.Empty();
	HorlogeLookup.Empty();
	HorlogeActorLookup.Empty();
	
	nbHorlogesToSolve = 0;
	nbHorlogesAreSolve = 0;
	isPuzzleHorlogeResolve = false;

	for (const FHorlogeSolutionConfig& Solution : HorlogeSolutions)
	{
		HorlogeSolutionLookup.Add(
		   Solution.HorlogeSymbole,
		   Solution
	   );

		nbHorlogesToSolve++;

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("SOLUTION REGISTERED: '%s' -> %f:%f"),
			*Solution.HorlogeSymbole,
			Solution.ShouldBeTimeHours,
			Solution.ShouldBeTimeMinutes
		);
	}
	GetAllHorloges();
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

	if (int32* IndexPtr = HorlogeLookup.Find(NormalizedSymbol))
	{
		int32 Index = *IndexPtr;

		if (!FoundHorloges.IsValidIndex(Index))
		{
			UE_LOG(LogTemp, Warning, TEXT("Horloge with symbole %s not found in FoundHorloges."), *HorlogeSymbole)
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
		VerifyPuzzleSolution(FoundHorloges[Index]); // <-- ajout
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
		OnMinutesStateChanged.Broadcast(HorlogeSymbole, timeMinutes);
		
		VerifyPuzzleSolution(FoundHorloges[Index]); // <-- ajout
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Horloge with symbole %s not found in FoundHorloges."), *HorlogeSymbole);
}


void AGamePuzzleHorloge::VerifyPuzzleSolution(FHorlogeState& HorlogeState)
{
	FHorlogeSolutionConfig* SolutionPtr = HorlogeSolutionLookup.Find(HorlogeState.HorlogeSymbole);
	if (!SolutionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Horloge %s not found in solution."), *HorlogeState.HorlogeSymbole);
		return;
	}

	AHorlogeActor* HorlogeActor = nullptr;
	if (AHorlogeActor** ActorPtr = HorlogeActorLookup.Find(HorlogeState.HorlogeSymbole))
	{
		HorlogeActor = *ActorPtr;
	}

	const bool bIsCorrect =
		SolutionPtr->ShouldBeTimeHours == HorlogeState.timeHours &&
		SolutionPtr->ShouldBeTimeMinutes == HorlogeState.timeMinutes;

	if (bIsCorrect && !HorlogeState.bIsSolved)
	{
		// Cette horloge vient d'être réglée correctement
		HorlogeState.bIsSolved = true;
		nbHorlogesAreSolve++;

		UE_LOG(LogTemp, Log, TEXT("Horloge %s is in the correct state: [%f:%f]"),
			*HorlogeState.HorlogeSymbole,
			HorlogeState.timeHours,
			HorlogeState.timeMinutes);

		if (HorlogeActor)
		{
			HorlogeActor->PlaySuccessCue();
		}

		if (nbHorlogesToSolve == nbHorlogesAreSolve)
		{
			isPuzzleHorlogeResolve = true;

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

			if (YaySound)
			{
				UGameplayStatics::PlaySound2D(this, YaySound);
			}

			// La porte est ouverte en écoutant ce délégué depuis une Blueprint (ou une autre classe C++)
			OnPuzzleSolved.Broadcast();
		}
	}
	else if (!bIsCorrect && HorlogeState.bIsSolved)
	{
		// Le joueur a débougé une horloge qui était correcte : on décompte
		HorlogeState.bIsSolved = false;
		nbHorlogesAreSolve = FMath::Max(0, nbHorlogesAreSolve - 1);
	}
	else if (!bIsCorrect && HorlogeActor)
	{
		HorlogeActor->PlayFailCue();
	}
}


void AGamePuzzleHorloge::GetAllHorloges()
{
	FName HorlogeTag = FName(TEXT("Horloge"));

	TArray<AActor*> TempFoundHorloges;

	UGameplayStatics::GetAllActorsWithTag(
		GetWorld(),
		HorlogeTag,
		TempFoundHorloges
	);

	FoundHorloges.Empty();
	HorlogeLookup.Empty();
	HorlogeActorLookup.Empty();

	for (AActor* Horloge : TempFoundHorloges)
	{
		AHorlogeActor* HorlogeActor = Cast<AHorlogeActor>(Horloge);

		if (!HorlogeActor)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Actor %s tagged Horloge but is not an AHorlogeActor."),
				*Horloge->GetName()
			);

			continue;
		}
		
		const FString ClockSymbol = HorlogeActor->Symbole;

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("REGISTERING CLOCK: Actor=%s | Symbol=%s"),
			*HorlogeActor->GetActorLabel(),
			*ClockSymbol
		);

		FHorlogeState NewHorlogeState;

		NewHorlogeState.HorlogeSymbole = ClockSymbol;
		NewHorlogeState.timeHours = static_cast<double>(HorlogeActor->Hours);
		NewHorlogeState.timeMinutes = static_cast<double>(HorlogeActor->Minutes);
		NewHorlogeState.bIsSolved = false;

		const int32 Index = FoundHorloges.Add(NewHorlogeState);

		HorlogeLookup.Add(ClockSymbol, Index);
		HorlogeActorLookup.Add(ClockSymbol, HorlogeActor);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("CLOCK REGISTERED: Symbol=%s | Index=%d | Time=%f:%f"),
			*ClockSymbol,
			Index,
			NewHorlogeState.timeHours,
			NewHorlogeState.timeMinutes
		);
	}
}



void AGamePuzzleHorloge::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//UE_LOG(LogTemp, Log, TEXT("AGamePuzzleHorloge is being destroyed. Cleaning up."));

	FoundHorloges.Empty();
	HorlogeLookup.Empty();
	HorlogeActorLookup.Empty();
	Super::EndPlay(EndPlayReason);
}
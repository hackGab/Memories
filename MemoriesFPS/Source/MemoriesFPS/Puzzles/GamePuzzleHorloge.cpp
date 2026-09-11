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

	int32 UnderscoreIndex = INDEX_NONE;

	if (RawSymbol.FindLastChar(TEXT('_'), UnderscoreIndex))
	{
		// Pour l'instant on conserve le symbole complet.
		// Cette fonction pourra être utilisée si nécessaire.
	}

	Normalized.TrimStartAndEndInline();

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

	if (int32* IndexPtr = HorlogeLookup.Find(NormalizedSymbol))
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
    // Find the solution for this clock

    FHorlogeSolutionConfig* SolutionPtr =
        HorlogeSolutionLookup.Find(HorlogeState.HorlogeSymbole);

    if (!SolutionPtr)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Horloge %s not found in solution."),
            *HorlogeState.HorlogeSymbole
        );

        return;
    }
	
    // Find the actual clock actor

    AHorlogeActor* HorlogeActor = nullptr;

    if (AHorlogeActor** ActorPtr =
        HorlogeActorLookup.Find(HorlogeState.HorlogeSymbole))
    {
        HorlogeActor = *ActorPtr;
    }
    // Check if the clock has the correct time

    const bool bIsCorrect =
        SolutionPtr->ShouldBeTimeHours == HorlogeState.timeHours &&
        SolutionPtr->ShouldBeTimeMinutes == HorlogeState.timeMinutes;
    // CLOCK SOLVED

    if (bIsCorrect && !HorlogeState.bIsSolved)
    {
        // Mark this clock as solved
        HorlogeState.bIsSolved = true;

        nbHorlogesAreSolve++;

        UE_LOG(
            LogTemp,
            Log,
            TEXT(
                "Horloge %s is in the correct state: [%02d:%02d]"
            ),
            *HorlogeState.HorlogeSymbole,
            static_cast<int32>(HorlogeState.timeHours),
            static_cast<int32>(HorlogeState.timeMinutes)
        );
        // Play success feedback and lock the clock
        if (HorlogeActor)
        {
            HorlogeActor->PlaySuccessCue();

            // Prevent the player from modifying this clock anymore
            HorlogeActor->LockClock();
        }
        // Check if ALL clocks are solved

        if (nbHorlogesAreSolve >= nbHorlogesToSolve)
        {
            isPuzzleHorlogeResolve = true;

            UE_LOG(
                LogTemp,
                Display,
                TEXT(
                    "Tous les horloges sont réglées correctement !"
                )
            );
            // Notify GameMode
            AMemoriesGameMode* GM =
                Cast<AMemoriesGameMode>(
                    UGameplayStatics::GetGameMode(this)
                );

            if (!GM)
            {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT(
                        "AMemoriesGameMode introuvable ou mauvais type !"
                    )
                );

                return;
            }

            GM->SetIsPuzzleCandleResolve(
                isPuzzleHorlogeResolve
            );

            UE_LOG(
                LogTemp,
                Display,
                TEXT(
                    "AMemoriesGameMode reçoit PuzzleHorlogeResolve = %hhd"
                ),
                GM->GetIsPuzzleHorlogeResolve()
            );

            // ====================================================
            // On-screen debug
            // ====================================================

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(
                    -1,
                    3.f,
                    FColor::Green,
                    FString::Printf(
                        TEXT("Puzzle horloge : %s"),
                        GM->GetIsPuzzleHorlogeResolve()
                            ? TEXT("RÉSOLU")
                            : TEXT("NON RÉSOLU")
                    )
                );
            }

            // ====================================================
            // Play puzzle completion sound
            // ====================================================

            if (YaySound)
            {
                UGameplayStatics::PlaySound2D(
                    this,
                    YaySound
                );
            }

            // ====================================================
            // Notify Blueprint / other C++ systems
            // ====================================================

            OnPuzzleSolved.Broadcast();
        }
    }

    // ========================================================
    // CLOCK WAS SOLVED BUT PLAYER MOVED IT AGAIN
    // ========================================================

    else if (!bIsCorrect && HorlogeState.bIsSolved)
    {
        HorlogeState.bIsSolved = false;

        nbHorlogesAreSolve =
            FMath::Max(
                0,
                nbHorlogesAreSolve - 1
            );

        UE_LOG(
            LogTemp,
            Warning,
            TEXT(
                "Horloge %s n'est plus dans la bonne position."
            ),
            *HorlogeState.HorlogeSymbole
        );
    }

    // ========================================================
    // WRONG TIME
    // ========================================================

    else if (!bIsCorrect && HorlogeActor)
    {
        HorlogeActor->PlayFailCue();
    }
}

void AGamePuzzleHorloge::GetAllHorloges()
{
	const FName HorlogeTag = FName(TEXT("Horloge"));

	TArray<AActor*> TempFoundHorloges;

	UGameplayStatics::GetAllActorsWithTag(
		GetWorld(),
		HorlogeTag,
		TempFoundHorloges
	);

	FoundHorloges.Empty();
	HorlogeLookup.Empty();
	HorlogeActorLookup.Empty();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("========== GET ALL HORLOGES ==========")
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Nombre d'acteurs trouvés avec le tag Horloge : %d"),
		TempFoundHorloges.Num()
	);

	for (AActor* Horloge : TempFoundHorloges)
	{
		AHorlogeActor* HorlogeActor =
			Cast<AHorlogeActor>(Horloge);

		if (!HorlogeActor)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Actor %s possède le tag Horloge mais n'est pas AHorlogeActor"),
				*Horloge->GetName()
			);

			continue;
		}

		// ========================================================
		// Récupération du symbole
		// ========================================================

		FString Symbol = NormalizeSymbol(HorlogeActor->Symbole);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Horloge trouvée : Actor=%s | Label=%s | Symbole=%s"),
			*HorlogeActor->GetName(),
			*HorlogeActor->GetActorLabel(),
			*Symbol
		);

		if (Symbol.IsEmpty())
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("L'horloge %s n'a aucun symbole !"),
				*HorlogeActor->GetName()
			);

			continue;
		}

		// ========================================================
		// Création du state
		// ========================================================

		FHorlogeState NewHorlogeState;

		NewHorlogeState.HorlogeSymbole = Symbol;
		NewHorlogeState.Symbole = Symbol;

		NewHorlogeState.timeHours =
			static_cast<double>(HorlogeActor->Hours);

		NewHorlogeState.timeMinutes =
			static_cast<double>(HorlogeActor->Minutes);

		NewHorlogeState.bIsSolved = false;

		// ========================================================
		// Ajout
		// ========================================================

		int32 Index = FoundHorloges.Add(NewHorlogeState);

		HorlogeLookup.Add(Symbol, Index);

		HorlogeActorLookup.Add(
			Symbol,
			HorlogeActor
		);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Ajoutée au lookup : [%s] -> Index %d"),
			*Symbol,
			Index
		);
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Total FoundHorloges = %d"),
		FoundHorloges.Num()
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("======================================")
	);
}

void AGamePuzzleHorloge::RandomizeHorloge(
	AHorlogeActor* HorlogeActor,
	const FHorlogeSolutionConfig& Solution
)
{
	if (!HorlogeActor)
	{
		return;
	}

	// How far the randomized time must be from the solution
	const int32 MinimumHourDistance = 2;
	const int32 MinimumMinuteDistance = 10;

	int32 RandomHours = 0;
	int32 RandomMinutes = 0;

	int32 Attempts = 0;

	do
	{
		RandomHours = FMath::RandRange(0, 11);
		RandomMinutes = FMath::RandRange(0, 59);

		Attempts++;

		// Prevent an infinite loop
		if (Attempts > 100)
		{
			break;
		}

	} while (
		FMath::Abs(RandomHours - static_cast<int32>(Solution.ShouldBeTimeHours)) 
			< MinimumHourDistance
		||
		FMath::Abs(RandomMinutes - static_cast<int32>(Solution.ShouldBeTimeMinutes))
			< MinimumMinuteDistance
	);

	HorlogeActor->Hours = RandomHours;
	HorlogeActor->Minutes = RandomMinutes;

	// Reset the visual position of the hands
	HorlogeActor->SyncToPuzzleValues();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Horloge '%s' randomisée à %02d:%02d | Solution: %02d:%02d"),
		*HorlogeActor->Symbole,
		RandomHours,
		RandomMinutes,
		static_cast<int32>(Solution.ShouldBeTimeHours),
		static_cast<int32>(Solution.ShouldBeTimeMinutes)
	);
}

void AGamePuzzleHorloge::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//UE_LOG(LogTemp, Log, TEXT("AGamePuzzleHorloge is being destroyed. Cleaning up."));

	FoundHorloges.Empty();
	HorlogeLookup.Empty();
	HorlogeActorLookup.Empty();
	Super::EndPlay(EndPlayReason);
}
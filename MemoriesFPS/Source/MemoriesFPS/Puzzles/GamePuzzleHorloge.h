// Created By Gabriel Laplante on 2026-07-30

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AHorlogeActor.h" 
#include "GamePuzzleHorloge.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHorlogeStateChanged, const FString&, HorlogeSymbole, double, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleSolved);

// --- Struct: Configuration d'une horloge (éditeur) ---
USTRUCT(BlueprintType)
struct FHorlogeSolutionConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HorlogeSymbole;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double ShouldBeTimeHours = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double ShouldBeTimeMinutes = 0.0;
};

// --- Struct: État runtime d'une horloge ---
USTRUCT(BlueprintType)
struct FHorlogeState
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|HorlogeConfig")
	FString HorlogeSymbole;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|HorlogeConfig")
	double timeHours = 0.0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|HorlogeConfig")
	double timeMinutes = 0.0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horloge")
	FString Symbole;
	
	// Empêche de recompter une horloge déjà validée
	UPROPERTY(BlueprintReadOnly)
	bool bIsSolved = false;
};

UCLASS(Blueprintable, BlueprintType)
class MEMORIESFPS_API AGamePuzzleHorloge : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// --- Config (à remplir dans l'éditeur, une entrée par horloge secondaire) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Horloge|Config")
	TArray<FHorlogeSolutionConfig> HorlogeSolutions;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Horloge|Reward")
	TArray<AActor*> ObjectsToUnlock;

	// --- Runtime ---
	UPROPERTY(BlueprintReadOnly)
	TArray<FHorlogeState> FoundHorloges;

	UPROPERTY()
	TMap<FString, int32> HorlogeLookup;

	UPROPERTY()
	TMap<FString, FHorlogeSolutionConfig> HorlogeSolutionLookup;

	UPROPERTY()
	TMap<FString, AHorlogeActor*> HorlogeActorLookup;

	UPROPERTY(BlueprintReadOnly)
	int32 nbHorlogesToSolve = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 nbHorlogesAreSolve = 0;

	UPROPERTY(BlueprintReadOnly)
	bool isPuzzleHorlogeResolve = false;

	// --- Sound reward ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Horloge|Reward")
	USoundBase* YaySound;

	// --- Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "Horloge")
	FOnHorlogeStateChanged OnHoursStateChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Horloge")
	FOnHorlogeStateChanged OnMinutesStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Horloge")
	FOnPuzzleSolved OnPuzzleSolved;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	AActor* LinkedUnlockActor;
	

	// --- Functions ---
	UFUNCTION(BlueprintCallable, Category="Horloge")
	void OnHoursTimeChanged(const FString& HorlogeSymbole, double timeHours);
	
	UFUNCTION(BlueprintCallable, Category="Horloge")
	void OnMinutesTimeChanged(const FString& HorlogeSymbole, double timeMinutes);

	FString NormalizeSymbol(const FString& RawSymbol);

	void UpdateHoursStateFromEvent(const FString& HorlogeSymbole, double timeHours);
	void UpdateMinutesStateFromEvent(const FString& HorlogeSymbole, double timeMinutes);

	void VerifyPuzzleSolution(FHorlogeState& HorlogeState);

	void GetAllHorloges();
	
private:
	void RandomizeHorloge(
	 AHorlogeActor* HorlogeActor,
	 const FHorlogeSolutionConfig& Solution
 );
};
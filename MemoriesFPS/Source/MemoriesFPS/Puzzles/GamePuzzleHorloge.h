// Created By Gabriel Laplante on 2026-07-30

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GamePuzzleHorloge.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHoursStateChanged, const FString&, HorlogeSymbole, double, timeHours);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMinutesStateChanged, const FString&, HorlogeSymbole, double, timeMinutes);


USTRUCT(BlueprintType)
struct FHorlogeSolutionConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|HorlogeConfig")
	FString HorlogeSymbole;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|HorlogeConfig")
	double ShouldBeTimeHours;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|HorlogeConfig")
	double ShouldBeTimeMinutes;
	
};

USTRUCT(BlueprintType)
struct FHorlogeState
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|HorlogeConfig")
	FString HorlogeSymbole;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|HorlogeConfig")
	double timeHours;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|HorlogeConfig")
	double timeMinutes;
};


UCLASS()
class MEMORIESFPS_API AGamePuzzleHorloge : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(BlueprintAssignable)
	FOnHoursStateChanged OnHoursStateChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnMinutesStateChanged OnMinutesStateChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void OnHoursTimeChanged(const FString& HorlogeSymbole, double timeHours);
	
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void OnMinutesTimeChanged(const FString& HorlogeSymbole, double timeMinutes);
	
	

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle", meta = (TitleProperty = "HorlogeSymbole"))
	TArray<FHorlogeSolutionConfig> HorlogeSolutions;
	
	UPROPERTY()
	TMap<FString, int32> HorlogeLookup;
	
	UPROPERTY()
	TMap<FString, FHorlogeSolutionConfig> HorlogeSolutionLookup;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle", meta = (TitleProperty = "HorlogeSymbole"))
	TArray<FHorlogeState> FoundHorloges;
	
	void GetAllHorloges();
	void UpdateHoursStateFromEvent(const FString& HorlogeSymbole, double timeHours);
	void UpdateMinutesStateFromEvent(const FString& HorlogeSymbole, double timeMinutes);
	
	void VerifyPuzzleSolution(FHorlogeState);
	
	static FString NormalizeSymbol(const FString& RawSymbol);
	
	int32 nbHorlogesToSolve;
	int32 nbHorlogesAreSolve; 
	bool isPuzzleHorlogeResolve = false;
};

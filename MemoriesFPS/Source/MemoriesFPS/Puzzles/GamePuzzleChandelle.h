// Created By Gabriel Laplante on 2026-07-22

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GamePuzzleChandelle.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCandleStateChanged, const FString&, CandleSymbole, bool, bIsLit);


// Structure to hold candle configuration data
USTRUCT(BlueprintType)
struct FCandleSolutionConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|CandlesConfig")
	FString CandleSymbole;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|CandlesConfig")
	bool bShouldBeLit;
};

// Structure to hold candle state data (those are the candles that are found in the scene)
USTRUCT(BlueprintType)
struct FCandleState
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|CandlesFoundState")
	FString CandleSymbole;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|CandlesFoundState")
	bool bIsLit;
};


UCLASS()
class MEMORIESFPS_API AGamePuzzleChandelle : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnCandleStateChanged OnCandleStateChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void OnCandleLitChanged(const FString& CandleSymbole, bool bIsLit);
	

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle", meta = (TitleProperty = "CandleSymbole"))
	TArray<FCandleSolutionConfig> CandleSolutions;
	
	UPROPERTY()
	TMap<FString, int32> CandleLookup;
	
	UPROPERTY()
	TMap<FString, bool> CandleSolutionLookup;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Puzzle", meta = (TitleProperty = "CandleSymbole"))
	TArray<FCandleState> FoundCandles;
	
	void GetAllCandles();
	void UpdateCandleStateFromEvent(const FString& CandleSymbole, bool bIsLit);
	
	void VerifyPuzzleSolution(const FString& CandleSymbole, bool bIsLit);
	
	static FString NormalizeSymbol(const FString& RawSymbol);
	
	
	int32 nbCandlesToLit;
	int32 nbCandlesAreLit;
	bool isPuzzleCandleResolve = false;
	
};

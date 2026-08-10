#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APuzzleChandelier.generated.h"

UENUM(BlueprintType)
enum class EPuzzleColor : uint8
{
    Red,
    Blue,
    Green,
    Purple,
    Yellow,
    Pink,
    None
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFlameColorChanged, FName, CardinalPoint, EPuzzleColor, Color);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleSolved);

USTRUCT(BlueprintType)
struct FExteriorLight
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPuzzleColor MissingColor = EPuzzleColor::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPuzzleColor CurrentColor = EPuzzleColor::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RotationIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* LightActor = nullptr;
};

USTRUCT(BlueprintType)
struct FPedestal
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPuzzleColor PlayerColor = EPuzzleColor::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* PedestalActor = nullptr;
};

USTRUCT(BlueprintType)
struct FChandelierFlame
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPuzzleColor FlameColor = EPuzzleColor::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* FlameActor = nullptr;
};

UCLASS()
class MEMORIESFPS_API APuzzleChandelier : public AActor
{
    GENERATED_BODY()

public:
    APuzzleChandelier();

protected:
    virtual void BeginPlay() override;

    FTimerHandle RotationTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    float RotationInterval = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    TArray<EPuzzleColor> ColorRotation;

    void InitColorRotation();
    void InitSolution();
    void RotateExteriorLights();

public:
    // "North", "West", "South", "East"
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    TMap<FName, FExteriorLight> ExteriorLights;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    TMap<FName, FPedestal> Pedestals;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    TMap<FName, FChandelierFlame> Flames;

    // Solution: which color each cardinal must be
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    TMap<FName, EPuzzleColor> Solution;

    // Called from pedestal (BP) when a player stands on it
    UFUNCTION(BlueprintCallable, Category="Puzzle")
    void OnPedestalActivated(FName CardinalPoint, EPuzzleColor PlayerColor);

    void CheckPuzzleSolved();
    void SpawnMissingStatues(int32 MissingCount);

    // Number of active players (set from GameMode or BP)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    int32 CurrentPlayerCount = 4;

    UPROPERTY(BlueprintAssignable, Category="Puzzle")
    FOnFlameColorChanged OnFlameColorChanged;

    UPROPERTY(BlueprintAssignable, Category="Puzzle")
    FOnPuzzleSolved OnPuzzleSolved;

    // Class of the statue to spawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    TSubclassOf<AActor> StatueClass;

    // Spawn points for statues
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    TArray<AActor*> StatueSpawnPoints;
};

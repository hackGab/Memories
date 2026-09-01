#include "APuzzleChandelier.h"
#include "TimerManager.h"
#include "Engine/World.h"

APuzzleChandelier::APuzzleChandelier()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APuzzleChandelier::BeginPlay()
{
    Super::BeginPlay();

    InitColorRotation();
    InitSolution();

    GetWorld()->GetTimerManager().SetTimer(
        RotationTimer,
        this,
        &APuzzleChandelier::RotateExteriorLights,
        RotationInterval,
        true
    );
}

void APuzzleChandelier::InitColorRotation()
{
    ColorRotation.Empty();
    ColorRotation.Add(EPuzzleColor::Red);
    ColorRotation.Add(EPuzzleColor::Blue);
    ColorRotation.Add(EPuzzleColor::Green);
    ColorRotation.Add(EPuzzleColor::Purple);
    ColorRotation.Add(EPuzzleColor::Yellow);
    ColorRotation.Add(EPuzzleColor::Pink);
}

void APuzzleChandelier::InitSolution()
{
    Solution.Empty();

    for (const auto& Elem : ExteriorLights)
    {
        const FName Cardinal = Elem.Key;
        const FExteriorLight& Light = Elem.Value;

        Solution.Add(Cardinal, Light.MissingColor);
    }
}

void APuzzleChandelier::RotateExteriorLights()
{
    for (auto& Elem : ExteriorLights)
    {
        FExteriorLight& Light = Elem.Value;

        Light.RotationIndex = (Light.RotationIndex + 1) % ColorRotation.Num();
        Light.CurrentColor = ColorRotation[Light.RotationIndex];

        // Event BP pour changer la couleur visuelle
    }
}

void APuzzleChandelier::OnPedestalActivated(FName CardinalPoint, EPuzzleColor PlayerColor)
{
    if (!Pedestals.Contains(CardinalPoint) || !Flames.Contains(CardinalPoint))
        return;

    FPedestal& Pedestal = Pedestals[CardinalPoint];
    Pedestal.PlayerColor = PlayerColor;

    FChandelierFlame& Flame = Flames[CardinalPoint];
    Flame.FlameColor = PlayerColor;

    OnFlameColorChanged.Broadcast(CardinalPoint, PlayerColor);

    CheckPuzzleSolved();
}

void APuzzleChandelier::CheckPuzzleSolved()
{
    for (const auto& Elem : Solution)
    {
        const FName Cardinal = Elem.Key;
        const EPuzzleColor RequiredColor = Elem.Value;

        if (!Flames.Contains(Cardinal))
            return;

        const FChandelierFlame& Flame = Flames[Cardinal];

        if (Flame.FlameColor != RequiredColor)
        {
            return;
        }
    }

    OnPuzzleSolved.Broadcast();

    const int32 MissingPlayers = FMath::Max(0, 4 - CurrentPlayerCount);
    const int32 StatueCount = MissingPlayers + 2;

    SpawnMissingStatues(StatueCount);
}

void APuzzleChandelier::SpawnMissingStatues(int32 MissingCount)
{
    if (!StatueClass)
        return;

    UWorld* World = GetWorld();
    if (!World)
        return;

    for (int32 i = 0; i < MissingCount; ++i)
    {
        AActor* SpawnPoint = nullptr;

        if (StatueSpawnPoints.IsValidIndex(i))
        {
            SpawnPoint = StatueSpawnPoints[i];
        }

        FTransform SpawnTransform = SpawnPoint
            ? SpawnPoint->GetActorTransform()
            : GetActorTransform();

        World->SpawnActor<AActor>(StatueClass, SpawnTransform);
    }
}

#include "APuzzleChandelier.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

APuzzleChandelier::APuzzleChandelier()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APuzzleChandelier::BeginPlay()
{
    Super::BeginPlay();

    InitColorRotation();
    BuildFilteredRotations(); 
    InitSolution();

    GetWorld()->GetTimerManager().SetTimer(
        RotationTimer,
        this,
        &APuzzleChandelier::RotateExteriorLights,
        RotationInterval,
        true
    );
    
    for (const auto& Elem : Pedestals)
    {
        PedestalOccupied.Add(Elem.Key, false);
        PedestalActiveColor.Add(Elem.Key, EPuzzleColor::None);
    }
}

void APuzzleChandelier::SetPlayerOnPedestal(
    FName CardinalPoint,
    EPuzzleColor PlayerColor)
{
    if (!Pedestals.Contains(CardinalPoint))
    {
        UE_LOG(LogTemp, Error,
            TEXT("[Chandelier] SetPlayerOnPedestal FAILED: %s does not exist"),
            *CardinalPoint.ToString());
        return;
    }

    UE_LOG(LogTemp, Warning,
        TEXT("[Chandelier] PLAYER ENTER -> %s | Color: %s"),
        *CardinalPoint.ToString(),
        *UEnum::GetValueAsString(PlayerColor));

    // Enregistre le joueur
    PlayerColors.FindOrAdd(CardinalPoint) = PlayerColor;

    // Le joueur est maintenant sur le piédestal
    Pedestals[CardinalPoint].PlayerColor = PlayerColor;

    // Recalcule complètement l'état
    RefreshPedestal(CardinalPoint);
    
    PlayerColors.FindOrAdd(CardinalPoint) =
        PlayerColor;

    EPuzzleColor FinalColor =
        GetPedestalColor(CardinalPoint);

    PedestalOccupied.FindOrAdd(CardinalPoint) =
        FinalColor != EPuzzleColor::None;

    PedestalActiveColor.FindOrAdd(CardinalPoint) =
        FinalColor;

    // Update pedestal state
    Pedestals[CardinalPoint].PlayerColor =
        PlayerColor;

    // Update flame
    if (Flames.Contains(CardinalPoint))
    {
        Flames[CardinalPoint].FlameColor =
            FinalColor;

        UpdateFlameVisual(
            CardinalPoint,
            FinalColor
        );
    }

    // Lock and update exterior light
    if (ExteriorLights.Contains(CardinalPoint))
    {
        FExteriorLight& Light =
            ExteriorLights[CardinalPoint];

        Light.bLocked =
            FinalColor != EPuzzleColor::None;

        Light.CurrentColor =
            FinalColor;

        UpdateLightVisual(
            CardinalPoint,
            FinalColor
        );
    }

    OnFlameColorChanged.Broadcast(
        CardinalPoint,
        FinalColor
    );

    CheckPuzzleSolved();
}
void APuzzleChandelier::SetStatueOnPedestal(
FName CardinalPoint,
EPuzzleColor StatueColor)
{
    if (!Pedestals.Contains(CardinalPoint))
    {
        UE_LOG(LogTemp, Error,
            TEXT("[Chandelier] SetStatueOnPedestal FAILED: %s does not exist"),
            *CardinalPoint.ToString());
        return;
    }

    UE_LOG(LogTemp, Warning,
        TEXT("[Chandelier] STATUE ENTER -> %s | Color: %s"),
        *CardinalPoint.ToString(),
        *UEnum::GetValueAsString(StatueColor));

    StatueColors.FindOrAdd(CardinalPoint) = StatueColor;

    RefreshPedestal(CardinalPoint);
    
    StatueColors.FindOrAdd(CardinalPoint) =
        StatueColor;

    EPuzzleColor FinalColor =
        GetPedestalColor(CardinalPoint);

    PedestalOccupied.FindOrAdd(CardinalPoint) =
        FinalColor != EPuzzleColor::None;

    PedestalActiveColor.FindOrAdd(CardinalPoint) =
        FinalColor;

    // Update flame
    if (Flames.Contains(CardinalPoint))
    {
        Flames[CardinalPoint].FlameColor =
            FinalColor;

        UpdateFlameVisual(
            CardinalPoint,
            FinalColor
        );
    }

    // Lock and update exterior light
    if (ExteriorLights.Contains(CardinalPoint))
    {
        FExteriorLight& Light =
            ExteriorLights[CardinalPoint];

        Light.bLocked =
            FinalColor != EPuzzleColor::None;

        Light.CurrentColor =
            FinalColor;

        UpdateLightVisual(
            CardinalPoint,
            FinalColor
        );
    }

    OnFlameColorChanged.Broadcast(
        CardinalPoint,
        FinalColor
    );

    CheckPuzzleSolved();
}

void APuzzleChandelier::RemovePlayerFromPedestal(
FName CardinalPoint)
{
    if (!Pedestals.Contains(CardinalPoint))
    {
        UE_LOG(LogTemp, Error,
            TEXT("[Chandelier] RemovePlayerFromPedestal FAILED: %s does not exist"),
            *CardinalPoint.ToString());
        return;
    }

    UE_LOG(LogTemp, Warning,
        TEXT("[Chandelier] PLAYER EXIT -> %s"),
        *CardinalPoint.ToString());
    
    PlayerColors.FindOrAdd(CardinalPoint) =
        EPuzzleColor::None;

    Pedestals[CardinalPoint].PlayerColor =
        EPuzzleColor::None;

    RefreshPedestal(CardinalPoint);
}
void APuzzleChandelier::RemoveStatueFromPedestal(
    FName CardinalPoint)
{
    
    if (!Pedestals.Contains(CardinalPoint))
    {
        UE_LOG(LogTemp, Error,  TEXT("[Chandelier] RemoveStatueFromPedestal FAILED: %s does not exist"),
            *CardinalPoint.ToString());
        return;
    }
    
    UE_LOG(LogTemp, Warning,
        TEXT("[Chandelier] STATUE EXIT -> %s"),
        *CardinalPoint.ToString());
    
    StatueColors.FindOrAdd(CardinalPoint) =
        EPuzzleColor::None;

    RefreshPedestal(CardinalPoint);
}

void APuzzleChandelier::RefreshPedestal(
    FName CardinalPoint)
{
    UE_LOG(LogTemp, Warning,
        TEXT("[Chandelier] RefreshPedestal -> %s"),
        *CardinalPoint.ToString());

    EPuzzleColor FinalColor = GetPedestalColor(CardinalPoint);

    const bool bOccupied =
        FinalColor != EPuzzleColor::None;

    UE_LOG(LogTemp, Warning,
        TEXT("[Chandelier] %s | FinalColor = %s | Occupied = %s"),
        *CardinalPoint.ToString(),
        *UEnum::GetValueAsString(FinalColor),
        bOccupied ? TEXT("TRUE") : TEXT("FALSE"));
    
    // État général du piédestal
    PedestalOccupied.FindOrAdd(CardinalPoint) = bOccupied;
    PedestalActiveColor.FindOrAdd(CardinalPoint) = FinalColor;
    
    // Flame
    if (Flames.Contains(CardinalPoint))
    {
        Flames[CardinalPoint].FlameColor = FinalColor;

        UE_LOG(LogTemp, Warning,
            TEXT("[Chandelier] Flame %s -> %s"),
            *CardinalPoint.ToString(),
            *UEnum::GetValueAsString(FinalColor));

        UpdateFlameVisual(
            CardinalPoint,
            FinalColor
        );
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("[Chandelier] NO FLAME registered for %s"),
            *CardinalPoint.ToString());
    }
    
    // Exterior light
    if (ExteriorLights.Contains(CardinalPoint))
    {
        FExteriorLight& Light =
            ExteriorLights[CardinalPoint];

        Light.bLocked = bOccupied;

        UE_LOG(LogTemp, Warning,
            TEXT("[Chandelier] ExteriorLight %s | Locked = %s"),
            *CardinalPoint.ToString(),
            bOccupied ? TEXT("TRUE") : TEXT("FALSE"));

        if (bOccupied)
        {
            // Piédestal occupé :
            // la lumière prend la couleur du joueur/statue
            Light.CurrentColor = FinalColor;

            UpdateLightVisual(
                CardinalPoint,
                FinalColor
            );
        }
        else
        {
            // Piédestal vide :
            // la lumière est déverrouillée.
            // Elle reprendra sa rotation au prochain Tick du timer.

            UE_LOG(LogTemp, Warning,
                TEXT("[Chandelier] %s UNLOCKED -> rotation resumed"),
                *CardinalPoint.ToString());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("[Chandelier] NO EXTERIOR LIGHT registered for %s"),
            *CardinalPoint.ToString());
    }
    // Event

    OnFlameColorChanged.Broadcast(
        CardinalPoint,
        FinalColor
    );

    CheckPuzzleSolved();
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

void APuzzleChandelier::BuildFilteredRotations()
{
    for (auto& Elem : ExteriorLights)
    {
        FExteriorLight& Light = Elem.Value;
        Light.FilteredRotation.Empty();

        for (const EPuzzleColor& Color : ColorRotation)
        {
            if (Color != Light.MissingColor)
            {
                Light.FilteredRotation.Add(Color);
            }
        }

        Light.RotationIndex = 0;
        if (Light.FilteredRotation.Num() > 0)
        {
            Light.CurrentColor = Light.FilteredRotation[0];
        }
    }
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
        FName Cardinal = Elem.Key;
        FExteriorLight& Light = Elem.Value;
        
        if (Light.bLocked)
        {
            continue;
        }

        if (Light.FilteredRotation.Num() == 0)
        {
            continue;
        }

        Light.RotationIndex =
            (Light.RotationIndex + 1) %
            Light.FilteredRotation.Num();

        Light.CurrentColor =
            Light.FilteredRotation[Light.RotationIndex];

        UpdateLightVisual(
            Cardinal,
            Light.CurrentColor
        );
    }
}

void APuzzleChandelier::UpdateLightVisual(
    FName CardinalPoint,
    EPuzzleColor Color)
{
    if (!ExteriorLights.Contains(CardinalPoint))
    {
        return;
    }
    AActor* LightActor =
        ExteriorLights[CardinalPoint].LightActor;

    if (!LightActor)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("No LightActor assigned for %s"),
            *CardinalPoint.ToString()
        );

        return;
    }

    FLinearColor RenderColor =
        ColorToLinearColor(Color);

    if (UStaticMeshComponent* Mesh =
        LightActor->FindComponentByClass<UStaticMeshComponent>())
    {
        UMaterialInstanceDynamic* MID =
            Mesh->CreateAndSetMaterialInstanceDynamic(0);

        if (MID)
        {
            MID->SetVectorParameterValue(
                TEXT("FlameColor"),
                RenderColor
            );

            UE_LOG(
                LogTemp,
                Warning,
                TEXT("Light %s -> %s"),
                *CardinalPoint.ToString(),
                *UEnum::GetValueAsString(Color)
            );
        }
        else
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Could not create MID for light %s"),
                *CardinalPoint.ToString()
            );
        }
    }

    // Update point light
    if (UPointLightComponent* Light =
        LightActor->FindComponentByClass<UPointLightComponent>())
    {
        Light->SetLightColor(RenderColor);
    }
}

void APuzzleChandelier::OnPedestalActivated(
    FName CardinalPoint,
    EPuzzleColor PlayerColor)
{
    UE_LOG(LogTemp, Warning,
       TEXT("[Chandelier] OnPedestalActivated -> forwarding to SetPlayerOnPedestal"));

    SetPlayerOnPedestal(
        CardinalPoint,
        PlayerColor
    );
    if (!Pedestals.Contains(CardinalPoint) ||
        !Flames.Contains(CardinalPoint))
    {
        return;
    }

    // Mark pedestal occupied
    PedestalOccupied.FindOrAdd(CardinalPoint) = true;

    // Store active color
    PedestalActiveColor.FindOrAdd(CardinalPoint) = PlayerColor;

    // Update pedestal
    FPedestal& Pedestal = Pedestals[CardinalPoint];
    Pedestal.PlayerColor = PlayerColor;

    // Update flame
    FChandelierFlame& Flame = Flames[CardinalPoint];
    Flame.FlameColor = PlayerColor;

    UpdateFlameVisual(
        CardinalPoint,
        PlayerColor
    );

    // Lock corresponding exterior light
    if (ExteriorLights.Contains(CardinalPoint))
    {
        FExteriorLight& Light =
            ExteriorLights[CardinalPoint];

        Light.bLocked = true;
        Light.CurrentColor = PlayerColor;

        UpdateLightVisual(
            CardinalPoint,
            PlayerColor
        );
    }

    if (GEngine)
    {
        FString Msg = FString::Printf(
            TEXT("Pedestal %s occupied -> %s"),
            *CardinalPoint.ToString(),
            *UEnum::GetValueAsString(PlayerColor)
        );

        GEngine->AddOnScreenDebugMessage(
            -1,
            3.0f,
            FColor::Cyan,
            Msg
        );
    }

    OnFlameColorChanged.Broadcast(
        CardinalPoint,
        PlayerColor
    );

    CheckPuzzleSolved();
}

void APuzzleChandelier::OnPedestalDeactivated(
FName CardinalPoint)
{
    UE_LOG(LogTemp, Warning,
        TEXT("[Chandelier] OnPedestalDeactivated -> forwarding to RemovePlayerFromPedestal"));

    RemovePlayerFromPedestal(CardinalPoint);
    if (!Pedestals.Contains(CardinalPoint) ||
        !Flames.Contains(CardinalPoint))
    {
        return;
    }

    // Pedestal is now empty
    PedestalOccupied.FindOrAdd(CardinalPoint) = false;

    PedestalActiveColor.FindOrAdd(CardinalPoint) =
        EPuzzleColor::None;

    // Reset pedestal
    Pedestals[CardinalPoint].PlayerColor =
        EPuzzleColor::None;

    // Reset flame
    Flames[CardinalPoint].FlameColor =
        EPuzzleColor::None;

    UpdateFlameVisual(
        CardinalPoint,
        EPuzzleColor::None
    );

    // Unlock exterior light
    if (ExteriorLights.Contains(CardinalPoint))
    {
        FExteriorLight& Light =
            ExteriorLights[CardinalPoint];

        Light.bLocked = false;

        // It will resume rotating on the next timer tick
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.0f,
            FColor::Orange,
            FString::Printf(
                TEXT("Pedestal %s is empty -> rotation resumed"),
                *CardinalPoint.ToString()
            )
        );
    }

    OnFlameColorChanged.Broadcast(
        CardinalPoint,
        EPuzzleColor::None
    );
}

EPuzzleColor APuzzleChandelier::GetPedestalColor(
    FName CardinalPoint) const
{
    // Player
    if (const EPuzzleColor* PlayerColor =
        PlayerColors.Find(CardinalPoint))
    {
        if (*PlayerColor != EPuzzleColor::None)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[Chandelier] GetPedestalColor %s -> PLAYER = %s"),
                *CardinalPoint.ToString(),
                *UEnum::GetValueAsString(*PlayerColor));

            return *PlayerColor;
        }
    }
    
    // Statue
    if (const EPuzzleColor* StatueColor =
        StatueColors.Find(CardinalPoint))
    {
        if (*StatueColor != EPuzzleColor::None)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[Chandelier] GetPedestalColor %s -> STATUE = %s"),
                *CardinalPoint.ToString(),
                *UEnum::GetValueAsString(*StatueColor));

            return *StatueColor;
        }
    }
    // Empty

    UE_LOG(LogTemp, Warning,
        TEXT("[Chandelier] GetPedestalColor %s -> NONE"),
        *CardinalPoint.ToString());

    return EPuzzleColor::None;
}

void APuzzleChandelier::UpdateFlameVisual(FName CardinalPoint, EPuzzleColor Color)
{
    if (!Flames.Contains(CardinalPoint)) return;

    AActor* FlameActor = Flames[CardinalPoint].FlameActor;
    if (!FlameActor) return;

    FLinearColor RenderColor = ColorToLinearColor(Color);

    // If the flame uses a static/skeletal mesh material
    if (UStaticMeshComponent* Mesh = FlameActor->FindComponentByClass<UStaticMeshComponent>())
    {
        UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(0);
        if (MID)
        {
            MID->SetVectorParameterValue(TEXT("FlameColor"), RenderColor);
        }
    }

    // If the flame also has a point light
    if (UPointLightComponent* Light = FlameActor->FindComponentByClass<UPointLightComponent>())
    {
        Light->SetLightColor(RenderColor);
    }
}

FLinearColor APuzzleChandelier::ColorToLinearColor(EPuzzleColor Color) const
{
    switch (Color)
    {
    case EPuzzleColor::Red:    return FLinearColor::Red;
    case EPuzzleColor::Blue:   return FLinearColor::Blue;
    case EPuzzleColor::Green:  return FLinearColor::Green;
    case EPuzzleColor::Purple: return FLinearColor(0.5f, 0.f, 0.5f);
    case EPuzzleColor::Yellow: return FLinearColor::Yellow;
    case EPuzzleColor::Pink:   return FLinearColor(1.f, 0.4f, 0.7f);
    default:                  return FLinearColor::Black;
    }
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

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("=== PUZZLE SOLVED ==="));
    }

    const int32 MissingPlayers = FMath::Max(0, 4 - CurrentPlayerCount);
    const int32 StatueCount = MissingPlayers + 2;

    SpawnMissingStatues(StatueCount);
}

void APuzzleChandelier::SpawnMissingStatues(int32 MissingCount)
{
    if (!StatueClass) return;

    UWorld* World = GetWorld();
    if (!World) return;
    TArray<FName> Cardinals;
    Solution.GetKeys(Cardinals);

    for (int32 i = 0; i < MissingCount; ++i)
    {
        AActor* SpawnPoint = StatueSpawnPoints.IsValidIndex(i) ? StatueSpawnPoints[i] : nullptr;
        FTransform SpawnTransform = SpawnPoint ? SpawnPoint->GetActorTransform() : GetActorTransform();

        AActor* NewStatue = World->SpawnActor<AActor>(StatueClass, SpawnTransform);

        // If StatueClass exposes a BlueprintReadWrite "StatueColor" property, set it:
        if (NewStatue && Cardinals.IsValidIndex(i))
        {
            FName Cardinal = Cardinals[i];
            EPuzzleColor AssignedColor = Solution[Cardinal];

            FProperty* ColorProp = NewStatue->GetClass()->FindPropertyByName(TEXT("StatueColor"));
            if (FByteProperty* ByteProp = CastField<FByteProperty>(ColorProp))
            {
                ByteProp->SetPropertyValue_InContainer(NewStatue, static_cast<uint8>(AssignedColor));
            }
        }
    }
}
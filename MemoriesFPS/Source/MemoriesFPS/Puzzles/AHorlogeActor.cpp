// Made by Pierre-Luc

#include "AHorlogeActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
#include "GamePuzzleHorloge.h"
#include "Kismet/GameplayStatics.h"

AHorlogeActor::AHorlogeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    ClockFaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClockFace"));
    ClockFaceMesh->SetupAttachment(SceneRoot);

    LogoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LogoMesh"));
    LogoMesh->SetupAttachment(SceneRoot);

    // --- Small Hand ---
    SmallHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PetiteAiguille"));
    SmallHandMesh->SetupAttachment(SceneRoot);
    SmallHandMesh->SetGenerateOverlapEvents(true);
    SmallHandMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SmallHandMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // --- Big Hand ---
    BigHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrosseAiguille"));
    BigHandMesh->SetupAttachment(SceneRoot);
    BigHandMesh->SetGenerateOverlapEvents(true);
    BigHandMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BigHandMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // --- Light + Audio ---
    LightCue = CreateDefaultSubobject<UPointLightComponent>(TEXT("LightCue"));
    LightCue->SetupAttachment(SceneRoot);

    AudioSuccess = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioSuccess"));
    AudioSuccess->SetupAttachment(SceneRoot);
    AudioSuccess->bAutoActivate = false;

    AudioFail = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioFail"));
    AudioFail->SetupAttachment(SceneRoot);
    AudioFail->bAutoActivate = false;

    Hours = 0;
    Minutes = 0;
    SelectedHand = EClockHand::None;
}

void AHorlogeActor::BeginPlay()
{
    Super::BeginPlay();

    DebugMessage("Horloge BeginPlay()", FColor::Magenta);

    AudioSuccess->Stop();
    AudioFail->Stop();

    LightCue->SetVisibility(false);
    LightCue->SetIntensity(0.f);

    if (LogoMesh)
    {
        LogoDynMat = LogoMesh->CreateAndSetMaterialInstanceDynamic(0);
    }

    // Bind hover events
    SmallHandMesh->OnBeginCursorOver.AddDynamic(this, &AHorlogeActor::OnSmallHandHoverBegin);
    SmallHandMesh->OnEndCursorOver.AddDynamic(this, &AHorlogeActor::OnSmallHandHoverEnd);

    BigHandMesh->OnBeginCursorOver.AddDynamic(this, &AHorlogeActor::OnBigHandHoverBegin);
    BigHandMesh->OnEndCursorOver.AddDynamic(this, &AHorlogeActor::OnBigHandHoverEnd);

    // Bind click events
    SmallHandMesh->OnClicked.AddDynamic(this, &AHorlogeActor::OnSmallHandClicked);
    BigHandMesh->OnClicked.AddDynamic(this, &AHorlogeActor::OnBigHandClicked);
}

void AHorlogeActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AHorlogeActor::DebugMessage(const FString& Msg, FColor Color)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, Color, Msg);
    }
}

// Sélection des aiguilles

void AHorlogeActor::SelectHand(EClockHand Hand)
{
    SelectedHand = Hand;

    if (DefaultMaterial)
    {
        SmallHandMesh->SetMaterial(0, DefaultMaterial);
        BigHandMesh->SetMaterial(0, DefaultMaterial);
    }

    if (M_OutlineHover)
    {
        if (Hand == EClockHand::Small)
        {
            SmallHandMesh->SetMaterial(0, M_OutlineHover);
        }
        else if (Hand == EClockHand::Big)
        {
            BigHandMesh->SetMaterial(0, M_OutlineHover);
        }
    }

    FString HandName = (Hand == EClockHand::Small ? "Small" : Hand == EClockHand::Big ? "Big" : "None");
    DebugMessage(FString("Selected hand: ") + HandName, FColor::Yellow);
}

void AHorlogeActor::CycleSelectedHand()
{
    switch (SelectedHand)
    {
        case EClockHand::None:
        case EClockHand::Big:
            SelectHand(EClockHand::Small);
            break;
        case EClockHand::Small:
            SelectHand(EClockHand::Big);
            break;
    }
}

// Rotation
void AHorlogeActor::RotateSelectedHand(int32 Amount)
{
    DebugMessage(FString("RotateSelectedHand Amount = ") + FString::FromInt(Amount), FColor::Cyan);

    if (SelectedHand == EClockHand::Small)
    {
        Hours = (Hours + Amount + 12) % 12;
        float Rotation = (Hours / 12.f) * 360.f;
        SmallHandMesh->SetRelativeRotation(FRotator(0.f, Rotation, 0.f));

        AGamePuzzleHorloge* Puzzle = Cast<AGamePuzzleHorloge>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AGamePuzzleHorloge::StaticClass())
        );

        if (Puzzle)
        {
            Puzzle->OnHoursTimeChanged(GetActorLabel(), Hours);
        }

        OnHoursChanged.Broadcast(Hours);
    }
    else if (SelectedHand == EClockHand::Big)
    {
        Minutes = (Minutes + Amount + 60) % 60;
        float Rotation = (Minutes / 60.f) * 360.f;
        BigHandMesh->SetRelativeRotation(FRotator(0.f, Rotation, 0.f));

        AGamePuzzleHorloge* Puzzle = Cast<AGamePuzzleHorloge>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AGamePuzzleHorloge::StaticClass())
        );

        if (Puzzle)
        {
            Puzzle->OnMinutesTimeChanged(GetActorLabel(), Minutes);
        }

        OnMinutesChanged.Broadcast(Minutes);
    }
    else
    {
        DebugMessage("Aucune aiguille sélectionnée !", FColor::Red);
    }
}

// Hover
void AHorlogeActor::OnSmallHandHoverBegin(UPrimitiveComponent* TouchedComponent)
{
    if (M_OutlineHover)
        SmallHandMesh->SetMaterial(0, M_OutlineHover);

    DebugMessage("Hover Small Hand", FColor::Blue);
}

void AHorlogeActor::OnSmallHandHoverEnd(UPrimitiveComponent* TouchedComponent)
{
    if (SelectedHand != EClockHand::Small && DefaultMaterial)
        SmallHandMesh->SetMaterial(0, DefaultMaterial);

    DebugMessage("End Hover Small Hand", FColor::Blue);
}

void AHorlogeActor::OnBigHandHoverBegin(UPrimitiveComponent* TouchedComponent)
{
    if (M_OutlineHover)
        BigHandMesh->SetMaterial(0, M_OutlineHover);

    DebugMessage("Hover Big Hand", FColor::Blue);
}

void AHorlogeActor::OnBigHandHoverEnd(UPrimitiveComponent* TouchedComponent)
{
    if (SelectedHand != EClockHand::Big && DefaultMaterial)
        BigHandMesh->SetMaterial(0, DefaultMaterial);

    DebugMessage("End Hover Big Hand", FColor::Blue);
}

// Click
void AHorlogeActor::OnSmallHandClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
    DebugMessage("Clicked Small Hand", FColor::Red);
    SelectHand(EClockHand::Small);
}

void AHorlogeActor::OnBigHandClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
    DebugMessage("Clicked Big Hand", FColor::Red);
    SelectHand(EClockHand::Big);
}

// Cues son + lumière + logo
void AHorlogeActor::PlaySuccessCue()
{
    LightCue->SetVisibility(true);
    LightCue->SetLightColor(SuccessColor);
    LightCue->SetIntensity(5000.f);

    if (LogoDynMat)
    {
        LogoDynMat->SetVectorParameterValue(LogoEmissiveParamName, SuccessColor);
    }

    AudioSuccess->Play();
}

void AHorlogeActor::PlayFailCue()
{
    LightCue->SetVisibility(true);
    LightCue->SetLightColor(FailColor);
    LightCue->SetIntensity(2000.f);

    if (LogoDynMat)
    {
        LogoDynMat->SetVectorParameterValue(LogoEmissiveParamName, FailColor);
    }

    AudioFail->Play();
}
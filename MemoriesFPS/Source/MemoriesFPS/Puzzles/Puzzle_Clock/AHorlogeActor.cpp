#include "AHorlogeActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GamePuzzleHorloge.h"

AHorlogeActor::AHorlogeActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// Clock face
	ClockFaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClockFace"));
	ClockFaceMesh->SetupAttachment(SceneRoot);

	// Logo
	LogoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LogoMesh"));
	LogoMesh->SetupAttachment(SceneRoot);

	// Pivots (must be created before the hands)
	SmallHandPivot = CreateDefaultSubobject<USceneComponent>(TEXT("SmallHandPivot"));
	SmallHandPivot->SetupAttachment(SceneRoot);

	BigHandPivot = CreateDefaultSubobject<USceneComponent>(TEXT("BigHandPivot"));
	BigHandPivot->SetupAttachment(SceneRoot);

	// Small hand
	SmallHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PetiteAiguille"));
	SmallHandMesh->SetupAttachment(SmallHandPivot);
	SmallHandMesh->SetGenerateOverlapEvents(true);
	SmallHandMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SmallHandMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Big hand
	BigHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrosseAiguille"));
	BigHandMesh->SetupAttachment(BigHandPivot);
	BigHandMesh->SetGenerateOverlapEvents(true);
	BigHandMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BigHandMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Light
	LightCue = CreateDefaultSubobject<UPointLightComponent>(TEXT("LightCue"));
	LightCue->SetupAttachment(SceneRoot);

	// Success audio
	AudioSuccess = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioSuccess"));
	AudioSuccess->SetupAttachment(SceneRoot);
	AudioSuccess->bAutoActivate = false;

	// Fail audio
	AudioFail = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioFail"));
	AudioFail->SetupAttachment(SceneRoot);
	AudioFail->bAutoActivate = false;

	// Initial values
	Hours = 0;
	Minutes = 0;

	SelectedHand = EClockHand::None;

	SmallHandRotation = 0.f;
	BigHandRotation = 0.f;

	TargetSmallHandRotation = 0.f;
	TargetBigHandRotation = 0.f;

	bAnimatingSmallHand = false;
	bAnimatingBigHand = false;
}

// Begin Play

void AHorlogeActor::BeginPlay()
{
	Super::BeginPlay();

	DebugMessage(
		FString::Printf(TEXT("Horloge BeginPlay: %s"), *Symbole),
		FColor::Magenta
	);

	// Audio
	if (AudioSuccess)
	{
		AudioSuccess->Stop();
	}

	if (AudioFail)
	{
		AudioFail->Stop();
	}

	// Light
	if (LightCue)
	{
		LightCue->SetVisibility(false);
		LightCue->SetIntensity(0.f);
	}

	// Logo material
	if (LogoMesh && LogoMesh->GetNumMaterials() > 0)
	{
		LogoDynMat = LogoMesh->CreateAndSetMaterialInstanceDynamic(0);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Horloge %s: LogoMesh has no material in slot 0."), *Symbole);
	}

	// Save the original hand materials (used to remove the outline)
	if (SmallHandMesh) SmallHandOriginalMaterial = SmallHandMesh->GetMaterial(0);
	if (BigHandMesh)   BigHandOriginalMaterial   = BigHandMesh->GetMaterial(0);

	// Bind click events
	if (SmallHandMesh)
	{
		SmallHandMesh->OnClicked.AddDynamic(this, &AHorlogeActor::OnSmallHandClicked);
		SmallHandMesh->OnBeginCursorOver.AddDynamic(this, &AHorlogeActor::OnSmallHandHoverBegin);
		SmallHandMesh->OnEndCursorOver.AddDynamic(this, &AHorlogeActor::OnSmallHandHoverEnd);
	}

	if (BigHandMesh)
	{
		BigHandMesh->OnClicked.AddDynamic(this, &AHorlogeActor::OnBigHandClicked);
		BigHandMesh->OnBeginCursorOver.AddDynamic(this, &AHorlogeActor::OnBigHandHoverBegin);
		BigHandMesh->OnEndCursorOver.AddDynamic(this, &AHorlogeActor::OnBigHandHoverEnd);
	}
	if (SmallHandMesh && SmallHandPivot)
	{
		SmallHandMesh->AttachToComponent(
			SmallHandPivot,
			FAttachmentTransformRules::KeepWorldTransform
		);
	}

	if (BigHandMesh && BigHandPivot)
	{
		BigHandMesh->AttachToComponent(
			BigHandPivot,
			FAttachmentTransformRules::KeepWorldTransform
		);
	}

	// Initial clock position
	RandomizeClockTime();

	SmallHandRotation = GetContinuousHourRotation();
	BigHandRotation = GetMinuteRotation();

	TargetSmallHandRotation = SmallHandRotation;
	TargetBigHandRotation = BigHandRotation;

	if (SmallHandPivot)
	{
		SmallHandPivot->SetRelativeRotation(FRotator(SmallHandRotation, 0.f, 0.f));
	}

	if (BigHandPivot)
	{
		BigHandPivot->SetRelativeRotation(FRotator(BigHandRotation, 0.f, 0.f));
	}
}

// Tick

void AHorlogeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSmallHandAnimation(DeltaTime);
	UpdateBigHandAnimation(DeltaTime);
}

float AHorlogeActor::GetContinuousHourRotation() const
{
	return (Hours * 30.f) + (Minutes * 0.5f);
}

// Get minute rotation

float AHorlogeActor::GetMinuteRotation() const
{
	return Minutes * 6.f;
}

// Start small hand animation

void AHorlogeActor::StartSmallHandAnimation(float Degrees)
{
	if (bAnimatingSmallHand)
	{
		SmallHandAnimationStart = TargetSmallHandRotation;
	}
	else
	{
		SmallHandAnimationStart = SmallHandRotation;
	}

	TargetSmallHandRotation = SmallHandAnimationStart + Degrees;

	SmallHandAnimationElapsed = 0.f;

	const float Distance = FMath::Abs(TargetSmallHandRotation - SmallHandAnimationStart);

	SmallHandAnimationDuration = FMath::Max(Distance / RotationSpeedDegreesPerSecond, 0.08f);

	bAnimatingSmallHand = true;
}

// Start big hand animation

void AHorlogeActor::StartBigHandAnimation(float Degrees)
{
	if (bAnimatingBigHand)
	{
		BigHandAnimationStart = TargetBigHandRotation;
	}
	else
	{
		BigHandAnimationStart = BigHandRotation;
	}

	TargetBigHandRotation = BigHandAnimationStart + Degrees;

	BigHandAnimationElapsed = 0.f;

	const float Distance = FMath::Abs(TargetBigHandRotation - BigHandAnimationStart);

	BigHandAnimationDuration = FMath::Max(Distance / RotationSpeedDegreesPerSecond, 0.08f);

	bAnimatingBigHand = true;
}

// Update small hand

void AHorlogeActor::UpdateSmallHandAnimation(float DeltaTime)
{
	if (!bAnimatingSmallHand || !SmallHandPivot)
	{
		return;
	}

	SmallHandAnimationElapsed += DeltaTime;

	const float Alpha = FMath::Clamp(SmallHandAnimationElapsed / SmallHandAnimationDuration, 0.f, 1.f);

	// SmoothStep ease-in / ease-out
	const float EaseAlpha = Alpha * Alpha * (3.f - 2.f * Alpha);

	SmallHandRotation = FMath::Lerp(SmallHandAnimationStart, TargetSmallHandRotation, EaseAlpha);

	SmallHandPivot->SetRelativeRotation(FRotator(SmallHandRotation, 0.f, 0.f));
	if (Alpha >= 1.f)
	{
		SmallHandRotation = TargetSmallHandRotation;

		SmallHandPivot->SetRelativeRotation(FRotator(SmallHandRotation, 0.f, 0.f));

		bAnimatingSmallHand = false;
	}
}

// Update big hand

void AHorlogeActor::UpdateBigHandAnimation(float DeltaTime)
{
	if (!bAnimatingBigHand || !BigHandPivot)
	{
		return;
	}

	BigHandAnimationElapsed += DeltaTime;

	const float Alpha = FMath::Clamp(BigHandAnimationElapsed / BigHandAnimationDuration, 0.f, 1.f);

	// SmoothStep ease-in / ease-out
	const float EaseAlpha = Alpha * Alpha * (3.f - 2.f * Alpha);

	BigHandRotation = FMath::Lerp(BigHandAnimationStart, TargetBigHandRotation, EaseAlpha);

	BigHandPivot->SetRelativeRotation(FRotator(BigHandRotation, 0.f, 0.f));
	if (Alpha >= 1.f)
	{
		BigHandRotation = TargetBigHandRotation;

		BigHandPivot->SetRelativeRotation(FRotator(BigHandRotation, 0.f, 0.f));

		bAnimatingBigHand = false;
		bWaitingForClockAnimation = false;
	}
}

// Debug

void AHorlogeActor::DebugMessage(const FString& Msg, FColor Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, Color, Msg);
	}
}

// Materials / outline
// A hand is outlined if it is hovered OR selected. Otherwise it uses its original material.

void AHorlogeActor::UpdateHandMaterials()
{
	if (SmallHandMesh && SmallHandOriginalMaterial)
	{
		const bool bOutline = bSmallHovered || SelectedHand == EClockHand::Small;
		SmallHandMesh->SetMaterial(
			0,
			(bOutline && M_OutlineHover) ? M_OutlineHover : SmallHandOriginalMaterial
		);
	}

	if (BigHandMesh && BigHandOriginalMaterial)
	{
		const bool bOutline = bBigHovered || SelectedHand == EClockHand::Big;
		BigHandMesh->SetMaterial(
			0,
			(bOutline && M_OutlineHover) ? M_OutlineHover : BigHandOriginalMaterial
		);
	}
}

// Select hand

void AHorlogeActor::SelectHand(EClockHand Hand)
{
	SelectedHand = Hand;
	UpdateHandMaterials();
	OnSelectionChanged(Hand);
}

// Clear selection (removes outline)

void AHorlogeActor::ClearSelection()
{
	bSmallHovered = false;
	bBigHovered = false;
	SelectHand(EClockHand::None);
}

// Cycle selected hand

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

	default:
		break;
	}
}

void AHorlogeActor::RotateSelectedHand(int32 Amount)
{
	if (Amount == 0)
	{
		return;
	}

	if (bIsLocked)
	{
		DebugMessage("Clock is locked", FColor::Red);
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastRotationTime < RotationRepeatInterval)
	{
		return; // too soon since the last step, ignore this call
	}
	LastRotationTime = CurrentTime;

	AGamePuzzleHorloge* Puzzle = Cast<AGamePuzzleHorloge>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AGamePuzzleHorloge::StaticClass())
	);

	// SMALL HAND
	if (SelectedHand == EClockHand::Small)
	{
		Hours = (Hours + Amount + 12) % 12;

		// One hour = 30 degrees
		const float Degrees = Amount * 30.f;

		StartSmallHandAnimation(Degrees);

		if (Puzzle)
		{
			Puzzle->OnClockTimeChanged(Symbole, Hours, Minutes);
		}

		OnHoursChanged.Broadcast(Hours);
	}
	// BIG HAND
	else if (SelectedHand == EClockHand::Big)
	{
		const int32 OldMinutes = Minutes;

		// Calculate new minutes
		Minutes = (Minutes + Amount + 60) % 60;

		// Detect hour rollover
		if (Amount > 0 && OldMinutes == 59 && Minutes == 0)
		{
			Hours = (Hours + 1) % 12;
		}
		else if (Amount < 0 && OldMinutes == 0 && Minutes == 59)
		{
			Hours = (Hours - 1 + 12) % 12;
		}

		const float BigDegrees = Amount * 6.f;
		StartBigHandAnimation(BigDegrees);

		const float SmallDegrees = Amount * 0.5f;
		StartSmallHandAnimation(SmallDegrees);

		// Notify puzzle
		if (Puzzle)
		{
			Puzzle->OnMinutesTimeChanged(Symbole, Minutes);
			Puzzle->OnHoursTimeChanged(Symbole, Hours);
		}

		OnMinutesChanged.Broadcast(Minutes);
		OnHoursChanged.Broadcast(Hours);
	}
	else
	{
		DebugMessage(TEXT("Aucune aiguille sélectionnée !"), FColor::Red);
		return;
	}

	PrintCurrentClockTime();
}

// Sync to puzzle values

void AHorlogeActor::SyncToPuzzleValues()
{
	SmallHandRotation = GetContinuousHourRotation();
	BigHandRotation = GetMinuteRotation();

	TargetSmallHandRotation = SmallHandRotation;
	TargetBigHandRotation = BigHandRotation;

	bAnimatingSmallHand = false;
	bAnimatingBigHand = false;

	if (SmallHandPivot)
	{
		SmallHandPivot->SetRelativeRotation(FRotator(SmallHandRotation, 0.f, 0.f));
	}

	if (BigHandPivot)
	{
		BigHandPivot->SetRelativeRotation(FRotator(BigHandRotation, 0.f, 0.f));
	}
}

// Hover

void AHorlogeActor::OnSmallHandHoverBegin(UPrimitiveComponent* TouchedComponent)
{
	bSmallHovered = true;
	UpdateHandMaterials();
}

void AHorlogeActor::OnSmallHandHoverEnd(UPrimitiveComponent* TouchedComponent)
{
	bSmallHovered = false;
	UpdateHandMaterials();
}

void AHorlogeActor::OnBigHandHoverBegin(UPrimitiveComponent* TouchedComponent)
{
	bBigHovered = true;
	UpdateHandMaterials();
}

void AHorlogeActor::OnBigHandHoverEnd(UPrimitiveComponent* TouchedComponent)
{
	bBigHovered = false;
	UpdateHandMaterials();
}

void AHorlogeActor::LockClock()
{
	bIsLocked = true;

	// Remove the outline when the clock is solved
	ClearSelection();

	if (AudioSuccess)
	{
		AudioSuccess->Play();
	}

	if (LogoDynMat)
	{
		LogoDynMat->SetVectorParameterValue(LogoEmissiveParamName, SuccessColor);
	}
}

void AHorlogeActor::ActivatePuzzleEntry()
{
	// Light intensity up
	if (LightCue)
	{
		LightCue->SetIntensity(5000.f);
	}

	// Logo lights up
	if (LogoDynMat)
	{
		LogoDynMat->SetVectorParameterValue(LogoEmissiveParamName, FLinearColor(1.f, 1.f, 1.f));
	}

	// Hands re-center slightly
	StartSmallHandAnimation(5.f);
	StartBigHandAnimation(-5.f);
}

// Click

void AHorlogeActor::OnSmallHandClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (ButtonPressed != EKeys::LeftMouseButton)
	{
		return;
	}

	DebugMessage(TEXT("Clicked Small Hand"), FColor::Red);

	SelectHand(EClockHand::Small);
}

void AHorlogeActor::OnBigHandClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (ButtonPressed != EKeys::LeftMouseButton)
	{
		return;
	}

	DebugMessage(TEXT("Clicked Big Hand"), FColor::Red);

	SelectHand(EClockHand::Big);
}

// Success cue

void AHorlogeActor::PlaySuccessCue()
{
	// Light
	if (LightCue)
	{
		LightCue->SetVisibility(true);
		LightCue->SetLightColor(FLinearColor::Green);
		LightCue->SetIntensity(500.f);
	}

	// Sound
	if (AudioSuccess)
	{
		AudioSuccess->Play();
	}

	// Logo
	if (!LogoDynMat && LogoMesh && LogoMesh->GetNumMaterials() > 0)
	{
		LogoDynMat = LogoMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (LogoDynMat)
	{
		LogoDynMat->SetVectorParameterValue(LogoEmissiveParamName, SuccessColor);
	}
}

void AHorlogeActor::RandomizeClockTime()
{
	AGamePuzzleHorloge* Puzzle = Cast<AGamePuzzleHorloge>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AGamePuzzleHorloge::StaticClass())
	);

	if (!Puzzle)
	{
		UE_LOG(LogTemp, Warning, TEXT("No GamePuzzleHorloge found for clock %s"), *Symbole);
		return;
	}

	// Find this clock's solution
	FHorlogeSolutionConfig* Solution = Puzzle->HorlogeSolutionLookup.Find(Symbole);

	if (!Solution)
	{
		UE_LOG(LogTemp, Warning, TEXT("No solution found for clock %s"), *Symbole);
		return;
	}

	const int32 SolutionHours = FMath::RoundToInt(Solution->ShouldBeTimeHours);
	const int32 SolutionMinutes = FMath::RoundToInt(Solution->ShouldBeTimeMinutes);

	int32 NewHours = 0;
	int32 NewMinutes = 0;

	// Try several times to find a sufficiently different time
	for (int32 Attempt = 0; Attempt < 100; ++Attempt)
	{
		NewHours = FMath::RandRange(0, 11);
		NewMinutes = FMath::RandRange(0, 59);

		const int32 HourDifference = FMath::Abs(NewHours - SolutionHours);
		const int32 MinuteDifference = FMath::Abs(NewMinutes - SolutionMinutes);

		if (HourDifference >= MinimumHourDifference ||
			MinuteDifference >= MinimumMinuteDifference)
		{
			break;
		}
	}

	Hours = NewHours;
	Minutes = NewMinutes;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Clock %s randomized to %02d:%02d | Solution = %02d:%02d"),
		*Symbole,
		Hours,
		Minutes,
		SolutionHours,
		SolutionMinutes
	);
}

// Fail cue

void AHorlogeActor::PlayFailCue()
{
	// Light OFF
	if (LightCue)
	{
		LightCue->SetVisibility(false);
		LightCue->SetLightColor(FLinearColor::Red);
	}

	// Sound
	if (AudioFail)
	{
		AudioFail->Play();
	}

	// Logo
	if (!LogoDynMat && LogoMesh && LogoMesh->GetNumMaterials() > 0)
	{
		LogoDynMat = LogoMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (LogoDynMat)
	{
		LogoDynMat->SetVectorParameterValue(LogoEmissiveParamName, FailColor);
	}
}

void AHorlogeActor::PrintCurrentClockTime()
{
	const FString CurrentTime = FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);

	DebugMessage(
		FString::Printf(TEXT("Horloge %s : %s"), *Symbole, *CurrentTime),
		FColor::Yellow
	);
}
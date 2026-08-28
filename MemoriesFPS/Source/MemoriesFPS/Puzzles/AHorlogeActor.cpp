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

	SceneRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("SceneRoot")
	);

	RootComponent = SceneRoot;
	// Clock face

	ClockFaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("ClockFace")
	);

	ClockFaceMesh->SetupAttachment(SceneRoot);
	
	// Logo
	LogoMesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("LogoMesh")
	);

	LogoMesh->SetupAttachment(SceneRoot);
	// Small hand

	SmallHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("PetiteAiguille")
	);

	SmallHandMesh->SetupAttachment(SceneRoot);

	SmallHandMesh->SetGenerateOverlapEvents(true);

	SmallHandMesh->SetCollisionEnabled(
		ECollisionEnabled::QueryOnly
	);

	SmallHandMesh->SetCollisionResponseToChannel(
		ECC_Visibility,
		ECR_Block
	);

	// Big hand
	BigHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("GrosseAiguille")
	);

	BigHandMesh->SetupAttachment(SceneRoot);

	BigHandMesh->SetGenerateOverlapEvents(true);

	BigHandMesh->SetCollisionEnabled(
		ECollisionEnabled::QueryOnly
	);

	BigHandMesh->SetCollisionResponseToChannel(
		ECC_Visibility,
		ECR_Block
	);

	// Light

	LightCue = CreateDefaultSubobject<UPointLightComponent>(
		TEXT("LightCue")
	);

	LightCue->SetupAttachment(SceneRoot);
	
	// Success audio

	AudioSuccess = CreateDefaultSubobject<UAudioComponent>(
		TEXT("AudioSuccess")
	);

	AudioSuccess->SetupAttachment(SceneRoot);

	AudioSuccess->bAutoActivate = false;
	
	// Fail audio
	AudioFail = CreateDefaultSubobject<UAudioComponent>(
		TEXT("AudioFail")
	);

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
		FString::Printf(
			TEXT("Horloge BeginPlay: %s"),
			*Symbole
		),
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
		LogoDynMat =
			LogoMesh->CreateAndSetMaterialInstanceDynamic(0);
	}
	else
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Horloge %s: LogoMesh has no material in slot 0."),
			*Symbole
		);
	}
	

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
	
	// Initial clock position
	RandomizeClockTime();

	SmallHandRotation = GetContinuousHourRotation();
	BigHandRotation = GetMinuteRotation();

	TargetSmallHandRotation = SmallHandRotation;
	TargetBigHandRotation = BigHandRotation;

	if (SmallHandMesh)
	{
		SmallHandMesh->SetRelativeRotation(
			FRotator(
				SmallHandRotation,
				0.f,
				0.f
			)
		);
	}

	if (BigHandMesh)
	{
		BigHandMesh->SetRelativeRotation(
			FRotator(
				BigHandRotation,
				0.f,
				0.f
			)
		);
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
	return
		(Hours * 30.f) +
		(Minutes * 0.5f);
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
	TargetSmallHandRotation =
		SmallHandAnimationStart + Degrees;

	SmallHandAnimationElapsed = 0.f;

	float Distance = FMath::Abs(
		TargetSmallHandRotation -
		SmallHandAnimationStart
	);

	SmallHandAnimationDuration =
		FMath::Max(
			Distance / RotationSpeedDegreesPerSecond,
			0.08f
		);

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

	TargetBigHandRotation =
		BigHandAnimationStart + Degrees;

	BigHandAnimationElapsed = 0.f;

	float Distance = FMath::Abs(
		TargetBigHandRotation -
		BigHandAnimationStart
	);

	BigHandAnimationDuration =
		FMath::Max(
			Distance / RotationSpeedDegreesPerSecond,
			0.08f
		);

	bAnimatingBigHand = true;
}
// Update small hand
void AHorlogeActor::UpdateSmallHandAnimation(float DeltaTime)
{
	if (!bAnimatingSmallHand || !SmallHandMesh)
	{
		return;
	}

	SmallHandAnimationElapsed += DeltaTime;

	float Alpha =
		FMath::Clamp(
			SmallHandAnimationElapsed /
			SmallHandAnimationDuration,
			0.f,
			1.f
		);

	// SmoothStep:
	// Starts slowly
	// Speeds up
	// Slows down before stopping
	float EaseAlpha =
		Alpha * Alpha * (3.f - 2.f * Alpha);

	SmallHandRotation =
		FMath::Lerp(
			SmallHandAnimationStart,
			TargetSmallHandRotation,
			EaseAlpha
		);

	SmallHandMesh->SetRelativeRotation(
		FRotator(
			SmallHandRotation,
			0.f,
			0.f
		)
	);

	if (Alpha >= 1.f)
	{
		SmallHandRotation = TargetSmallHandRotation;

		SmallHandMesh->SetRelativeRotation(
			FRotator(
				SmallHandRotation,
				0.f,
				0.f
			)
		);
		bAnimatingSmallHand = false;
	}
}

// Update big hand

void AHorlogeActor::UpdateBigHandAnimation(float DeltaTime)
{
	if (!bAnimatingBigHand || !BigHandMesh)
	{
		return;
	}

	BigHandAnimationElapsed += DeltaTime;

	float Alpha =
		FMath::Clamp(
			BigHandAnimationElapsed /
			BigHandAnimationDuration,
			0.f,
			1.f
		);

	// SmoothStep ease-in/ease-out
	float EaseAlpha =
		Alpha * Alpha * (3.f - 2.f * Alpha);

	BigHandRotation =
		FMath::Lerp(
			BigHandAnimationStart,
			TargetBigHandRotation,
			EaseAlpha
		);

	BigHandMesh->SetRelativeRotation(
		FRotator(
			BigHandRotation,
			0.f,
			0.f
		)
	);

	if (Alpha >= 1.f)
	{
		BigHandRotation = TargetBigHandRotation;

		BigHandMesh->SetRelativeRotation(
			FRotator(
				BigHandRotation,
				0.f,
				0.f
			)
		);
		
		bWaitingForClockAnimation = false;
	}
}
// Debug

void AHorlogeActor::DebugMessage(
	const FString& Msg,
	FColor Color
)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			Color,
			Msg
		);
	}
}

// Select hand

void AHorlogeActor::SelectHand(EClockHand Hand)
{
	SelectedHand = Hand;

	if (DefaultMaterial)
	{
		if (SmallHandMesh)
		{
			SmallHandMesh->SetMaterial(
				0,
				DefaultMaterial
			);
		}

		if (BigHandMesh)
		{
			BigHandMesh->SetMaterial(
				0,
				DefaultMaterial
			);
		}
	}

	if (M_OutlineHover)
	{
		if (Hand == EClockHand::Small)
		{
			SmallHandMesh->SetMaterial(
				0,
				M_OutlineHover
			);
		}
		else if (Hand == EClockHand::Big)
		{
			BigHandMesh->SetMaterial(
				0,
				M_OutlineHover
			);
		}
	}
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
	DebugMessage(
		FString("RotateSelectedHand Amount = ") +
		FString::FromInt(Amount),
		FColor::Cyan
	);

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

	AGamePuzzleHorloge* Puzzle =
		Cast<AGamePuzzleHorloge>(
			UGameplayStatics::GetActorOfClass(
				GetWorld(),
				AGamePuzzleHorloge::StaticClass()
			)
		);
	// SMALL HAND
	if (SelectedHand == EClockHand::Small)
	{
		Hours = (Hours + Amount + 12) % 12;

		// One hour = 30 degrees
		float Degrees = Amount * 30.f;

		StartSmallHandAnimation(Degrees);

		if (Puzzle)
		{
			Puzzle->OnHoursTimeChanged(
				Symbole,
				Hours
			);
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
			Puzzle->OnMinutesTimeChanged(
				Symbole,
				Minutes
			);

			Puzzle->OnHoursTimeChanged(
				Symbole,
				Hours
			);
		}

		OnMinutesChanged.Broadcast(Minutes);
		OnHoursChanged.Broadcast(Hours);
	}
	else
	{
		DebugMessage(
			TEXT("Aucune aiguille sélectionnée !"),
			FColor::Red);
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

	if (SmallHandMesh)
	{
		SmallHandMesh->SetRelativeRotation(
			FRotator(
				SmallHandRotation,
				0.f,
				0.f
			)
		);
	}

	if (BigHandMesh)
	{
		BigHandMesh->SetRelativeRotation(
			FRotator(
				BigHandRotation,
				0.f,
				0.f
			)
		);
	}
}

// Small hand hover

void AHorlogeActor::OnSmallHandHoverBegin(
	UPrimitiveComponent* TouchedComponent
)
{
	if (M_OutlineHover && SmallHandMesh)
	{
		SmallHandMesh->SetMaterial(
			0,
			M_OutlineHover
		);
	}

	DebugMessage(
		TEXT("Hover Small Hand"),
		FColor::Blue
	);
}


void AHorlogeActor::OnSmallHandHoverEnd(
	UPrimitiveComponent* TouchedComponent
)
{
	if (
		SelectedHand != EClockHand::Small &&
		DefaultMaterial &&
		SmallHandMesh
	)
	{
		SmallHandMesh->SetMaterial(
			0,
			DefaultMaterial
		);
	}

	DebugMessage(
		TEXT("End Hover Small Hand"),
		FColor::Blue
	);
}

void AHorlogeActor::LockClock()
{
	bIsLocked = true;

	if (AudioSuccess)
	{
		AudioSuccess->Play();
	}

	if (LogoDynMat)
	{
		LogoDynMat->SetVectorParameterValue(
			LogoEmissiveParamName,
			SuccessColor
		);
	}
}
void AHorlogeActor::ActivatePuzzleEntry()
{
	// Lumière qui augmente
	if (LightCue)
		LightCue->SetIntensity(5000.f);

	// Logo qui s’allume
	if (LogoDynMat)
		LogoDynMat->SetVectorParameterValue(LogoEmissiveParamName, FLinearColor(1.f, 1.f, 1.f));

	// Aiguilles qui se recentrent légèrement
	StartSmallHandAnimation(5.f); // petit mouvement
	StartBigHandAnimation(-5.f);  // petit mouvement inverse
}
void AHorlogeActor::OnBigHandHoverBegin(
	UPrimitiveComponent* TouchedComponent
)
{
	if (M_OutlineHover && BigHandMesh)
	{
		BigHandMesh->SetMaterial(
			0,
			M_OutlineHover
		);
	}
	DebugMessage(
		TEXT("Hover Big Hand"),
		FColor::Blue
	);
}


void AHorlogeActor::OnBigHandHoverEnd(
	UPrimitiveComponent* TouchedComponent
)
{
	if (
		SelectedHand != EClockHand::Big &&
		DefaultMaterial &&
		BigHandMesh
	)
	{
		BigHandMesh->SetMaterial(
			0,
			DefaultMaterial
		);
	}

	DebugMessage(
		TEXT("End Hover Big Hand"),
		FColor::Blue
	);
}
// Click


void AHorlogeActor::OnSmallHandClicked(
	UPrimitiveComponent* TouchedComponent,
	FKey ButtonPressed
)
{
	DebugMessage(
		TEXT("Clicked Small Hand"),
		FColor::Red
	);

	SelectHand(EClockHand::Small);
}

void AHorlogeActor::OnBigHandClicked(
	UPrimitiveComponent* TouchedComponent,
	FKey ButtonPressed
)
{
	DebugMessage(
		TEXT("Clicked Big Hand"),
		FColor::Red
	);

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

		// Give it a visible intensity
		LightCue->SetIntensity(500.f);
	}
	
	// Sound
	if (AudioSuccess)
	{
		AudioSuccess->Play();
	}
	// Logo

	if (
		!LogoDynMat &&
		LogoMesh &&
		LogoMesh->GetNumMaterials() > 0
	)
	{
		LogoDynMat =
			LogoMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (LogoDynMat)
	{
		LogoDynMat->SetVectorParameterValue(
			LogoEmissiveParamName,
			SuccessColor
		);
	}
}
void AHorlogeActor::RandomizeClockTime()
{
	AGamePuzzleHorloge* Puzzle =
		Cast<AGamePuzzleHorloge>(
			UGameplayStatics::GetActorOfClass(
				GetWorld(),
				AGamePuzzleHorloge::StaticClass()
			)
		);

	if (!Puzzle)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("No GamePuzzleHorloge found for clock %s"),
			*Symbole
		);
		return;
	}

	// Find this clock's solution
	FHorlogeSolutionConfig* Solution =
		Puzzle->HorlogeSolutionLookup.Find(Symbole);

	if (!Solution)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("No solution found for clock %s"),
			*Symbole
		);
		return;
	}

	const int32 SolutionHours =
		FMath::RoundToInt(Solution->ShouldBeTimeHours);

	const int32 SolutionMinutes =
		FMath::RoundToInt(Solution->ShouldBeTimeMinutes);

	int32 NewHours;
	int32 NewMinutes;

	// Try several times to find a sufficiently different time
	for (int32 Attempt = 0; Attempt < 100; ++Attempt)
	{
		NewHours = FMath::RandRange(0, 11);
		NewMinutes = FMath::RandRange(0, 59);

		const int32 HourDifference =
			FMath::Abs(NewHours - SolutionHours);

		const int32 MinuteDifference =
			FMath::Abs(NewMinutes - SolutionMinutes);

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
	if (
		!LogoDynMat &&
		LogoMesh &&
		LogoMesh->GetNumMaterials() > 0
	)
	{
		LogoDynMat =
			LogoMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (LogoDynMat)
	{
		LogoDynMat->SetVectorParameterValue(
			LogoEmissiveParamName,
			FailColor
		);
	}
}
void AHorlogeActor::PrintCurrentClockTime()
{
	FString CurrentTime = FString::Printf(
		TEXT("%02d:%02d"),
		Hours,
		Minutes
	);

	DebugMessage(
		FString::Printf(
			TEXT("Horloge %s : %s"),
			*Symbole,
			*CurrentTime
		),
		FColor::Yellow
	);
}

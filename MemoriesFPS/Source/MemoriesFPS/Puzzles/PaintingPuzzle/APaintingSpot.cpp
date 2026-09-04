// PaintingSpot.cpp
#include "APaintingSpot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

APaintingSpot::APaintingSpot()
{
	PrimaryActorTick.bCanEverTick = false;

	SpotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpotMesh"));
	SetRootComponent(SpotMesh);

	SpotTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("SpotTrigger"));
	SpotTrigger->SetupAttachment(SpotMesh);

	CorrectPaintingID = 0;
	RequiredRotationIndex = 0;
	CurrentPainting = nullptr;
}

void APaintingSpot::BeginPlay()
{
	Super::BeginPlay();
}

void APaintingSpot::AttachPainting(AAPaintingFrame* Painting)
{
	if (!Painting) return;

	CurrentPainting = Painting;
	Painting->bIsPlaced = true;

	Painting->AttachToComponent(SpotMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Painting->SetActorRelativeLocation(FVector::ZeroVector);

	// Optionnel : reset rotation
	Painting->RotationIndex = 0;
	Painting->FrameMesh->SetRelativeRotation(FRotator::ZeroRotator);
}

bool APaintingSpot::IsCorrect() const
{
	if (!CurrentPainting) return false;

	const bool bIDMatch = (CurrentPainting->PaintingID == CorrectPaintingID);
	const bool bRotMatch = (CurrentPainting->RotationIndex == RequiredRotationIndex);

	return bIDMatch && bRotMatch;
}

// PaintingFrame.cpp
#include "APaintingFrame.h"

APaintingFrame::APaintingFrame()
{
	PrimaryActorTick.bCanEverTick = false;

	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMesh"));
	SetRootComponent(FrameMesh);

	bIsPlaced = false;
	RotationIndex = 0;
}

void APaintingFrame::BeginPlay()
{
	Super::BeginPlay();
}

void APaintingFrame::RotateOnWall()
{
	if (!bIsPlaced) return;

	RotationIndex = (RotationIndex + 1) % 4;

	const FRotator NewRot(0.f, RotationIndex * RotationStepDegrees, 0.f);
	FrameMesh->SetRelativeRotation(NewRot);
}

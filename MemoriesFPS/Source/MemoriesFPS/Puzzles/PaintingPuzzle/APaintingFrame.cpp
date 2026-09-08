// PaintingFrame.cpp
#include "APaintingFrame.h"

AAPaintingFrame::AAPaintingFrame()
{
	PrimaryActorTick.bCanEverTick = false;

	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMesh"));
	SetRootComponent(FrameMesh);

	bIsPlaced = false;
	RotationIndex = 0;
}

void AAPaintingFrame::BeginPlay()
{
	Super::BeginPlay();
}

void AAPaintingFrame::RotateOnWall()
{
	if (!bIsPlaced) return;

	RotationIndex = (RotationIndex + 1) % 4;

	const FRotator NewRot(0.f, RotationIndex * RotationStepDegrees, 0.f);
	FrameMesh->SetRelativeRotation(NewRot);
}

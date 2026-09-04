// PaintingFrame.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APaintingFrame.generated.h"

UCLASS()
class YOURGAME_API AAPaintingFrame : public AActor
{
	GENERATED_BODY()

public:
	AAPaintingFrame();

protected:
	virtual void BeginPlay() override;

public:
	// Mesh du cadre
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* FrameMesh;

	// ID du cadre (pour le puzzle)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	int32 PaintingID;

	// Est-ce que le cadre est placé sur un spot ?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Puzzle")
	bool bIsPlaced;

	// Index de rotation (0–3)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Puzzle")
	int32 RotationIndex;

	// Rotation step (90°)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	float RotationStepDegrees = 90.f;

	// Appelé quand le joueur veut faire pivoter le cadre
	UFUNCTION(BlueprintCallable, Category="Puzzle")
	void RotateOnWall();
};

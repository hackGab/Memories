// PaintingSpot.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APaintingFrame.h"
#include "APaintingSpot.generated.h"

UCLASS()
class YOURGAME_API APaintingSpot : public AActor
{
	GENERATED_BODY()

public:
	APaintingSpot();

protected:
	virtual void BeginPlay() override;

public:
	// Mesh du spot (cadre vide)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* SpotMesh;

	// Trigger pour interaction / placement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* SpotTrigger;

	// ID du cadre correct
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	int32 CorrectPaintingID;

	// Rotation requise (0–3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	int32 RequiredRotationIndex;

	// Cadre actuellement placé
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Puzzle")
	AAPaintingFrame* CurrentPainting;

	// Attacher un cadre à ce spot
	UFUNCTION(BlueprintCallable, Category="Puzzle")
	void AttachPainting(AAPaintingFrame* Painting);

	// Vérifier si ce spot est correct
	UFUNCTION(BlueprintCallable, Category="Puzzle")
	bool IsCorrect() const;
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AHorlogeActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UPointLightComponent;
class UAudioComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class AGamePuzzleHorloge;

UENUM(BlueprintType)
enum class EClockHand : uint8
{
	None,
	Small,
	Big
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoursChanged, int32, NewHours);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinutesChanged, int32, NewMinutes);

UCLASS()
class MEMORIESFPS_API AHorlogeActor : public AActor
{
	GENERATED_BODY()

public:

	AHorlogeActor();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Horloge")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Horloge")
	UStaticMeshComponent* ClockFaceMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Horloge")
	UStaticMeshComponent* LogoMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Horloge")
	UStaticMeshComponent* SmallHandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Horloge")
	UStaticMeshComponent* BigHandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Horloge")
	UPointLightComponent* LightCue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Horloge")
	UAudioComponent* AudioSuccess;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Horloge")
	UAudioComponent* AudioFail;
	
	// Materials

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horloge|Materials")
	UMaterialInterface* DefaultMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horloge|Materials")
	UMaterialInterface* M_OutlineHover;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horloge|Materials")
	FName LogoEmissiveParamName = TEXT("EmissiveColor");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horloge|Materials")
	FLinearColor SuccessColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horloge|Materials")
	FLinearColor FailColor = FLinearColor::Red;

	UPROPERTY()
	UMaterialInstanceDynamic* LogoDynMat;
	
	// Rotation

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horloge|Rotation")
	float RotationSpeedDegreesPerSecond = 180.f;

	float SmallHandRotation = 0.f;
	float BigHandRotation = 0.f;

	float TargetSmallHandRotation = 0.f;
	float TargetBigHandRotation = 0.f;

	// Animation state
	float SmallHandAnimationStart = 0.f;
	float BigHandAnimationStart = 0.f;

	float SmallHandAnimationElapsed = 0.f;
	float BigHandAnimationElapsed = 0.f;

	float SmallHandAnimationDuration = 0.2f;
	float BigHandAnimationDuration = 0.15f;

	bool bAnimatingSmallHand = false;
	bool bAnimatingBigHand = false;
	
	// Input state

	bool bRotateLeft = false;
	bool bRotateRight = false;
	
	// Selected hand

	EClockHand SelectedHand;

	// Hover / click
	UFUNCTION()
	void OnSmallHandHoverBegin(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnSmallHandHoverEnd(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnBigHandHoverBegin(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnBigHandHoverEnd(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnSmallHandClicked(
		UPrimitiveComponent* TouchedComponent,
		FKey ButtonPressed
	);

	UFUNCTION()
	void OnBigHandClicked(
		UPrimitiveComponent* TouchedComponent,
		FKey ButtonPressed
	);

public:
	// Clock data

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horloge")
	FString Symbole;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horloge")
	int32 Hours = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horloge")
	int32 Minutes = 0;
	
	// Delegates

	UPROPERTY(BlueprintAssignable, Category = "Horloge")
	FOnHoursChanged OnHoursChanged;

	UPROPERTY(BlueprintAssignable, Category = "Horloge")
	FOnMinutesChanged OnMinutesChanged;
	
	// Functions

	UFUNCTION(BlueprintCallable, Category = "Horloge")
	void SelectHand(EClockHand Hand);

	UFUNCTION(BlueprintCallable, Category = "Horloge")
	void CycleSelectedHand();

	UFUNCTION(BlueprintCallable, Category = "Horloge")
	void RotateSelectedHand(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Horloge")
	void SyncToPuzzleValues();

	UFUNCTION(BlueprintCallable, Category = "Horloge")
	void PlaySuccessCue();

	UFUNCTION(BlueprintCallable, Category = "Horloge")
	void PlayFailCue();

	void DebugMessage(const FString& Msg, FColor Color);

private:
	
	void PrintCurrentClockTime();
	// Rotation helpers


	float GetContinuousHourRotation() const;
	float GetMinuteRotation() const;

	void StartSmallHandAnimation(float Degrees);
	void StartBigHandAnimation(float Degrees);

	void UpdateSmallHandAnimation(float DeltaTime);
	void UpdateBigHandAnimation(float DeltaTime);
};

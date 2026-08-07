#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraComponent.h"
#include "AHorlogeActor.generated.h"

UENUM(BlueprintType)
enum class EClockHand : uint8
{
    None,
    Small,
    Big
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoursChanged, int32, NewHours);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinutesChanged, int32, NewMinutes);

UCLASS(BlueprintType, Blueprintable)
class MEMORIESFPS_API AHorlogeActor : public AActor
{
    GENERATED_BODY()

public:
    AHorlogeActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    void DebugMessage(const FString& Msg, FColor Color = FColor::White);

    // --- Components ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* ClockFaceMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* SmallHandMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* BigHandMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UPointLightComponent* LightCue;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UAudioComponent* AudioSuccess;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UAudioComponent* AudioFail;

    // --- Focus Camera ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Horloge|Focus")
    UCameraComponent* FocusCamera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Horloge|Focus")
    float FocusBlendTime = 0.5f;

    UPROPERTY(BlueprintReadOnly, Category="Horloge|Focus")
    bool bIsFocused = false;

    // --- Materials ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* DefaultMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* M_OutlineHover;

    // --- Time Values ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Hours;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Minutes;

    // --- Feedback (light/logo colors + sounds) ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Horloge|Feedback")
    UStaticMeshComponent* LogoMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Horloge|Feedback")
    FLinearColor SuccessColor = FLinearColor::Green;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Horloge|Feedback")
    FLinearColor FailColor = FLinearColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Horloge|Feedback")
    FName LogoEmissiveParamName = TEXT("EmissiveColor");

    UPROPERTY()
    UMaterialInstanceDynamic* LogoDynMat;

    // --- Delegates ---
    UPROPERTY(BlueprintAssignable)
    FOnHoursChanged OnHoursChanged;

    UPROPERTY(BlueprintAssignable)
    FOnMinutesChanged OnMinutesChanged;

    // --- Functions ---
    UFUNCTION(BlueprintCallable, Category="Horloge")
    void SelectHand(EClockHand Hand);

    UFUNCTION(BlueprintCallable, Category="Horloge")
    void CycleSelectedHand();

    UFUNCTION(BlueprintCallable, Category="Horloge")
    void RotateSelectedHand(int32 Amount);

    UFUNCTION(BlueprintCallable, Category="Horloge")
    void PlaySuccessCue();

    UFUNCTION(BlueprintCallable, Category="Horloge")
    void PlayFailCue();

    // --- Focus ---
    UFUNCTION(BlueprintCallable, Category="Horloge|Focus")
    void FocusOnHorloge(APlayerController* PC);

    UFUNCTION(BlueprintCallable, Category="Horloge|Focus")
    void UnfocusHorloge(APlayerController* PC);

    // --- Hover Events ---
    UFUNCTION(BlueprintCallable, Category="Horloge|Hover")
    void OnSmallHandHoverBegin(UPrimitiveComponent* TouchedComponent);

    UFUNCTION(BlueprintCallable, Category="Horloge|Hover")
    void OnSmallHandHoverEnd(UPrimitiveComponent* TouchedComponent);

    UFUNCTION(BlueprintCallable, Category="Horloge|Hover")
    void OnBigHandHoverBegin(UPrimitiveComponent* TouchedComponent);

    UFUNCTION(BlueprintCallable, Category="Horloge|Hover")
    void OnBigHandHoverEnd(UPrimitiveComponent* TouchedComponent);

    // --- Click Events ---
    UFUNCTION(BlueprintCallable, Category="Horloge|Click")
    void OnSmallHandClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

    UFUNCTION(BlueprintCallable, Category="Horloge|Click")
    void OnBigHandClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

    EClockHand GetSelectedHand() const { return SelectedHand; }

private:
    EClockHand SelectedHand;
};
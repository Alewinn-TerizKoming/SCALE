// (C) Alewinn / Industries Indigenes

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "ScaleSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS()
class SCALE_API UScaleSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CameraCollision)
	uint32 bMaintainTargetDistance:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CameraCollision, meta=(editcondition="bMaintainTargetDistance"))
	float MinimalTargetDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CameraCollision, meta=(editcondition="bMaintainTargetDistance"))
	float LastFrameDeltaAcceptance;
	
protected:
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
	
	// virtual FVector BlendLocations(const FVector& DesiredArmLocation, const FVector& TraceHitLocation, bool bHitSomething, float DeltaTime) override;
	
private:
	FVector PreviousCamLocation;
	FHitResult LastResult;
};

// (C) Alewinn / Industries Indigenes

#pragma once

#include "CoreMinimal.h"
#include "ScaleBaseCharacter.h"
#include "ScalePlayerCharacter.generated.h"

class USphereComponent;
class UCameraComponent;
class UScaleSpringArmComponent;
/**
 * 
 */
UCLASS()
class SCALE_API AScalePlayerCharacter : public AScaleBaseCharacter
{
	GENERATED_BODY()
	
public:
	AScalePlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);

	void MoveCamX(float Value);
	void MoveCamY(float Value);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UScaleSpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* ViewCamera;

	// UPROPERTY(VisibleAnywhere)
	// USphereComponent* CamCollision;
	
};

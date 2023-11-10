// (C) Alewinn / Industries Indigenes

#pragma once

#include "CoreMinimal.h"
#include "ScaleBaseCharacter.h"
#include "ScalePlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
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
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	
};

// (C) Alewinn / Industries Indigenes

#pragma once

#include "CoreMinimal.h"
#include "ScaleBaseCharacter.h"
#include "ScalePlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SCALE_API AScalePlayerCharacter : public AScaleBaseCharacter
{
	GENERATED_BODY()
	
public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

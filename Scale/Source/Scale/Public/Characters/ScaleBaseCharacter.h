// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ScaleBaseCharacter.generated.h"

UCLASS()
class SCALE_API AScaleBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AScaleBaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	
};

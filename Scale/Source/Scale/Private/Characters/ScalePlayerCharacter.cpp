// (C) Alewinn / Industries Indigenes


#include "Characters/ScalePlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AScalePlayerCharacter::AScalePlayerCharacter()
{
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f,0.f,720.f); 
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;
	
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ViewCamera->SetupAttachment(CameraBoom);
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AScalePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AScalePlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AScalePlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("CamX"), this, &AScalePlayerCharacter::MoveCamX);
	PlayerInputComponent->BindAxis(FName("CamY"), this, &AScalePlayerCharacter::MoveCamY);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
}

void AScalePlayerCharacter::MoveForward(float Value)
{
	if(Controller && (Value != 0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f,ControlRotation.Yaw, 0.f);

		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
	}
}

void AScalePlayerCharacter::MoveRight(float Value)
{
	if(Controller && (Value != 0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f,ControlRotation.Yaw, 0.f);

		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
	}
}

void AScalePlayerCharacter::MoveCamX(float Value)
{
	AddControllerYawInput(Value);
}

void AScalePlayerCharacter::MoveCamY(float Value)
{
	AddControllerPitchInput(Value);
}
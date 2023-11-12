// (C) Alewinn / Industries Indigenes


#include "Gameplay/ScaleSpringArmComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsSettings.h"

void UScaleSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag,
                                                        float DeltaTime)
{

	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
	return;
	// Not used...-------
	
	FRotator DesiredRot = GetTargetRotation();
	
	// Save Previous desired rotation for later bounce camera rotation calculation (won't work with lag)
	FRotator UnchangedPreviousDesiredRot = PreviousDesiredRot;

	// If our viewtarget is simulating using physics, we may need to clamp deltatime
	if (bClampToMaxPhysicsDeltaTime)
	{
		// Use the same max timestep cap as the physics system to avoid camera jitter when the viewtarget simulates less time than the camera
		DeltaTime = FMath::Min(DeltaTime, UPhysicsSettings::Get()->MaxPhysicsDeltaTime);
	}

	// Apply 'lag' to rotation if desired
	if (bDoRotationLag)
	{
		if (bUseCameraLagSubstepping && DeltaTime > CameraLagMaxTimeStep && CameraRotationLagSpeed > 0.f)
		{
			const FRotator ArmRotStep = (DesiredRot - PreviousDesiredRot).GetNormalized() * (1.f / DeltaTime);
			FRotator LerpTarget = PreviousDesiredRot;
			float RemainingTime = DeltaTime;
			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += ArmRotStep * LerpAmount;
				RemainingTime -= LerpAmount;

				DesiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(LerpTarget), LerpAmount,
				                                       CameraRotationLagSpeed));
				PreviousDesiredRot = DesiredRot;
			}
		}
		else
		{
			DesiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(DesiredRot), DeltaTime,
			                                       CameraRotationLagSpeed));
		}
	}
	PreviousDesiredRot = DesiredRot;

	// Get the spring arm 'origin', the target we want to look at
	FVector ArmOrigin = GetComponentLocation() + TargetOffset;
	// We lag the target, not the actual camera position, so rotating the camera around does not have lag
	FVector DesiredLoc = ArmOrigin;
	if (bDoLocationLag)
	{
		if (bUseCameraLagSubstepping && DeltaTime > CameraLagMaxTimeStep && CameraLagSpeed > 0.f)
		{
			const FVector ArmMovementStep = (DesiredLoc - PreviousDesiredLoc) * (1.f / DeltaTime);
			FVector LerpTarget = PreviousDesiredLoc;

			float RemainingTime = DeltaTime;
			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += ArmMovementStep * LerpAmount;
				RemainingTime -= LerpAmount;

				DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, LerpTarget, LerpAmount, CameraLagSpeed);
				PreviousDesiredLoc = DesiredLoc;
			}
		}
		else
		{
			DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, DesiredLoc, DeltaTime, CameraLagSpeed);
		}

		// Clamp distance if requested
		bool bClampedDist = false;
		if (CameraLagMaxDistance > 0.f)
		{
			const FVector FromOrigin = DesiredLoc - ArmOrigin;
			if (FromOrigin.SizeSquared() > FMath::Square(CameraLagMaxDistance))
			{
				DesiredLoc = ArmOrigin + FromOrigin.GetClampedToMaxSize(CameraLagMaxDistance);
				bClampedDist = true;
			}
		}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (bDrawDebugLagMarkers)
		{
			DrawDebugSphere(GetWorld(), ArmOrigin, 5.f, 8, FColor::Green);
			DrawDebugSphere(GetWorld(), DesiredLoc, 5.f, 8, FColor::Yellow);

			const FVector ToOrigin = ArmOrigin - DesiredLoc;
			DrawDebugDirectionalArrow(GetWorld(), DesiredLoc, DesiredLoc + ToOrigin * 0.5f, 7.5f,
			                          bClampedDist ? FColor::Red : FColor::Green);
			DrawDebugDirectionalArrow(GetWorld(), DesiredLoc + ToOrigin * 0.5f, ArmOrigin, 7.5f,
			                          bClampedDist ? FColor::Red : FColor::Green);
		}
#endif
	}

	PreviousArmOrigin = ArmOrigin;
	PreviousDesiredLoc = DesiredLoc;

	// Now offset camera position back along our rotation
	DesiredLoc -= DesiredRot.Vector() * TargetArmLength;
	// Add socket offset in local space
	DesiredLoc += FRotationMatrix(DesiredRot).TransformVector(SocketOffset);

	// Do a sweep to ensure we are not penetrating the world
	FVector ResultLoc;
	if (bDoTrace && (TargetArmLength != 0.0f))
	{
		bIsCameraFixed = true;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetOwner());

		FHitResult Result;
		GetWorld()->SweepSingleByChannel(Result, ArmOrigin, DesiredLoc, FQuat::Identity, ProbeChannel,
		                                 FCollisionShape::MakeSphere(ProbeSize), QueryParams);
		UnfixedCameraPosition = DesiredLoc;
		
		ResultLoc = BlendLocations(DesiredLoc, Result.Location, Result.bBlockingHit, DeltaTime);
		
		if (ResultLoc == DesiredLoc)
		{
			bIsCameraFixed = false;
		}

		// If we hit something, calculate the symmetrically opposed position of the camera based on previous frame camera position
		if (Result.bBlockingHit && ( FVector::Dist(Result.Location, LastResult.Location) > LastFrameDeltaAcceptance  ) )
		{
			FHitResult VisibilityTestHit;
			FVector LastBlockingLocation = ResultLoc;
			FVector TestedNewLocation = 2 * PreviousCamLocation - UnfixedCameraPosition;

			// We keep this position only if there are no new obstacles
			GetWorld()->SweepSingleByChannel(VisibilityTestHit, ArmOrigin, TestedNewLocation, FQuat::Identity, ProbeChannel, FCollisionShape::MakeSphere(ProbeSize), QueryParams);
			if (!VisibilityTestHit.bBlockingHit)
			{
				ResultLoc = TestedNewLocation;
				// if we keep the new location, we have to recalculate camera rotation toward the SpringArm
				// DesiredRot = FRotator(FMath::QInterpTo(FQuat(UnchangedPreviousDesiredRot), FQuat(GetTargetRotation()), DeltaTime, CameraRotationLagSpeed));
				
				DesiredRot = UKismetMathLibrary::FindLookAtRotation(TestedNewLocation, ArmOrigin);
				PreviousDesiredRot = DesiredRot;
			}
			else
			{
				ResultLoc = LastBlockingLocation;
			}
		}
		LastResult = Result;
	}
	else
	{
		ResultLoc = DesiredLoc;
		bIsCameraFixed = false;
		UnfixedCameraPosition = ResultLoc;
	}

	// Form a transform for new world transform for camera
	FTransform WorldCamTM(DesiredRot, ResultLoc);
	// Convert to relative to component
	FTransform RelCamTM = WorldCamTM.GetRelativeTransform(GetComponentTransform());

	// Update socket location/rotation
	RelativeSocketLocation = RelCamTM.GetLocation();
	RelativeSocketRotation = RelCamTM.GetRotation();

	// Save current camera position for next frame camera bouncing position
	PreviousCamLocation = ResultLoc;
	
	UpdateChildTransforms();
	
}

// FVector UScaleSpringArmComponent::BlendLocations(const FVector& DesiredArmLocation, const FVector& TraceHitLocation,
//                                                  bool bHitSomething, float DeltaTime)
// {
// 	if (bHitSomething)
// 	{
// 		FVector TestedNewLocation = 2*PreviousLocation - GetUnfixedCameraPosition(); 
//
// 		
// #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
// 		DrawDebugSphere(GetWorld(), TestedNewLocation , 5.f, 8, FColor::Green, true, 1.f);
// #endif
//
// 		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetOwner());
// 		FHitResult Result;
// 		GetWorld()->SweepSingleByChannel(Result, PreviousArmOrigin, TestedNewLocation, FQuat::Identity, ProbeChannel, FCollisionShape::MakeSphere(ProbeSize), QueryParams);
//
// 		if(!Result.bBlockingHit)
// 		{
// 			PreviousLocation =  TestedNewLocation;
// 			return PreviousLocation;			
// 		}
// 			
// 	}
//
// 	PreviousLocation = Super::BlendLocations(DesiredArmLocation, TraceHitLocation, bHitSomething, DeltaTime);
// 	return PreviousLocation; 
// }

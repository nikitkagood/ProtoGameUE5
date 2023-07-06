// Nikita Belov, All rights reserved


#include "Character/Components/CustomCharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PhysicsVolume.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
{
	MaxWalkSpeed = 500.f; //When jogging
	MaxWalkSpeedSlowWalk = 250.f;
	MaxWalkSpeedSprint = 1000.f;
	MaxWalkSpeedProne = 120.f;

	bCanProne = true;
	bCanSprint = true;
	bCanSlowWalk = true;

	ProneHalfHeight = 12.f;
	bProneMaintainsBaseLocation = true;
}

TObjectPtr<AGameCharacterBase> UCustomCharacterMovementComponent::GetCharacterOwner() const
{
	return Cast<AGameCharacterBase>(CharacterOwner);
}

float UCustomCharacterMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		if (IsSlowWalking())
		{
			return MaxWalkSpeedSlowWalk;
		}
		if (IsSprinting())
		{
			return MaxWalkSpeedSprint;
		}
		if (IsProne())
		{
			return MaxWalkSpeedProne;
		}
		if (IsCrouching())
		{
			return MaxWalkSpeedCrouched;
		}

		return MaxWalkSpeed; //Jog
	case MOVE_Falling:
		return MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

void UCustomCharacterMovementComponent::StartSlowWalk()
{
	if (!HasValidData())
	{
		return;
	}

	if (!CanSlowWalkInCurrentState())
	{
		return;
	}

	GetCharacterOwner()->bIsSlowWalking = true;
}

void UCustomCharacterMovementComponent::EndSlowWalk()
{
	if (!HasValidData())
	{
		return;
	}

	GetCharacterOwner()->bIsSlowWalking = false;
}

void UCustomCharacterMovementComponent::StartSprint()
{
	if (!HasValidData())
	{
		return;
	}

	if (!CanSprintInCurrentState())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("UCustomCharacterMovementComponent::StartSprint"));
	GetCharacterOwner()->bIsSprinting = true;
}

void UCustomCharacterMovementComponent::EndSprint()
{
	if (!HasValidData())
	{
		return;
	}

	GetCharacterOwner()->bIsSprinting = false;
}

void UCustomCharacterMovementComponent::Prone(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!bClientSimulation && !CanProneInCurrentState())
	{
		return;
	}

	auto CharacterOwnerCustom = GetCharacterOwner();

	// See if collision is already at desired size.
	if (CharacterOwnerCustom->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == ProneHalfHeight)
	{
		if (!bClientSimulation)
		{
			CharacterOwnerCustom->bIsProne = true;
			CharacterOwnerCustom->bIsCrouched = false;
		}
		CharacterOwnerCustom->OnStartProne(0.f, 0.f);
		return;
	}

	if (bClientSimulation && CharacterOwnerCustom->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// restore collision size before crouching
		AGameCharacterBase* DefaultCharacter = CharacterOwnerCustom->GetClass()->GetDefaultObject<AGameCharacterBase>();
		CharacterOwnerCustom->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
		bShrinkProxyCapsule = true;
	}

	// Change collision size to prone dimensions
	const float ComponentScale = CharacterOwnerCustom->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwnerCustom->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwnerCustom->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// Height is not allowed to be smaller than radius.
	const float ClampedProneHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, ProneHalfHeight);

	CharacterOwnerCustom->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedProneHalfHeight);

	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedProneHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (!bClientSimulation)
	{
		// Prone to a larger height? (this is rare)
		if (ClampedProneHalfHeight > OldUnscaledHalfHeight)
		{
			FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwnerCustom);
			FCollisionResponseParams ResponseParam;
			InitCollisionParams(CapsuleParams, ResponseParam);
			const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
				UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

			// If encroached, cancel
			if (bEncroached)
			{
				CharacterOwnerCustom->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);

				return;
			}
		}

		if (bProneMaintainsBaseLocation)
		{
			// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
			UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		}

		CharacterOwnerCustom->bIsProne = true;
		CharacterOwnerCustom->bIsCrouched = false;
	}

	bForceNextFloorCheck = true;

	// OnStartProne takes the change from the Default size, not the current one (though they are usually the same).
	const float MeshAdjust = ScaledHalfHeightAdjust;
	AGameCharacterBase* DefaultCharacter = CharacterOwnerCustom->GetClass()->GetDefaultObject<AGameCharacterBase>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ClampedProneHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	CharacterOwnerCustom->OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Don't smooth this change in mesh position
	if ((bClientSimulation && CharacterOwnerCustom->GetLocalRole() == ROLE_SimulatedProxy) || (IsNetMode(NM_ListenServer) && CharacterOwnerCustom->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			ClientData->MeshTranslationOffset -= FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

void UCustomCharacterMovementComponent::UnProne(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	auto CharacterOwnerCustom = GetCharacterOwner();

	AGameCharacterBase* DefaultCharacter = CharacterOwnerCustom->GetClass()->GetDefaultObject<AGameCharacterBase>();

	// See if collision is already at desired size.
	if (CharacterOwnerCustom->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight())
	{
		if (!bClientSimulation)
		{
			CharacterOwnerCustom->bIsProne = false;
		}
		CharacterOwnerCustom->OnEndProne(0.f, 0.f);
		return;
	}

	const float CurrentProneHalfHeight = CharacterOwnerCustom->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwnerCustom->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwnerCustom->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// Grow to uncrouched size.
	check(CharacterOwnerCustom->GetCapsuleComponent());

	if (!bClientSimulation)
	{
		// Try to stay in place and see if the larger capsule fits. We use a slightly taller capsule to avoid penetration.
		const UWorld* MyWorld = GetWorld();
		const float SweepInflation = UE_KINDA_SMALL_NUMBER * 10.f;
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwnerCustom);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);

		// Compensate for the difference between current capsule size and standing size
		const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
		bool bEncroached = true;

		if (!bCrouchMaintainsBaseLocation)
		{
			// Expand in place
			bEncroached = MyWorld->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				// Try adjusting capsule position to see if we can avoid encroachment.
				if (ScaledHalfHeightAdjust > 0.f)
				{
					// Shrink to a short capsule, sweep down to base to find where that would hit something, and then try to stand up from there.
					float PawnRadius, PawnHalfHeight;
					CharacterOwnerCustom->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
					const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
					const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
					const FVector Down = FVector(0.f, 0.f, -TraceDist);

					FHitResult Hit(1.f);
					const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
					const bool bBlockingHit = MyWorld->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
					if (Hit.bStartPenetrating)
					{
						bEncroached = true;
					}
					else
					{
						// Compute where the base of the sweep ended up, and see if we can stand there
						const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
						const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + StandingCapsuleShape.Capsule.HalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
						bEncroached = MyWorld->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
						if (!bEncroached)
						{
							// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
							UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
						}
					}
				}
			}
		}
		else
		{
			// Expand while keeping base location the same.
			FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentProneHalfHeight);
			bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				if (IsMovingOnGround())
				{
			
					//Something might be just barely overhead, try moving down closer to the floor to avoid it.
					const float MinFloorDist = UE_KINDA_SMALL_NUMBER * 10.f;
					if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
					{
						StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
						bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					}
				}
			}

			if (!bEncroached)
			{
				// Commit the change in location.
				UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				bForceNextFloorCheck = true;
			}
		}

		// If still encroached then abort.
		if (bEncroached)
		{
			return;
		}

		CharacterOwnerCustom->bIsProne = false;
	}
	else
	{
		bShrinkProxyCapsule = true;
	}

	// Now call SetCapsuleSize() to cause touch/untouch events and actually grow the capsule
	CharacterOwnerCustom->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();
	CharacterOwnerCustom->OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Don't smooth this change in mesh position
	if ((bClientSimulation && CharacterOwnerCustom->GetLocalRole() == ROLE_SimulatedProxy) || (IsNetMode(NM_ListenServer) && CharacterOwnerCustom->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			ClientData->MeshTranslationOffset += FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

void UCustomCharacterMovementComponent::Crouch(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!bClientSimulation && !CanCrouchInCurrentState())
	{
		return;
	}

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == GetCrouchedHalfHeight())
	{
		if (!bClientSimulation)
		{
			GetCharacterOwner()->bIsCrouched = true;
			GetCharacterOwner()->bIsProne = false;
		}
		CharacterOwner->OnStartCrouch(0.f, 0.f);
		return;
	}

	if (bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// restore collision size before crouching
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
		bShrinkProxyCapsule = true;
	}

	// Change collision size to crouching dimensions
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// Height is not allowed to be smaller than radius.
	const float ClampedCrouchedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, GetCrouchedHalfHeight());

	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedCrouchedHalfHeight);

	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedCrouchedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (!bClientSimulation)
	{
		// Crouching to a larger height. Happens when crouching from prone.
		if (ClampedCrouchedHalfHeight > OldUnscaledHalfHeight)
		{
			FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
			FCollisionResponseParams ResponseParam;
			InitCollisionParams(CapsuleParams, ResponseParam);
			const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
				UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

			// If encroached, cancel
			if (bEncroached)
			{
				CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);

				return;
			}
		}

		if (bCrouchMaintainsBaseLocation)
		{
			// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
			UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		}

		GetCharacterOwner()->bIsCrouched = true;
		GetCharacterOwner()->bIsProne = false;
	}

	bForceNextFloorCheck = true;

	// OnStartCrouch takes the change from the Default size, not the current one (though they are usually the same).
	const float MeshAdjust = ScaledHalfHeightAdjust;
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ClampedCrouchedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	CharacterOwner->OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Don't smooth this change in mesh position
	if ((bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy) || (IsNetMode(NM_ListenServer) && CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			ClientData->MeshTranslationOffset -= FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

void UCustomCharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight())
	{
		if (!bClientSimulation)
		{
			CharacterOwner->bIsCrouched = false;
		}
		CharacterOwner->OnEndCrouch(0.f, 0.f);
		return;
	}

	const float CurrentCrouchedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// Grow to uncrouched size.
	check(CharacterOwner->GetCapsuleComponent());

	if (!bClientSimulation)
	{
		// Try to stay in place and see if the larger capsule fits. We use a slightly taller capsule to avoid penetration.
		const UWorld* MyWorld = GetWorld();
		const float SweepInflation = UE_KINDA_SMALL_NUMBER * 10.f;
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);

		// Compensate for the difference between current capsule size and standing size
		const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
		bool bEncroached = true; //whether the capsule is shrunk

		if (!bCrouchMaintainsBaseLocation)
		{
			// Expand in place
			bEncroached = MyWorld->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				// Try adjusting capsule position to see if we can avoid encroachment.
				if (ScaledHalfHeightAdjust > 0.f)
				{
					// Shrink to a short capsule, sweep down to base to find where that would hit something, and then try to stand up from there.
					float PawnRadius, PawnHalfHeight;
					CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
					const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
					const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
					const FVector Down = FVector(0.f, 0.f, -TraceDist);

					FHitResult Hit(1.f);
					const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
					const bool bBlockingHit = MyWorld->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
					if (Hit.bStartPenetrating)
					{
						bEncroached = true;
					}
					else
					{
						// Compute where the base of the sweep ended up, and see if we can stand there
						const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
						const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + StandingCapsuleShape.Capsule.HalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
						bEncroached = MyWorld->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
						if (!bEncroached)
						{
							// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
							UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
						}
					}
				}
			}
		}
		else
		{
			// Expand while keeping base location the same.
			FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrouchedHalfHeight);
			bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				if (IsMovingOnGround())
				{
					//Something might be just barely overhead, try moving down closer to the floor to avoid it.
					const float MinFloorDist = UE_KINDA_SMALL_NUMBER * 10.f;
					if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
					{
						StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
						bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					}
				}
			}

			if (!bEncroached)
			{
				// Commit the change in location.
				UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				bForceNextFloorCheck = true;
			}
		}

		// If still encroached then abort.
		if (bEncroached)
		{
			return;
		}

		CharacterOwner->bIsCrouched = false;
	}
	else
	{
		bShrinkProxyCapsule = true;
	}

	// Now call SetCapsuleSize() to cause touch/untouch events and actually grow the capsule
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();
	CharacterOwner->OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Don't smooth this change in mesh position
	if ((bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy) || (IsNetMode(NM_ListenServer) && CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			ClientData->MeshTranslationOffset += FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

bool UCustomCharacterMovementComponent::CanSlowWalkInCurrentState() const
{
	if (!CanEverSlowWalk())
	{
		return false;
	}

	return (IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

bool UCustomCharacterMovementComponent::CanSprintInCurrentState() const
{
	if (!CanEverSprint())
	{
		return false;
	}

	return (IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

bool UCustomCharacterMovementComponent::CanProneInCurrentState() const
{
	if (!CanEverProne())
	{
		return false;
	}

	return (!IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

bool UCustomCharacterMovementComponent::CanCrouchInCurrentState() const
{
	if (!CanEverCrouch())
	{
		return false;
	}

	return (!IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

//void UCustomCharacterMovementComponent::SetProneHalfHeight(const float NewValue)
//{
//	ProneHalfHeight = NewValue;
//
//	if (GetCharacterOwner() != nullptr)
//	{
//		GetCharacterOwner()->RecalculateProneEyeHeight();
//	}
//}

bool UCustomCharacterMovementComponent::IsSlowWalking() const
{
	return GetCharacterOwner() && GetCharacterOwner()->bIsSlowWalking;
}

bool UCustomCharacterMovementComponent::IsSprinting() const
{
	return GetCharacterOwner() && GetCharacterOwner()->bIsSprinting;
}

bool UCustomCharacterMovementComponent::IsProne() const
{
	return GetCharacterOwner() && GetCharacterOwner()->bIsProne;
}

void UCustomCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Proxies get replicated crouch state.
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		if (!IsCrouching() && bWantsToCrouch && CanCrouchInCurrentState())
		{
			Crouch(false);
		}
		else if (!IsProne() && bWantsToProne && CanProneInCurrentState())
		{
			Prone(false);
		}		
		// Check for a change in crouch state. Players toggle crouch by changing bWantsToCrouch.
		else if (IsCrouching() && (!bWantsToCrouch || !CanCrouchInCurrentState()))
		{
			UnCrouch(false);
			//If we can't UnCrouch, we reset player input, so the character doesn't stand up automatically
			if (IsCrouching() && !bWantsToProne)
			{
				bWantsToCrouch = true;
			}
		}
		else if (IsProne() && (!bWantsToProne || !CanProneInCurrentState()))
		{
			UnProne(false);
			//If we can't UnProne, we reset player input, so the character doesn't stand up automatically
			if (IsProne() && !bWantsToCrouch)
			{
				bWantsToProne = true;
			}
		}

	}
}

void UCustomCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	// Proxies get replicated crouch state.
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		// Uncrouch if no longer allowed to be crouched
		if (IsCrouching() && !CanCrouchInCurrentState())
		{
			UnCrouch(false);
		}

		// Unprone if no longer allowed to be prone
		if (IsProne() && !CanProneInCurrentState())
		{
			UnProne(false);
		}
	}
}

//void UCustomCharacterMovementComponent::FillAsyncInput(const FVector& InputVector, FCharacterMovementComponentAsyncInput& AsyncInput)
//{
//	if (!CharacterOwner || !CharacterOwner->Controller)
//	{
//		return;
//	}
//
//	UCharacterMovementComponent::FillAsyncInput(InputVector, AsyncInput);
//
//	FCustomCharacterMovementComponentAsyncInput* CustomAsyncInput = Cast<FCustomCharacterMovementComponentAsyncInput>(&AsyncInput);
//
//	CustomAsyncInput->WalkSpeedWalk = WalkSpeedWalk;
//	CustomAsyncInput->WalkSpeedJog = WalkSpeedWalk;
//	CustomAsyncInput->WalkSpeedSprint = WalkSpeedWalk;
//	CustomAsyncInput->WalkSpeedProne = WalkSpeedWalk;
//
//}
//
//void UCustomCharacterMovementComponent::BuildAsyncInput()
//{
//	if (CharacterMovementCVars::AsyncCharacterMovement == 1 && IsAsyncCallbackRegistered())
//	{
//		FCharacterMovementComponentAsyncInput* Input = AsyncCallback->GetProducerInputData_External();
//		if (Input->bInitialized == false)
//		{
//			Input->Initialize<FCharacterMovementComponentAsyncInput::FCharacterInput, FCharacterMovementComponentAsyncInput::FUpdatedComponentInput>();
//		}
//
//		if (AsyncSimState.IsValid() == false)
//		{
//			AsyncSimState = MakeShared<FCharacterMovementComponentAsyncOutput, ESPMode::ThreadSafe>();
//		}
//		Input->AsyncSimState = AsyncSimState;
//
//		const FVector InputVector = ConsumeInputVector();
//		FillAsyncInput(InputVector, *Input);
//
//		PostBuildAsyncInput();
//	}
//}

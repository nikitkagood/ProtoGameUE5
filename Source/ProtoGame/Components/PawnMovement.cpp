// Nikita Belov, All rights reserved


#include "Components/PawnMovement.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PhysicsVolume.h"

DEFINE_LOG_CATEGORY(LogPawnMovement);

void UPawnMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	DrawDebugDirectionalArrow(GetWorld(), UpdatedComponent->GetComponentLocation(), Velocity.IsNearlyZero() ? UpdatedComponent->GetComponentLocation() 
		: UpdatedComponent->GetComponentLocation() + (Velocity.Length() * Velocity.GetSafeNormal()), 20, FColor::Cyan, false, 0.15f, 1, 3);

	DrawDebugDirectionalArrow(GetWorld(), UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * 25, RequestedVelocity.IsNearlyZero() ? UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * 25
		: UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * 25 + (RequestedVelocity.Length() * RequestedVelocity.GetSafeNormal()), 15, FColor::Emerald, false, 0.15f, 2, 3);

	FColor DebugMovementModeColor;
	
	switch (MovementMode)
	{
	case PawnMovementMode::None:
		DebugMovementModeColor = FColor::FromHex("#b3b3b3"); //Grey
		break;
	case PawnMovementMode::Falling:
		DebugMovementModeColor = FColor::FromHex("#c9383b"); //Red-ish
		break;
	case PawnMovementMode::MovingOnGround:
		DebugMovementModeColor = FColor::Green;
		break;
	case PawnMovementMode::Swimming:
	case PawnMovementMode::Flying:
	case PawnMovementMode::Custom:
	default:
		DebugMovementModeColor = FColor::Black;
		break;
	}

	DrawDebugString(GetWorld(), UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * 65, UEnum::GetValueAsString(MovementMode), nullptr, DebugMovementModeColor, 0, true, 1);



	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}

	const FVector OldVelocity = Velocity;
	//const FVector OldLocation = GetActorLocation();

	//Whether we apply any movement and respective forces at all
	//Doesn't affect gravity because it's an external force pretty much
	bool ShouldApplyMovement = true;

	bool AirControl = false; //hardcoded for now, we don't allow Pawn to move itself mid-air

	//No movement if we can't move, or if currently doing physical simulation on UpdatedComponent
	if (MovementMode == PawnMovementMode::None || MovementMode == PawnMovementMode::Falling
		|| UpdatedComponent->Mobility != EComponentMobility::Movable || UpdatedComponent->IsSimulatingPhysics())
	{
		ShouldApplyMovement = false;
	}
	
	//Clear input and requested velocity since it's irrelevant now
	if (ShouldApplyMovement == false)
	{
		ConsumeInputVector();
		RequestedVelocity = FVector::ZeroVector;
	}

	//TODO:
	// Force floor update if we've moved outside of CharacterMovement since last update.
	//if(OldLocation)
	//IsMovingOnGround();
	//Else we start to apply requested movement (i.e. on the ground movement for now)

	
	if (MovementMode != PawnMovementMode::Flying && MovementMode != PawnMovementMode::Swimming)
	{
		//Check floor results
		FTraceDatum TraceDatum;

		if (GetWorld()->QueryTraceData(TraceHandlePersistent, TraceDatum) == true)
		{
			//Distance to the ground can be tricky
			if (TraceDatum.OutHits.IsEmpty() == false && FMath::IsNearlyZero(TraceDatum.OutHits[0].Distance, SweepDistanceFloorTolerance))
			{
				MovementMode = PawnMovementMode::MovingOnGround;
			}
			else
			{
				MovementMode = PawnMovementMode::Falling;
			}
		}

		//Try to find floor
		SweepFloorAsync(FTraceDelegate::CreateUObject(this, &UPawnMovement::AsyncTraceDoneCallback));

	}

	//Accelerate. Only if there is a sufficient requested speed
	const float RequestedSpeedSquared = RequestedVelocity.SizeSquared();

	if (ShouldApplyMovement == true && RequestedSpeedSquared > UE_KINDA_SMALL_NUMBER)
	{
		float RequestedSpeed = FMath::Sqrt(RequestedSpeedSquared);

		if (ShouldComputeAccelerationToReachRequestedVelocity(RequestedSpeed))
		{
			const float NewMaxSpeed = (IsExceedingMaxSpeed(GetMaxSpeed())) ? Velocity.Size() : GetMaxSpeed();
			Velocity += (RequestedVelocity.GetSafeNormal() * AccelerationV2 * DeltaTime);
			Velocity = Velocity.GetClampedToMaxSize(NewMaxSpeed);
		}
		else
		{
			// Just set velocity directly.
			// If decelerating we do so instantly, so we don't slide through the destination if we can't brake fast enough.
			Velocity = RequestedVelocity;
		}
	}

	//Decelerate. Only if we don't stand still already and there is no acceleration
	if (ShouldApplyMovement == true && Velocity.SizeSquared() > 0.f && (RequestedSpeedSquared > UE_KINDA_SMALL_NUMBER) == false)
	{
		//const FVector OldVelocity = Velocity;
		const float VelSize = FMath::Max(Velocity.Size() - FMath::Abs(DecelerationV2) * DeltaTime, 0.f);
		Velocity = Velocity.GetSafeNormal() * VelSize;

		// Don't allow braking to lower us below max speed if we started above it.
		//if (IsExceedingMaxSpeed(GetMaxSpeed()) && Velocity.SizeSquared() < FMath::Square(MaxPawnSpeed))
		//{
		//	Velocity = OldVelocity.GetSafeNormal() * MaxPawnSpeed;
		//}
	}

	RequestedVelocity = FVector::ZeroVector;

	if (MovementMode == PawnMovementMode::Falling)
	{
		//Apply gravity
		Velocity = NewFallVelocity(Velocity, GravityVector * -GetGravityZ(), DeltaTime);
	}

	//Actually move things
	const AController* Controller = PawnOwner->GetController();
	if (Controller && Controller->IsLocalController())
	{
		// apply input for local players but also for AI that's not following a navigation path at the moment
		//if (Controller->IsLocalPlayerController() == true || Controller->IsFollowingAPath() == false || bUseAccelerationForPaths)
		//{
		//	ApplyControlInputToVelocity(DeltaTime);
		//}
		// if it's not player controller, but we do have a controller, then it's AI
		// (that's not following a path) and we need to limit the speed
		//else 
		if (IsExceedingMaxSpeed(GetMaxSpeed()) == true)
		{
			Velocity = Velocity.GetUnsafeNormal() * GetMaxSpeed();
		}

		LimitWorldBounds();
		bPositionCorrected = false;

		// Move actor
		FVector VelocityDelta = Velocity * DeltaTime;

		if (!VelocityDelta.IsNearlyZero(1e-6f))
		{
			const FVector OldLocation = UpdatedComponent->GetComponentLocation();
			const FQuat Rotation = UpdatedComponent->GetComponentQuat();

			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(VelocityDelta, Rotation, true, Hit);

			if (Hit.IsValidBlockingHit())
			{
				HandleImpact(Hit, DeltaTime, VelocityDelta);

				//Try to slide the remaining distance along the surface
				//Only if controlled by AI or a player
				if (true /*Controller->IsFollowingAPath()*/)
				{
					SlideAlongSurface(VelocityDelta, 1.f - Hit.Time, Hit.Normal, Hit, true);
					//MoveAlongFloor(VelocityDelta, DeltaTime);
				};
			}

			// Update velocity
			// We don't want position changes to vastly reverse our direction (which can happen due to penetration fixups etc)
			if (!bPositionCorrected)
			{
				const FVector NewLocation = UpdatedComponent->GetComponentLocation();
				Velocity = ((NewLocation - OldLocation) / DeltaTime);
			}
			else if(GetOwner() != nullptr)
			{
				UE_LOG(LogPawnMovement, Warning, TEXT("Owner %s, PawnMovement: position corrected"), *GetOwner()->GetActorNameOrLabel())
			}
		}

		// Finalize
		UpdateComponentVelocity();
	}
}

UPawnMovement::UPawnMovement()
{
	SetComponentTickInterval(1.f / 30.f);
	SetAsyncPhysicsTickEnabled(true);

	//default values
	NavAgentProps.bCanWalk = true;
	NavAgentProps.bCanCrouch = false;
	NavAgentProps.bCanFly = false;
	NavAgentProps.bCanJump = false;
	NavAgentProps.bCanSwim = false;
}

void UPawnMovement::ApplyControlInputToVelocity(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("ApplyControlInputToVelocity: velocity: %s"), *Velocity.ToString())

	FVector ControlAcceleration = GetPendingInputVector().GetClampedToMaxSize(1.f);

	const float AnalogInputModifier = (ControlAcceleration.SizeSquared() > 0.f ? ControlAcceleration.Size() : 0.f);
	const float MaxPawnSpeed = GetMaxSpeed() * AnalogInputModifier;
	const bool bExceedingMaxSpeed = IsExceedingMaxSpeed(MaxPawnSpeed);

	if (AnalogInputModifier > 0.f && !bExceedingMaxSpeed)
	{
		// Apply change in velocity direction
		if (Velocity.SizeSquared() > 0.f)
		{
			// Change direction faster than only using acceleration, but never increase velocity magnitude.
			const float TimeScale = FMath::Clamp(DeltaTime * TurningBoost, 0.f, 1.f);
			Velocity = Velocity + (ControlAcceleration * Velocity.Size() - Velocity) * TimeScale;
		}
	}
	else
	{
		// Dampen velocity magnitude based on deceleration.
		if (Velocity.SizeSquared() > 0.f)
		{
			const FVector OldVelocity = Velocity;
			const float VelSize = FMath::Max(Velocity.Size() - FMath::Abs(Deceleration) * DeltaTime, 0.f);
			Velocity = Velocity.GetSafeNormal() * VelSize;

			// Don't allow braking to lower us below max speed if we started above it.
			if (bExceedingMaxSpeed && Velocity.SizeSquared() < FMath::Square(MaxPawnSpeed))
			{
				Velocity = OldVelocity.GetSafeNormal() * MaxPawnSpeed;
			}
		}
	}

	// Apply acceleration and clamp velocity magnitude.
	const float NewMaxSpeed = (IsExceedingMaxSpeed(MaxPawnSpeed)) ? Velocity.Size() : MaxPawnSpeed;
	Velocity += ControlAcceleration * FMath::Abs(Acceleration) * DeltaTime;
	Velocity = Velocity.GetClampedToMaxSize(NewMaxSpeed);

	ConsumeInputVector();




	//bool bShouldFindFloor = Velocity.Z <= 0.f;

	//if (HasZVelocity)
	//{
	//	//Just apply gravity
	//	Velocity += GravityVector * GravitationalAccelerationConstant * DeltaTime;
	//}
	//else
	//{
	//	Velocity += (ControlAcceleration * FMath::Abs(Acceleration) + (GravityVector * GravitationalAccelerationConstant)) * DeltaTime;
	//}

	//PawnOwner;
	//Cast<UCapsuleComponent>(UpdatedComponent);

	//Velocity += (ControlAcceleration * FMath::Abs(Acceleration) + (GravityVector * GravitationalAccelerationConstant)) * DeltaTime;

	//if (bShouldFindFloor)
	//{
	//	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, Velocity.IsZero(), NULL);
	//}
	//else
	//{
	//	CurrentFloor.Clear();
	//}

	//Velocity.X += ControlAcceleration.X * FMath::Abs(Acceleration) * DeltaTime;
	//Velocity.Y += ControlAcceleration.Y * FMath::Abs(Acceleration) * DeltaTime;
	//Velocity.Z += -GravitationalAccelerationConstant * DeltaTime;

	//How it was originally done
	//Velocity = Velocity.GetClampedToMaxSize(NewMaxSpeed);

	//Velocity.X = FMath::Clamp(Velocity.X, -NewMaxSpeed, NewMaxSpeed);
	//Velocity.Y = FMath::Clamp(Velocity.Y, -NewMaxSpeed, NewMaxSpeed);

	//constexpr double MaxZVelocity = 100 * 100;
	//Velocity.Z = FMath::Clamp(Velocity.Z, -MaxZVelocity, MaxZVelocity);

	//ConsumeInputVector();
}

bool UPawnMovement::IsMovingOnGround() const
{
	return (MovementMode == PawnMovementMode::MovingOnGround) && UpdatedComponent;
}

bool UPawnMovement::IsFalling() const
{
	return (MovementMode == PawnMovementMode::Falling) && UpdatedComponent;
}

bool UPawnMovement::IsFlying() const
{
	return (MovementMode == PawnMovementMode::Flying) && UpdatedComponent;
}

bool UPawnMovement::IsSwimming() const
{
	return (MovementMode == PawnMovementMode::Swimming) && UpdatedComponent;
}

bool UPawnMovement::IsWithinEdgeTolerance(const FVector& CapsuleLocation, const FVector& TestImpactPoint, const float CapsuleRadius) const
{
	const float DistFromCenterSq = (TestImpactPoint - CapsuleLocation).SizeSquared2D();
	const float ReducedRadiusSq = FMath::Square(FMath::Max(UPawnMovement::SWEEP_EDGE_REJECT_DISTANCE + UE_KINDA_SMALL_NUMBER, CapsuleRadius - UPawnMovement::SWEEP_EDGE_REJECT_DISTANCE));
	return DistFromCenterSq < ReducedRadiusSq;
}

float UPawnMovement::GetBrakingDeceleration() const
{
	switch (MovementMode)
	{
	case PawnMovementMode::Falling:
		return 0.f;
		break;
	case PawnMovementMode::MovingOnGround:
		return DecelerationV2;
		break;
	case PawnMovementMode::Swimming:
		return DecelerationV2 * 1.5f; //TODO: value is out of thin air, test
	case PawnMovementMode::Flying:
		return DecelerationV2 / 1.5f; //TODO: value is out of thin air, test
	case PawnMovementMode::Custom:
		return 0.f;

	case PawnMovementMode::None:
	default:
		return 0.f;
		break;
	}
}

void UPawnMovement::SetMovementMode(PawnMovementMode movement_mode)
{
	MovementMode = movement_mode;
}

FHitResult UPawnMovement::SweepFloorSync(ECollisionChannel collision_channel, const FCollisionShape& collision_shape)
{
	FVector Origin = UpdatedComponent->Bounds.Origin;
	FVector BoxExtent = UpdatedComponent->Bounds.BoxExtent;

	float CollisionShapeExtentZ = collision_shape.GetExtent().Z;
	FVector Start = Origin + (BoxExtent.Z - CollisionShapeExtentZ) * FVector::DownVector;
	FVector End = Start + FVector::DownVector * MAX_FLOOR_DIST;

	FCollisionQueryParams query_params;
	query_params.AddIgnoredActor(GetOwner());
	query_params.MobilityType = EQueryMobilityType::Any;

	FHitResult HitResult;

	GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, collision_channel, collision_shape, query_params);

	return HitResult;
}

void UPawnMovement::SweepFloorAsync(FTraceDelegate callback, ECollisionChannel collision_channel, const FCollisionShape& collision_shape)
{
	FVector Origin = UpdatedComponent->Bounds.Origin;
	FVector BoxExtent = UpdatedComponent->Bounds.BoxExtent;

	float CollisionShapeExtentZ = collision_shape.GetExtent().Z;
	FVector Start = Origin + (BoxExtent.Z - CollisionShapeExtentZ) * FVector::DownVector;
	FVector End = Start + FVector::DownVector * MAX_FLOOR_DIST;

	FCollisionQueryParams query_params;
	query_params.AddIgnoredActor(GetOwner());
	query_params.AddIgnoredComponent(UpdatedPrimitive);
	query_params.MobilityType = EQueryMobilityType::Any;

	FTraceHandle TraceHandle;
	FTraceDatum TraceDatum;

	//DrawDebugCapsule(GetWorld(), Start, CollisionShapeExtentZ, CollisionShapeExtentZ, FQuat::Identity, FColor::Cyan, false, -1, 0, 0.25);
	//DrawDebugCapsule(GetWorld(), End, CollisionShapeExtentZ, CollisionShapeExtentZ, FQuat::Identity, FColor::Purple, false, -1, 0, 0.25);

	TraceHandle = GetWorld()->AsyncSweepByChannel(EAsyncTraceType::Single, Start, End, FQuat{}, collision_channel, collision_shape,
		query_params, FCollisionResponseParams::DefaultResponseParam, &callback);
}

void UPawnMovement::AsyncTraceDoneCallback(const FTraceHandle& TraceHandle, FTraceDatum& TraceData)
{
	TraceHandlePersistent = TraceHandle;
}

FVector UPawnMovement::NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const
{
	FVector Result = InitialVelocity;

	if (DeltaTime > 0.f)
	{
		//Apply gravity
		Result += Gravity * DeltaTime;

		// Don't exceed terminal velocity.
		const float TerminalLimit = FMath::Abs(GetPhysicsVolume()->TerminalVelocity);
		if (Result.SizeSquared() > FMath::Square(TerminalLimit))
		{
			const FVector GravityDir = Gravity.GetSafeNormal();
			if ((Result | GravityDir) > TerminalLimit)
			{
				Result = FVector::PointPlaneProject(Result, FVector::ZeroVector, GravityDir) + GravityDir * TerminalLimit;
			}
		}
	}

	return Result;
}

bool UPawnMovement::ShouldComputeAccelerationToReachRequestedVelocity(const float RequestedSpeed) const
{
	//Compute acceleration if accelerating toward requested speed
		//CharacterMovementComponent uses 1% buffer, but it's unclear what for, so it is not used
	return /*bRequestedMoveUseAcceleration && */ Velocity.SizeSquared() < FMath::Square(RequestedSpeed /* 1.01f*/);
}

float UPawnMovement::GetWalkableFloorAngle() const
{
	return WalkableFloorAngle;
}

void UPawnMovement::SetWalkableFloorAngle(float InWalkableFloorAngle)
{
	WalkableFloorAngle = FMath::Clamp(InWalkableFloorAngle, 0.f, 90.0f);
}

float UPawnMovement::GetWalkableFloorZ() const
{
	return FMath::Cos(FMath::DegreesToRadians(GetWalkableFloorAngle()));
}

bool UPawnMovement::IsWalkable(const FHitResult& Hit) const
{
	//Gravity rotation has been discarded

	if (!Hit.IsValidBlockingHit())
	{
		// No hit, or starting in penetration
		return false;
	}

	//Never walk up vertical surfaces
	if (Hit.ImpactNormal.Z < UE_KINDA_SMALL_NUMBER)
	{
		return false;
	}

	float TestWalkableZ = GetWalkableFloorZ();

	// See if this component overrides the walkable floor z.
	const UPrimitiveComponent* HitComponent = Hit.Component.Get();
	if (HitComponent)
	{
		const FWalkableSlopeOverride& SlopeOverride = HitComponent->GetWalkableSlopeOverride();
		TestWalkableZ = SlopeOverride.ModifyWalkableFloorZ(TestWalkableZ);
	}

	//Can't walk on this surface if it is too steep.
	if (Hit.ImpactNormal.Z < TestWalkableZ)
	{
		return false;
	}

	return true;
}

void UPawnMovement::OnPawnStuckInGeometry(const FHitResult* Hit) const
{
	UE_LOG(LogPawnMovement, Warning, TEXT("Pawn %s: stuck in geometry"), *PawnOwner.GetName())

	/*if (CharacterMovementCVars::StuckWarningPeriod >= 0)
	{
		const UWorld* MyWorld = World;
		const float RealTimeSeconds = MyWorld->GetRealTimeSeconds();
		if ((RealTimeSeconds - LastStuckWarningTime) >= CharacterMovementCVars::StuckWarningPeriod)
		{
			LastStuckWarningTime = RealTimeSeconds;
			if (Hit == nullptr)
			{
				UE_LOG(LogCharacterMovement, Log, TEXT("%s is stuck and failed to move! (%d other events since notify)"), *CharacterOwner->GetName(), StuckWarningCountSinceNotify);
			}
			else
			{
				UE_LOG(LogCharacterMovement, Log, TEXT("%s is stuck and failed to move! Velocity: X=%3.2f Y=%3.2f Z=%3.2f Location: X=%3.2f Y=%3.2f Z=%3.2f Normal: X=%3.2f Y=%3.2f Z=%3.2f PenetrationDepth:%.3f Actor:%s Component:%s BoneName:%s (%d other events since notify)"),
					*GetNameSafe(CharacterOwner),
					Velocity.X, Velocity.Y, Velocity.Z,
					Hit->Location.X, Hit->Location.Y, Hit->Location.Z,
					Hit->Normal.X, Hit->Normal.Y, Hit->Normal.Z,
					Hit->PenetrationDepth,
					*GetNameSafe(Hit->GetActor()),
					*GetNameSafe(Hit->GetComponent()),
					Hit->BoneName.IsValid() ? *Hit->BoneName.ToString() : TEXT("None"),
					StuckWarningCountSinceNotify
				);
			}
			ensure(false);
			StuckWarningCountSinceNotify = 0;
		}
		else
		{
			StuckWarningCountSinceNotify += 1;
		}
	}*/

	// Don't update velocity based on our (failed) change in position this update since we're stuck.

	//Output.bJustTeleported = true;
}

bool UPawnMovement::CanStepUp(const FHitResult& Hit) const
{
	if (!Hit.IsValidBlockingHit() || MovementMode == PawnMovementMode::Falling || MaxStepHeight <= 0.f)
	{
		return false;
	}

	// No component for "fake" hits when we are on a known good base.
	const UPrimitiveComponent* HitComponent = Hit.Component.Get();
	if (!HitComponent)
	{
		return true;
	}

	// TODO StepUp
	/*if (!HitComponent->CanCharacterStepUp(CharacterOwner))
	{
		return false;
	}*/

	// No actor for "fake" hits when we are on a known good base.
	const AActor* HitActor = Hit.GetActor();
	if (!HitActor)
	{
		return true;
	}

	/*if (!HitActor->CanBeBaseForCharacter(CharacterOwner))
	{
		return false;
	}*/

	return true;
}

bool UPawnMovement::StepUp(const FVector& Delta, const FHitResult& InHit, const FHitResult& FloorHit /*, FStepDownResult* OutStepDownResult*/)
{
	bool IsLineTrace = false; //hardcoded for now

	if (!CanStepUp(InHit))
	{
		return false;
	}

	FVector Origin = UpdatedComponent->Bounds.Origin;
	FVector BoxExtent = UpdatedComponent->Bounds.BoxExtent;

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	UCapsuleComponent* UpdatedCapsuleComponent = nullptr;

	if (UpdatedComponent->IsA<UCapsuleComponent>())
	{
		UpdatedCapsuleComponent = Cast<UCapsuleComponent>(UpdatedComponent);
	}
	else
	{
		//TODO: Implement other cases
		UE_LOG(LogPawnMovement, Warning, TEXT("UPawnMovement::StepUp: Currently UpdatedComponent (Pawn's root?) has to be CapsuleComponent, other cases are not implemented yet"));
		return false;
	}

	// Don't bother stepping up if top of capsule is hitting something.
	const float InitialImpactZ = InHit.ImpactPoint.Z;
	//float PawnRadius = Output.ScaledCapsuleRadius;
	//float PawnHalfHeight = Output.ScaledCapsuleHalfHeight;
	float PawnRadius = 0;
	float PawnHalfHeight = 0;
	UpdatedCapsuleComponent->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	if (InitialImpactZ > OldLocation.Z + (PawnHalfHeight - PawnRadius))
	{
		return false;
	}

	if (GravityVector.IsZero())
	{
		return false;
	}

	//	float MaxStepHeight = MaxStepHeight;

	// Gravity should be a normalized direction
	ensure(GravityVector.IsNormalized());

	float StepTravelUpHeight = MaxStepHeight;
	float StepTravelDownHeight = StepTravelUpHeight;
	const float StepSideZ = -1.f * FVector::DotProduct(InHit.ImpactNormal, GravityVector);
	float PawnInitialFloorBaseZ = OldLocation.Z - PawnHalfHeight;
	float PawnFloorPointZ = PawnInitialFloorBaseZ;

	if (IsMovingOnGround() && IsWalkable(FloorHit))
	{
		// Since we float a variable amount off the floor, we need to enforce max step height off the actual point of impact with the floor.
		const float FloorDist = FMath::Max(0.f, FloorHit.Distance);
		PawnInitialFloorBaseZ -= FloorDist;
		StepTravelUpHeight = FMath::Max(StepTravelUpHeight - FloorDist, 0.f);
		StepTravelDownHeight = (MaxStepHeight + MAX_FLOOR_DIST * 2.f);

		const bool bHitVerticalFace = !IsWithinEdgeTolerance(InHit.Location, InHit.ImpactPoint, PawnRadius);
		if (!IsLineTrace && !bHitVerticalFace)
		{
			PawnFloorPointZ = FloorHit.ImpactPoint.Z;
		}
		else
		{
			// Base floor point is the base of the capsule moved down by how far we are hovering over the surface we are hitting.
			PawnFloorPointZ -= FloorHit.Distance;
		}
	}

	// Don't step up if the impact is below us, accounting for distance from floor.
	if (InitialImpactZ <= PawnInitialFloorBaseZ)
	{
		return false;
	}

	// TODO ScopedMovementUpdate
	// Scope our movement updates, and do not apply them until all intermediate moves are completed.
	//FScopedMovementUpdate ScopedStepUpMovement(UpdatedComponent, EScopedUpdate::DeferredUpdates);

	// step up - treat as vertical wall
	FHitResult SweepUpHit(1.f);
	const FRotator PawnRotation = PawnOwner->GetActorRotation(); //maybe we should get async rotation
	MoveUpdatedComponent(-GravityVector * StepTravelUpHeight, PawnRotation, true, &SweepUpHit);

	if (SweepUpHit.bStartPenetrating)
	{
		// Undo movement
		ensure(false);
		//	ScopedStepUpMovement.RevertMove();
		return false;
	}

	// step fwd
	FHitResult Hit(1.f);
	MoveUpdatedComponent(Delta, PawnRotation, true, &Hit);

	// Check result of forward movement
	if (Hit.bBlockingHit)
	{
		if (Hit.bStartPenetrating)
		{
			// Undo movement
			//ScopedStepUpMovement.RevertMove();
			ensure(false);
			return false;
		}

		// If we hit something above us and also something ahead of us, we should notify about the upward hit as well.
		// The forward hit will be handled later (in the bSteppedOver case below).
		// In the case of hitting something above but not forward, we are not blocked from moving so we don't need the notification.
		if (SweepUpHit.bBlockingHit && Hit.bBlockingHit)
		{
			HandleImpact(SweepUpHit);
		}

		// pawn ran into a wall
		HandleImpact(Hit);
		if (IsFalling())
		{
			return true;
		}

		// adjust and try again
		const float ForwardHitTime = Hit.Time;
		const float ForwardSlideAmount = SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);

		if (IsFalling())
		{
			ensure(false);
			//	ScopedStepUpMovement.RevertMove();
			return false;
		}

		// If both the forward hit and the deflection got us nowhere, there is no point in this step up.
		if (ForwardHitTime == 0.f && ForwardSlideAmount == 0.f)
		{
			ensure(false);
			//ScopedStepUpMovement.RevertMove();
			return false;
		}
	}

	// Step down
	MoveUpdatedComponent(GravityVector * StepTravelDownHeight, UpdatedComponent->GetComponentRotation(), true, &Hit);

	// If step down was initially penetrating abort the step up
	if (Hit.bStartPenetrating)
	{
		ensure(false);
		//ScopedStepUpMovement.RevertMove();
		return false;
	}

	//FStepDownResult StepDownResult;
	if (Hit.IsValidBlockingHit())
	{
		// See if this step sequence would have allowed us to travel higher than our max step height allows.
		const float DeltaZ = Hit.ImpactPoint.Z - PawnFloorPointZ;
		if (DeltaZ > MaxStepHeight)
		{
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (too high Height %.3f) up from floor base %f to %f"), DeltaZ, PawnInitialFloorBaseZ, NewLocation.Z);
			//ScopedStepUpMovement.RevertMove();
			ensure(false);
			return false;
		}

		// Reject unwalkable surface normals here.
		if (!IsWalkable(Hit))
		{
			// Reject if normal opposes movement direction
			const bool bNormalTowardsMe = (Delta | Hit.ImpactNormal) < 0.f;
			if (bNormalTowardsMe)
			{
				//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (unwalkable normal %s opposed to movement)"), *Hit.ImpactNormal.ToString());
				//ScopedStepUpMovement.RevertMove();
				ensure(false);
				return false;
			}

			// Also reject if we would end up being higher than our starting location by stepping down.
			// It's fine to step down onto an unwalkable normal below us, we will just slide off. Rejecting those moves would prevent us from being able to walk off the edge.
			if (Hit.Location.Z > OldLocation.Z)
			{
				//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (unwalkable normal %s above old position)"), *Hit.ImpactNormal.ToString());
				//ScopedStepUpMovement.RevertMove();
				ensure(false);
				return false;
			}
		}

		// Reject moves where the downward sweep hit something very close to the edge of the capsule. This maintains consistency with FindFloor as well.
		if (!IsWithinEdgeTolerance(Hit.Location, Hit.ImpactPoint, PawnRadius))
		{
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (outside edge tolerance)"));
			//ScopedStepUpMovement.RevertMove();
			ensure(false);
			return false;
		}

		// Don't step up onto invalid surfaces if traveling higher.
		if (DeltaZ > 0.f && !CanStepUp(Hit))
		{
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (up onto surface with !CanStepUp())"));
			//ScopedStepUpMovement.RevertMove();
			ensure(false);
			return false;
		}

		// See if we can validate the floor as a result of this step down. In almost all cases this should succeed, and we can avoid computing the floor outside this method.
		if (FloorHit.IsValidBlockingHit())
		{
			//Character code example:
			//FindFloor(UpdatedComponentInput->GetPosition(), StepDownResult.FloorResult, false, &Hit);
			SweepFloorAsync(FTraceDelegate::CreateUObject(this, &UPawnMovement::AsyncTraceDoneCallback));

			//FTraceDatum TempTraceDatum;
			//GetWorld()->QueryTraceData(TraceHandlePersistent, TempTraceDatum);

			// Reject unwalkable normals if we end up higher than our initial height.
			// It's fine to walk down onto an unwalkable surface, don't reject those moves.
			if (Hit.Location.Z > OldLocation.Z)
			{
				// We should reject the floor result if we are trying to step up an actual step where we are not able to perch (this is rare).
				// In those cases we should instead abort the step up and try to slide along the stair.
				if (!FloorHit.bBlockingHit && StepSideZ < MAX_STEP_SIDE_Z)
				{
					//	ScopedStepUpMovement.RevertMove();
					ensure(false);
					return false;
				}
			}

			//TODO: skipped for now, check if it's actually needed
			//StepDownResult.bComputedFloor = true;
		}
	}

	// Copy step down result.
	//if (OutStepDownResult != NULL)
	//{
	//	*OutStepDownResult = StepDownResult;
	//}

	// Don't recalculate velocity based on this height adjustment, if considering vertical adjustments.
	//TODO: skipped for now, check if it's actually needed
	//Output.bJustTeleported |= !bMaintainHorizontalGroundVelocity;

	return true;
} 

void UPawnMovement::MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds)
{
	bool bMaintainHorizontalGroundVelocity = false; //hardcoded for now
	bool bJustTeleported = false; //hardcoded for now
	FHitResult HitResult;
	bool HitFromLineTrace = false; //i.e. not sweep, but currently we sweep only

	//if (!CurrentFloor.IsWalkableFloor())
	//{
	//	return;
	//}
	if (MovementMode == PawnMovementMode::Falling)
	{
		return;
	}

	// Move along the current floor
	const FVector Delta = InVelocity * FVector(1.0, 1.0, 0.0) * DeltaSeconds;
	FHitResult Hit(1.f);
	FVector RampVector = ComputeGroundMovementDelta(Delta, HitResult, HitFromLineTrace);

	SafeMoveUpdatedComponent(RampVector, UpdatedComponent->GetComponentQuat(), true, Hit);
	float LastMoveTimeSlice = DeltaSeconds;

	if (Hit.bStartPenetrating)
	{
		// Allow this hit to be used as an impact we can deflect off, otherwise we do nothing the rest of the update and appear to hitch.
		HandleImpact(Hit);
		SlideAlongSurface(Delta, 1.f, Hit.Normal, Hit, true);

		if (Hit.bStartPenetrating)
		{
			OnPawnStuckInGeometry(&Hit);
		}
	}
	else if (Hit.IsValidBlockingHit())
	{
		// We impacted something (most likely another ramp, but possibly a barrier).
		float PercentTimeApplied = Hit.Time;
		if ((Hit.Time > 0.f) && (Hit.Normal.Z > UE_KINDA_SMALL_NUMBER) && IsWalkable(Hit))
		{
			// Another walkable ramp.
			const float InitialPercentRemaining = 1.f - PercentTimeApplied;
			RampVector = ComputeGroundMovementDelta(Delta * InitialPercentRemaining, Hit, false);
			LastMoveTimeSlice = InitialPercentRemaining * LastMoveTimeSlice;
			SafeMoveUpdatedComponent(RampVector, UpdatedComponent->GetComponentQuat(), true, Hit);

			const float SecondHitPercent = Hit.Time * InitialPercentRemaining;
			PercentTimeApplied = FMath::Clamp(PercentTimeApplied + SecondHitPercent, 0.f, 1.f);
		}

		if (Hit.IsValidBlockingHit())
		{
			if (CanStepUp(Hit) || (PawnOwner->GetMovementBase() != nullptr && Hit.HitObjectHandle == PawnOwner->GetMovementBase()->GetOwner()))
			{
				// hit a barrier, try to step up
				const FVector PreStepUpLocation = UpdatedComponent->GetComponentLocation();

				FTraceDatum TempTraceDatum;
				FHitResult FloorHit;
				if (GetWorld()->QueryTraceData(TraceHandlePersistent, TempTraceDatum) == true)
				{
					FloorHit = TempTraceDatum.OutHits[0];
				}

				if (FloorHit.IsValidBlockingHit() && !StepUp(Delta * (1.f - PercentTimeApplied), Hit, FloorHit))
				{
					UE_LOG(LogPawnMovement, Verbose, TEXT("- StepUp (ImpactNormal %s, Normal %s"), *Hit.ImpactNormal.ToString(), *Hit.Normal.ToString());

					HandleImpact(Hit, LastMoveTimeSlice, RampVector);
					SlideAlongSurface(Delta, 1.f - PercentTimeApplied, Hit.Normal, Hit, true);
				}
				else
				{
					UE_LOG(LogPawnMovement, Verbose, TEXT("+ StepUp (ImpactNormal %s, Normal %s"), *Hit.ImpactNormal.ToString(), *Hit.Normal.ToString());

					if (!bMaintainHorizontalGroundVelocity)
					{
						// Don't recalculate velocity based on this height adjustment, if considering vertical adjustments. Only consider horizontal movement.
						bJustTeleported = true;

						const float StepUpTimeSlice = (1.f - PercentTimeApplied) * DeltaSeconds;
						if (/*!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && */ StepUpTimeSlice >= UE_KINDA_SMALL_NUMBER)
						{
							Velocity = (UpdatedComponent->GetComponentLocation() - PreStepUpLocation) / StepUpTimeSlice;
							Velocity = FVector::VectorPlaneProject(Velocity, -GravityVector);
						}
					}
				}
			}
			else if (Hit.Component.IsValid() && !Hit.Component.Get()->CanCharacterStepUp(PawnOwner))
			{
				HandleImpact(Hit, LastMoveTimeSlice, RampVector);
				SlideAlongSurface(Delta, 1.f - PercentTimeApplied, Hit.Normal, Hit, true);
			}
		}
	}
}

FVector UPawnMovement::ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace) const
{
	bool bMaintainHorizontalGroundVelocity = false; //TODO: hardoced for now

	const FVector FloorNormal = RampHit.ImpactNormal;
	const FVector ContactNormal = RampHit.Normal;

	if (FloorNormal.Z < (1.f - UE_KINDA_SMALL_NUMBER) && FloorNormal.Z > UE_KINDA_SMALL_NUMBER && ContactNormal.Z > UE_KINDA_SMALL_NUMBER && !bHitFromLineTrace && IsWalkable(RampHit))
	{
		// Compute a vector that moves parallel to the surface, by projecting the horizontal movement direction onto the ramp.
		const float FloorDotDelta = (FloorNormal | Delta);
		FVector RampMovement(Delta.X, Delta.Y, -FloorDotDelta / FloorNormal.Z);

		if (bMaintainHorizontalGroundVelocity)
		{
			return RampMovement;
		}
		else
		{
			return RampMovement.GetSafeNormal() * Delta.Size();
		}
	}

	return Delta;
}

void UPawnMovement::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	//Override NavMovementComponent since we don't want something else from outside to simply set new velocity

	if (MoveVelocity.SizeSquared() < UE_KINDA_SMALL_NUMBER)
	{
		return;
	}

	//if (ShouldPerformAirControlForPathFollowing())
	//{
	//	const FVector FallVelocity = MoveVelocity.GetClampedToMaxSize(GetMaxSpeed());
	//	PerformAirControlForPathFollowing(FallVelocity, FallVelocity.Z);
	//	return;
	//}

	RequestedVelocity = MoveVelocity;
	//bHasRequestedVelocity = true;
	//bRequestedMoveWithMaxSpeed = bForceMaxSpeed;

	//TODO: might need a different approach
	//Discard Z when moving on ground or falling
	if (IsMovingOnGround() || IsFalling())
	{
		RequestedVelocity.Z = 0.0f;
	}
}

void UPawnMovement::RequestPathMove(const FVector& MoveInput)
{
	checkf(false, TEXT("Not tested, no idea where it's being called from since AIController calls RequestDirectMove"))

	FVector AdjustedMoveInput(MoveInput);
	
	//preserve magnitude when moving on ground/falling and requested input has Z component
	//additionally see CharacterMovementComponent::ConstrainInputAcceleration for details
	if (MoveInput.Z != 0.f && (IsMovingOnGround() || IsFalling()))
	{
		const float Mag = MoveInput.Size();
		AdjustedMoveInput = MoveInput.GetSafeNormal2D() * Mag;
	}

	Super::RequestPathMove(AdjustedMoveInput);
}

bool UPawnMovement::CanStartPathFollowing() const
{
	if (PawnOwner)
	{
		if (PawnOwner->GetRootComponent() && PawnOwner->GetRootComponent()->IsSimulatingPhysics())
		{
			return false;
		}
	}

	return true;
}

bool UPawnMovement::CanStopPathFollowing() const
{
	return !IsFalling();
}

float UPawnMovement::GetPathFollowingBrakingDistance(float MaxSpeedParam) const
{
	if (NavMovementProperties.bUseFixedBrakingDistanceForPaths)
	{
		return NavMovementProperties.FixedPathBrakingDistance;
	}

	const float BrakingDeceleration = FMath::Abs(GetBrakingDeceleration());

	//pawn won't be able to stop with negative or nearly zero deceleration, use MaxSpeed for path length calculations
	float BrakingDistance = 0;
	if (BrakingDeceleration < UE_SMALL_NUMBER)
	{
		BrakingDistance = MaxSpeedParam;
	}
	else
	{
		BrakingDistance = FMath::Square(MaxSpeedParam) / (2.f * BrakingDeceleration);
	}

	return BrakingDistance;
}

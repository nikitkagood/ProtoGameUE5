// Nikita Belov, All rights reserved


#include "Components/PawnMovement.h"
#include "AIController.h"

void UPawnMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UE_LOG(LogTemp, Warning, TEXT("TickComponent: velocity: %s"), *Velocity.ToString())

	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}

	//Apply gravity
	constexpr double GravitationalAccelerationConstant = 9.81 * 100;
	Velocity += GravityVector * GravitationalAccelerationConstant * DeltaTime;

	IsMovingOnGround

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
		if (IsExceedingMaxSpeed(MaxSpeed) == true)
		{
			Velocity = Velocity.GetUnsafeNormal() * MaxSpeed;
		}

		LimitWorldBounds();
		bPositionCorrected = false;

		// Move actor
		FVector Delta = Velocity * DeltaTime;

		if (!Delta.IsNearlyZero(1e-6f))
		{
			const FVector OldLocation = UpdatedComponent->GetComponentLocation();
			const FQuat Rotation = UpdatedComponent->GetComponentQuat();

			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(Delta, Rotation, true, Hit);

			if (Hit.IsValidBlockingHit())
			{
				HandleImpact(Hit, DeltaTime, Delta);
				//Try to slide the remaining distance along the surface
				//Only if controlled by AI or a player
				Cast<AAIController>(Controller);
				if (Controller->IsFollowingAPath())
				{
					SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
				};
			}

			// Update velocity
			// We don't want position changes to vastly reverse our direction (which can happen due to penetration fixups etc)
			if (!bPositionCorrected)
			{
				//const FVector NewLocation = UpdatedComponent->GetComponentLocation();
				//Velocity = ((NewLocation - OldLocation) / DeltaTime);
			}
		}

		// Finalize
		UpdateComponentVelocity();
	}

}

void UPawnMovement::ApplyControlInputToVelocity(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("ApplyControlInputToVelocity: velocity: %s"), *Velocity.ToString())

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

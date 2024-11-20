// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "WorldCollision.h"

#include "PawnMovement.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPawnMovement, Display, All);

struct FStepDownResult;

UENUM()
enum class PawnMovementMode
{
	None = 0,

	//Gravity on, cannot move (unless air control)
	Falling,

	//Needs to be standing on a floor in order to move
	MovingOnGround,

	/** Swimming through a fluid volume, under the effects of gravity and buoyancy. */
	Swimming,

	//No gravity, don't check for a floor, TODO: Affected by the current physics volume's fluid friction?
	Flying,

	Custom		UMETA(DisplayName = "Custom"),
};

//DEPRECATED: UPawnMovement isn't finished and Mover is used instead
//General movement component for Pawns (pretty much a better CharacterMovementComponent)
//Aimed to be lightweight, as fast as possible and async, more features and customizibility
// 
//Features missing: some movement modes, Root Motion, replication
UCLASS(meta = (Deprecated))
class PROTOGAME_API UPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()

public:
	UPawnMovement();

	virtual bool IsMovingOnGround() const override;
	virtual bool IsFalling() const override;
	virtual bool IsFlying() const override; //TODO: implement
	virtual bool IsSwimming() const override; //TODO: implement
	virtual bool IsCrouching() const override { return false; }; //not supported (at least for now)

	//BEGIN UNavMovementComponent Interface
	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;
	//TODO: For the time being it's never used, even when Pathing...
	virtual void RequestPathMove(const FVector& MoveInput) override;
	virtual bool CanStartPathFollowing() const override;
	virtual bool CanStopPathFollowing() const override;
	virtual float GetPathFollowingBrakingDistance(float MaxSpeedParam) const override;
	//END UNavMovementComponent Interface

	///** Rotate a vector from world to gravity space. */
	//FVector RotateGravityToWorld(const FVector& World) const { return WorldToGravityTransform.RotateVector(World); }

	///** Rotate a vector gravity to world space. */
	//FVector RotateWorldToGravity(const FVector& Gravity) const { return GravityToWorldTransform.RotateVector(Gravity); }

	/**
	 * Return true if the 2D distance to the impact point is inside the edge tolerance (CapsuleRadius minus a small rejection threshold).
	 * Useful for rejecting adjacent hits when finding a floor or landing spot.
	 */
	virtual bool IsWithinEdgeTolerance(const FVector& CapsuleLocation, const FVector& TestImpactPoint, const float CapsuleRadius) const;

	float GetBrakingDeceleration() const;

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetMovementMode(PawnMovementMode movement_mode);

	virtual void ApplyControlInputToVelocity(float DeltaTime) override;

	//Sweep starts from the exact bottom of Bounding Box + shape extent
	//Be careful with collision shapes. Even if the actor uses the same shape, it may not match perfectly. Also sphere has flat bottom, while capsule doesn't. 
	// OutHit.Distance is very likely to be decently far from zero
	//It's more for testing rather than actual use, since everything is meant to be async
	//TODO: not tested
	FHitResult SweepFloorSync(ECollisionChannel collision_channel = ECollisionChannel::ECC_Pawn, const FCollisionShape& collision_shape = FCollisionShape::MakeCapsule(25, 25));

	//Use callback and QueryTraceData(FTraceHandle) to get results
	//Sweep starts from the exact bottom of Bounding Box + shape extent
	//Be careful with collision shapes. Even if the actor uses the same shape, it may not match perfectly. Also sphere has flat bottom, while capsule doesn't. 
	// OutHit.Distance is very likely to be decently far from zero
	void SweepFloorAsync(FTraceDelegate callback, ECollisionChannel collision_channel = ECollisionChannel::ECC_Pawn, const FCollisionShape& collision_shape = FCollisionShape::MakeCapsule(25, 25));

	void AsyncTraceDoneCallback(const FTraceHandle& TraceHandle, FTraceDatum& TraceData);

	//TODO: customizible floor calc location
	//void GetFloorCalcStartingLocation();

	//Calculates gravity
	FVector NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const;

	//When a pawn requests a velocity (like when following a path), this method returns true if when we should compute the
	// acceleration toward requested velocity. 
	//If it returns false, velocity is expected to decelerate or instantly snap to requested velocity.
	virtual bool ShouldComputeAccelerationToReachRequestedVelocity(const float RequestedSpeed) const;

	float GetWalkableFloorAngle() const;
	void SetWalkableFloorAngle(float InWalkableFloorAngle);

	/** Get the Z component of the normal of the steepest walkable surface for the character. Any lower than this and it is not walkable. */
	float GetWalkableFloorZ() const;

	bool IsWalkable(const FHitResult& Hit) const;

	void OnPawnStuckInGeometry(const FHitResult* Hit) const;

	bool CanStepUp(const FHitResult& Hit) const;
	bool StepUp(const FVector& Delta, const FHitResult& InHit, const FHitResult& FloorHit /*, FStepDownResult* OutStepDownResult*/); //TODO

	/**
	 * Move along the floor, using CurrentFloor and ComputeGroundMovementDelta() to get a movement direction.
	 * If a second walkable surface is hit, it will also be moved along using the same approach.
	 *
	 * @param InVelocity:			Velocity of movement
	 * @param DeltaSeconds:			Time over which movement occurs
	 */
	 virtual void MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds);
	 FVector ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace) const;

	//Where gravity points and it's strength
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnMovement)
	FVector GravityVector = FVector::DownVector;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = PawnMovement)
	PawnMovementMode MovementMode = PawnMovementMode::None;

	bool ShouldBreakImmediately;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PawnMovement)
	float AccelerationV2 = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PawnMovement)
	float DecelerationV2 = 100;

	//Value is arbitrary
	//How far we Trace/Sweep to find floor
	const float MAX_FLOOR_DIST = 2.4f;

	//Floor sweep doesn't produce perfect results, distance to the ground is never zero
	//If you are seeing pawns stuck in Falling mode when they should not, adjust this value
	//But this DOES mean pawns will be considered on the floor when they are *value* above the ground
	//
	//- 1.2f should be enough for standard sized Character CapsuleComponent, currently nothing else is tested
	//- I can already see how Cube collision hits an edge against a slope and FloorSweep calculated from the center, resulting in massive Distance
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = PawnMovement, meta = (AllowPrivateAccess = "true"))
	float SweepDistanceFloorTolerance = 1.5f;

	const float SWEEP_EDGE_REJECT_DISTANCE = 0.15f;

	// We should reject the floor result if we are trying to step up an actual step where we are not able to perch (this is rare).
	// In those cases we should instead abort the step up and try to slide along the stair.
	const float MAX_STEP_SIDE_Z = 0.08f;

private:
	//Used for async traces
	FTraceHandle TraceHandlePersistent;

	UPROPERTY(Transient, Category = "PawnMovement", VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector RequestedVelocity;

	UPROPERTY(Category = "Movement: Walking", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0", ForceUnits = "degrees"))
	float WalkableFloorAngle = 44.0f;

	UPROPERTY(Category = "Movement: Walking", EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxStepHeight = 25;

};

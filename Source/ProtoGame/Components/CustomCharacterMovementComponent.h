// Nikita Belov, All rights reserved

#pragma once

#include "Character/GameCharacterBase.h"

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "CustomCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EWalkingModeMovementType : uint8
{
	Stand = 0				UMETA(DisplayName = "Stand"),
	StandToProne			UMETA(DisplayName = "Stand To Prone"),
	StandToCrouch			UMETA(DisplayName = "Stand To Crouch"),
	Prone					UMETA(DisplayName = "Prone"),
	ProneToCrouch			UMETA(DisplayName = "Prone To Crouch"),
	ProneToStand			UMETA(DisplayName = "Prone To Stand"),
	Crouch					UMETA(DisplayName = "Crouch"),
	CrouchToStand			UMETA(DisplayName = "Crouch To Stand"),
	CrouchToProne			UMETA(DisplayName = "Crouch To Prone"),
	//SlowWalk				UMETA(DisplayName = "SlowWalk"),
	//Sprint				UMETA(DisplayName = "Sprint"),

};

//Added new fields to fully integrate new functionality
//struct PROTOGAME_API FCustomCharacterMovementComponentAsyncInput : public FCharacterMovementComponentAsyncInput
//{
//	float WalkSpeedWalk;
//	float WalkSpeedSprint;
//	float WalkSpeedProne;
//};

/**
 * Extends UCharacterMovementComponent (sometimes UNavMovementComponent)
 * Walking mode is extended. What was walk (in fact jog)/crouch now is jog, walk, crouch, prone.
 * Works only with AGameCharacterBase, which in turn extends ACharacter.
 * Inheritance is opposed to copying whole parent class and modifying it.
 * 
 * TODO: Async and networking aren't integrated, since parent class isn't always overrideable.
 *	This will require either extensive workarounds or copying whole class.
 *	Right now it's deemed not to be important. But if any issues arise, it will has to be fixed. 
 */
UCLASS()
class PROTOGAME_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
		
	friend UCharacterMovementComponent;

public:
	UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//Convenience function. Since CharacterOwner is ACharacter type, and we can't use it directly.
	TObjectPtr<AGameCharacterBase> GetCharacterOwner() const;

	virtual float GetMaxSpeed() const override;

	void StartSlowWalk();
	void EndSlowWalk();

	void StartSprint();
	void EndSprint();

	/**
	 * Checks if new capsule size fits (no encroachment), and call CharacterOwner->OnStartProne() if successful.
	 * In general you should set bWantsToProne instead to have the crouch persist during movement, or just use the prone functions on the owning Character.
	 * @param	bClientSimulation	true when called when bIsProne is replicated to non owned clients, to update collision cylinder and offset.
	 */
	void Prone(bool bClientSimulation = false);

	/**
	 * Checks if default capsule size fits (no encroachment), and trigger OnEndProne() on the owner if successful.
	 * @param	bClientSimulation	true when called when bIsProne is replicated to non owned clients, to update collision cylinder and offset.
	 */
	void UnProne(bool bClientSimulation = false);

	virtual void Crouch(bool bClientSimulation = false) override;
	void UnCrouch(bool bClientSimulation = false) override;

	bool CanSlowWalkInCurrentState() const;
	bool CanSprintInCurrentState() const;
	bool CanProneInCurrentState() const;
	virtual bool CanCrouchInCurrentState() const override;

	/** Sets collision half-height when crouching and updates dependent computations */
	//UFUNCTION(BlueprintSetter)
	//void SetProneHalfHeight(const float NewValue);

	/** Returns the collision half-height when crouching (component scale is applied separately) */
	UFUNCTION(BlueprintGetter)
	float GetProneHalfHeight() const { return ProneHalfHeight; };

	UFUNCTION(BlueprintCallable)
	bool IsSlowWalking() const;

	UFUNCTION(BlueprintCallable)
	bool IsSprinting() const;

	UFUNCTION(BlueprintCallable)
	bool IsProne() const;

	bool CanEverSlowWalk() const { return bCanSlowWalk; }
	bool CanEverSprint() const { return bCanSprint; }
	bool CanEverProne() const { return bCanProne; }


	/** Update the character state in PerformMovement right before doing the actual position change */
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	/** Update the character state in PerformMovement after the position change. Some rotation updates happen after this. */
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

protected:
	//virtual void FillAsyncInput(const FVector& InputVector, FCharacterMovementComponentAsyncInput& AsyncInput) override;
	//virtual void BuildAsyncInput() override;

public:
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedSlowWalk;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedSprint;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedProne;

	/** If true, try to prone (or keep prone) on next update. If false, try to stop prone on next update. */
	UPROPERTY(Category = "Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToProne : 1;

	//Expands UNavMovementComponent
	//Allow prone or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementCapabilities)
	uint8 bCanProne : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementCapabilities)
	uint8 bCanSprint: 1;

	//I.e. walk slowly, default movement is jog
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementCapabilities)
	uint8 bCanSlowWalk : 1;

	//Collision half-height when prone
	//Can't be smaller than radius
	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetCrouchedHalfHeight, BlueprintGetter = GetCrouchedHalfHeight, meta = (ClampMin = "0", UIMin = "0", ForceUnits = cm))
	float ProneHalfHeight;

	//Move capsule down or not
	UPROPERTY(Category = "Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadWrite, AdvancedDisplay)
	uint8 bProneMaintainsBaseLocation : 1;
};

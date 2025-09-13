// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MoverSimulationTypes.h"

#include "MoverNavPawn.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMoverPawn, Log, All);

class UNavMoverComponent;
class UCharacterMoverComponent;

//What is necessary to make Mover (+Nav) work:
//-Firstly make sure a (mover/nav)component is even valid (DefaultSubobject or BP and FindByClass), 
// so we can have UpdatedComponent and a valid Location
//-IMoverInputProducerInterface and ProduceInput_Implementation - the main things to implement, copy from MoverExamplesCharacter pretty much
// cut dependencies that you don't need
//-GetNavAgentLocation isn't mandatory but preferable since APawn::GetNavAgentLocation may not be optimal
//-Also check for movement modes being properly set (mostly Walking)
// 
//-NavWalking is just simplified Walking and isn't mandatory even when AI navigation is used
//-CachedMoveInputIntent and CachedMoveInputVelocity are for RequestMoveByIntent/Velocity, they can be cut out

//Pawn that uses Mover and NavMover
//TODO: When intent suddenly changes in completely opposite direction it looks weird
UCLASS()
class PROTOGAME_API AMoverNavPawn : public APawn, public IMoverInputProducerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMoverNavPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//~ Begin IMoverInputProducerInterface
	
	//Option to override it in BP has been removed for simplicity
	virtual void ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& OutInputCmd) override;
	//~ End IMoverInputProducerInterface

	//~ Begin INavAgentInterface Interface
	virtual FVector GetNavAgentLocation() const override;
	//~ End INavAgentInterface Interface

	// Request the character starts moving with an intended directional magnitude. A length of 1 indicates maximum acceleration.
	UFUNCTION(BlueprintCallable, Category = MoverNavPawn)
	virtual void RequestMoveByIntent(const FVector& DesiredIntent) { CachedMoveInputIntent = DesiredIntent; }

	// Request the character starts moving with a desired velocity. This will be used in lieu of any other input.
	UFUNCTION(BlueprintCallable, Category = MoverNavPawn)
	virtual void RequestMoveByVelocity(const FVector& DesiredVelocity) { CachedMoveInputVelocity = DesiredVelocity; }

	//Mover has an unusual variable access. They are stored in a SyncState.
	const FMoverDefaultSyncState* GetMoverSyncState() const;


	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = MoverNavPawn)
	//bool GetDesiredRotationRef(FRotator& OutRotation);
	//FRotator* GetDesiredRotationPtr(bool& OutResult);

	//Set where ProduceInput will source it's desired rotation from
	//bool SetDesiredRotationRef(FRotator& OutRotation);
protected:
	FMoverDefaultSyncState* GetMoverSyncStateMutable() const;

public:
	//Using interpolation to smooth change in velocity direction
	//Note that it's not going to interpolate when MovementInput is too small
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoverNavPawn)
	bool bInterpolateMoveInput = true;

	//Speed of interpolation. The value heavily depends on type of interpolation function.
	//VInterpTo: 0.002 is suitable for something on foot and almost instantly changes direction
	//VInterpConstTO: more like 45 or so
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoverNavPawn)
	float MovementVelocityInterpSpd = 0.002;

	// Whether or not we author our movement inputs relative to whatever base we're standing on, or leave them in world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoverNavPawn)
	bool bUseBaseRelativeMovement = true;

	//If true, rotate the Character toward the direction the actor is moving
	//If false controller values will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoverNavPawn)
	bool bOrientRotationToMovement = true;

	// canceling out any z intent if the actor is supposed to remain vertical
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoverNavPawn)
	bool bShouldRemainVertical = true;

	/**
	 * If true, the actor will continue orienting towards the last intended orientation (from input) even after movement intent input has ceased.
	 * This makes the character finish orienting after a quick stick flick from the player.  If false, character will not turn without input.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoverNavPawn)
	bool bMaintainLastInputOrientation = false;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Transient, Category = Movement)
	TObjectPtr<UCharacterMoverComponent> CharacterMoverComponent;

	//Not initialized by default. To be added in BP.
	//Holds functionality for nav movement data and functions
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Nav Movement")
	TObjectPtr<UNavMoverComponent> NavMoverComponent;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = MoverNavPawn)
	FVector CachedMoveInputIntent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = MoverNavPawn)
	FVector CachedMoveInputVelocity;

	FVector LastAffirmativeMoveInput = FVector::ZeroVector;	// Movement input (intent or velocity) the last time we had one that wasn't zero
};

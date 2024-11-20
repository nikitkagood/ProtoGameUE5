// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "BuoyancyComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Interfaces/InteractionInterface.h"

#include "GameCharacterBase.generated.h"

class ACharacter;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class UCharacterMovementComponent;
class UMeshComponent;
//custom
class UCustomCharacterMovementComponent;
class UVitalityComponent;
class URPGStatsComponent;
class UInventoryManager;
class UInventoryComponent;
class UInvSpecialSlotComponent;
class UItemBase;
class UInventoryItem;
class AItemActor;


//8 flags max but can be extended if needed
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EMovementInputToggleFlags : uint8
{
	None = 0			UMETA(DisplayName = "None", Hidden),
	SlowWalk = 1		UMETA(DisplayName = "SlowWalk"),
	Sprint = 2			UMETA(DisplayName = "Sprint"),
	Prone = 4			UMETA(DisplayName = "Prone"),
	Crouch = 8			UMETA(DisplayName = "Crouch"),
};
ENUM_CLASS_FLAGS(EMovementInputToggleFlags);

UENUM(BlueprintType)
enum class ECharacterInventoryType : uint8
{
	None = 0				UMETA(DisplayName = "None"),
	Pockets					UMETA(DisplayName = "Pockets"),
	Backpack				UMETA(DisplayName = "Backpack"),
	ChestRig				UMETA(DisplayName = "ChestRig"),
};


UCLASS(config=Game, BlueprintType)
class PROTOGAME_API AGameCharacterBase : public ACharacter
{
	GENERATED_BODY()

	friend UVitalityComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* InHandsSkeletalMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* StowedOnBackSkeletalMesh;

	//It kinda has to be skeletal mesh but at the moment it is static
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UMeshComponent* BackpackMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UMeshComponent* ChestRigMesh;

	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	//AItemActor* InHandsActor;

	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	//AItemActor* StowedOnBackActor;

	//Collision (clipping) avoidance. Prototype. Doesn't really work with visible head and neck
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm_FPCam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	UVitalityComponent* VitalityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	URPGStatsComponent* RPGStatsComponent;


	////Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	UInventoryManager* InventoryManager;

	//On character inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	UInventoryComponent* InventoryComponent_Pockets;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	UInvSpecialSlotComponent* InventoryItemSlot_Backpack;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	UInvSpecialSlotComponent* InventoryItemSlot_ChestRig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	UInvSpecialSlotComponent* PrimaryGunSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	UInvSpecialSlotComponent* SecondaryGunSlot;

	//what is currently in hands
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	UInvSpecialSlotComponent* ActiveSlot;

public:
	AGameCharacterBase(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	UInventoryManager* GetInventoryManger() const { return InventoryManager; }
	UInventoryComponent* GetInventoryComponent(ECharacterInventoryType type) const;

	UFUNCTION(BlueprintCallable)
	UCustomCharacterMovementComponent* GetCharacterMovement() const;
	
	TObjectPtr<AActor> GetInteractionActor() const { return interaction_actor; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Maybe will be needed, see ACharacter
	//virtual void GetReplicatedCustomConditionState(FCustomPropertyConditionState& OutActiveState) const override;

	////Character controls section

	//also de-equips
	bool EquipGun(UItemBase* item);

	void MoveForward(float Val);
	void MoveRight(float Val);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	virtual void Jump() override;
	UFUNCTION()
	void EndJump(const FHitResult& hit);
	virtual void OnJumped_Implementation() override;

	void ToggleCrouch();

	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;

	void ToggleProne();
	void StartProne();
	void EndProne();

	/**
	 * Called when Character goes prone. Called on non-owned Characters through bIsProne replication.
	 * @param	HalfHeightAdjust		difference between default collision half-height, and actual prone capsule half-height.
	 * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account. 
	 */
	virtual void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnStartProne", ScriptName = "OnStartProne"))
	void K2_OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	/** 
	 * Called when Character stops being prone. Called on non-owned Characters through bIsProne replication.
	 * @param	HalfHeightAdjust		difference between default collision half-height, and actual prone capsule half-height.
	 * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	 */
	virtual void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnEndProne", ScriptName = "OnEndProne"))
	void K2_OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	//void RecalculateProneEyeHeight();

	void ToggleSprint();
	void StartSprint();
	void EndSprint();

	void ToggleSlowWalk();
	void StartSlowWalk();
	void EndSlowWalk();

	bool CanSlowWalk() const;
	bool CanSprint() const;
	bool CanProne() const;
	virtual bool CanCrouch() const override;

	//bool IsFlagSet_ToggleInputMovement(EMovementInputToggleFlags flag);
	//void SetFlag_ToogleInputMovement(EMovementInputToggleFlags flag, bool value);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI")
	void OpenInteractionUI();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI")
	void CloseInteractionUI();
protected:
	//Gun in hands only
	void StartFireActive();
	//Gun in hands only
	void EndFireActive();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UpdateAllWeaponSlots();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UpdateWeaponSlot(UInvSpecialSlotComponent* slot);

	//void OnAction();

	UFUNCTION(BlueprintCallable, Category = "Items")
	void UseItem(UItemBase* item);

	//void DropItem();

	//DamageAmount - positive number
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void EnableRagdoll();
	void DisableRagdoll();

	void SetDeathState(bool is_dead);

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	//Ignores this actor
	//Currently we check for visibility but in case it won't be enough, use Interaction trace channel - ECC_GameTraceChannel3
	UFUNCTION(BlueprintCallable)
	FHitResult SweepInteractionFromView(ECollisionChannel collision_channel = ECollisionChannel::ECC_Visibility);

	//We need to loop sweep (trace) in order for outline and UI to work properly
	//But since we are looping already, it's also used for Action/interaction itself
	//Result is stored in interaction_actor
	UFUNCTION()
	void SweepInteractionLoop();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interaction")
	EInteractionActions GetInteractionAction();

private:
	void SetupMovementDefaults();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange = 130.f;

	FTimerHandle InteractionSweepTimerHandle;

	//May not be necessary - just use crouched value. Game is mainly 1st person anyway
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	//float ProneEyeHeight;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	uint8 bIsSlowWalking : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	uint8 bIsSprinting : 1;

	/** Set by character movement to specify that this Character is currently crouched. */
	UPROPERTY(BlueprintReadOnly, replicatedUsing = OnRep_IsProne, Category = "Character")
	uint8 bIsProne : 1;

	/** Set by character movement to specify that this Character is currently crouched. */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	uint8 bIsJumping : 1;

	/** Handle Prone replicated from server */
	UFUNCTION()
	virtual void OnRep_IsProne();

	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = EMovementInputToggleFlags))
	uint8 MovementToggleFlags = 0;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Movement")
	bool bAimButtonDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Movement")
	bool bFireButtonDown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Movement")
	float AimDownSightsSpeed;

	///** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera)
	//float BaseTurnRate;

	///** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera)
	//float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Interaction")
	TObjectPtr<AActor> interaction_actor;
};


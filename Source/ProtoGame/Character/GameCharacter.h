// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Interfaces/InteractionInterface.h"

#include "GameCharacter.generated.h"

class ACharacter;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class UCharacterMovementComponent;
//custom
class UVitalityComponent;
class URPGStatsComponent;
class UInventoryComponent;
class UItemBase;
class AItemActor;
class UInvSpecialSlotComponent;

UCLASS(config=Game)
class AGameCharacterBase : public ACharacter
{
	GENERATED_BODY()

	friend UVitalityComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* InHandsSkeletalMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* StowedOnBackSkeletalMesh;

	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	//AItemActor* InHandsActor;

	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	//AItemActor* StowedOnBackActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	UVitalityComponent* VitalityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	URPGStatsComponent* RPGStatsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	UInvSpecialSlotComponent* PrimaryGunSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	UInvSpecialSlotComponent* SecondaryGunSlot;

	//what is currently in hands
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	UInvSpecialSlotComponent* ActiveSlot;

public:
	AGameCharacterBase();

	//also de-equips
	bool EquipGun(UItemBase* item);

protected:
	virtual void BeginPlay();

	void MoveForward(float Val);
	void MoveRight(float Val);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void ToggleCrouch();
	void StartCrouch();
	void EndCrouch();

	void ToggleProne();
	void StartProne();
	void EndProne();

	void ToggleSprint();
	void StartSprint();
	void EndSprint();

	void ToggleWalk();
	void StartWalk();
	void EndWalk();
	
	void StartFire();
	void EndFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UpdateWeaponSlots();

	void OnAction();

	UFUNCTION(BlueprintCallable, Category = "Items")
	void UseItem(UItemBase* item);

	void DropItem();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void EnableRagdoll();
	void DisableRagdoll();

	void SetDeathState(bool is_dead);
	//void Ressurect();

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	FHitResult GetInteractionInfo();

private:
	void SetupMovementDefaults();

public:

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	//virtual void Tick(float DeltaTime) override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	bool bJumpButtonDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	bool bCrouchButtonDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	bool bProneButtonDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	bool bWalkButtonDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	bool bSprintButtonDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	bool bAimButtonDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	bool bFireButtonDown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	float JogSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	float SprintSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	float CrouchSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	float ProneSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Movement)
	float AimDownSightsSpeed;

	///** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera)
	//float BaseTurnRate;

	///** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera)
	//float BaseLookUpRate;
};


// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"

#include "Interfaces/InteractionInterface.h"
#include "Pawn/MoverNavPawn.h"

#include "GameCharacterBaseMover.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class UMeshComponent;

struct FInputActionValue;

//custom
class UVitalityComponent;
class URPGStatsComponent;
class UInventoryManager;
class UInventoryComponent;
class UInvSpecialSlotComponent;
class UItemBase;
class UInventoryItem;
class AItemActor;

//From GameCharacterBase
enum class EMovementInputToggleFlags : uint8;
enum class ECharacterInventoryType : uint8;

UCLASS(BlueprintType)
class PROTOGAME_API AGameCharacterBaseMover : public AMoverNavPawn
{
	GENERATED_BODY()

private:
	//friend UVitalityComponent;

	//In theory Mover supports any collision comp, but we'll be using Capsule by default anyway
	//which is set in constructor and is not easily changed in Editor
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Collision)
	UShapeComponent* CollisionComponent;

	//Main mesh of the Charaacter; Attach other meshes to it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Mesh)
	USkeletalMeshComponent* CharacterMesh;


	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Mesh)
	USkeletalMeshComponent* InHandsSkeletalMesh;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Mesh)
	USkeletalMeshComponent* OnBackSkeletalMesh;

	//Intended to be both Static or Skeletal but this has be decided in class constructor
	//So in Editor there won't be option
	//UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Mesh)
	//UMeshComponent* BackpackMesh;

	//UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Mesh)
	//UMeshComponent* ChestRigMesh;

	//(Item)Actor changed to SkeletalMesh due to functionality ItemActor has that interferes with being "in hands", maybe memory;  TODO: may change back to Actor
	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	//AItemActor* InHandsActor;

	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	//AItemActor* StowedOnBackActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Camera)
	USpringArmComponent* SpringArm_FPCam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Camera)
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	UVitalityComponent* VitalityComponent;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	URPGStatsComponent* RPGStatsComponent;


	////Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	UInventoryManager* InventoryManager;

	//On character inventory
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	//UInventoryComponent* InventoryComponent_Pockets;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	//UInvSpecialSlotComponent* InventoryItemSlot_Backpack;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	//UInvSpecialSlotComponent* InventoryItemSlot_ChestRig;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	//UInvSpecialSlotComponent* PrimaryGunSlot;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	//UInvSpecialSlotComponent* SecondaryGunSlot;

	//what is currently in hands
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	UInvSpecialSlotComponent* ActiveSlot;



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Interaction")
	TObjectPtr<AActor> interaction_actor;


public:
	AGameCharacterBaseMover(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay();

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

public:
	auto GetCollisionComponent() const { return CollisionComponent; }
	auto GetCharacterMesh() const {	return CharacterMesh; };
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	UInventoryManager* GetInventoryManger() const { return InventoryManager; }
	//UInventoryComponent* GetInventoryComponent(ECharacterInventoryType type) const;

	//UFUNCTION(BlueprintCallable, Category = "Interaction")
	//AActor* GetInteractionActor() const { return interaction_actor; }

	//also de-equips
	//TODO: refactor into: set -> binded event 
	bool EquipGun(UItemBase* item);


	UFUNCTION(BlueprintCallable, Category = "Items")
	void UseItem(UItemBase* item);

	//To be implemented by child BP
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI")
	void OpenInteractionUI();

	//To be implemented by child BP
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI")
	void CloseInteractionUI();


public:
	//Movement

	void ToggleSprint();
	void StartSprint();
	void EndSprint();

protected:

	//Interaction
	
	//Ignores this actor
	//Currently we check for visibility but in case it won't be enough, use Interaction trace channel - ECC_GameTraceChannel3
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	FHitResult SweepInteractionFromView(ECollisionChannel collision_channel = ECollisionChannel::ECC_Visibility);

	//We need to loop sweep (trace) in order for outline and UI to work properly
	//But since we are looping already, it's also used for Action/interaction itself
	//Result is stored in interaction_actor
	UFUNCTION()
	void SweepInteractionLoop();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interaction")
	EInteractionActions GetInteractionAction();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange = 130.f;

	FTimerHandle InteractionSweepTimerHandle;
};

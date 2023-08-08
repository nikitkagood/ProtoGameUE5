// Copyright Epic Games, Inc. All Rights Reserved.

//custom
#include "Character/GameCharacterBase.h"
#include "Item/Projectile.h"
#include "Item/ItemBase.h"
#include "Item/ItemActor.h"
#include "Item/WeaponGun.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InvSpecialSlot.h"
#include "Inventory/InventoryManager.h"
#include "Components/VitalityComponent.h"
#include "Components/RPGStatsComponent.h"
#include "Components/CustomCharacterMovementComponent.h"
#include "Library/BitmaskLib.h"

//engine
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"

//debug
#include "DrawDebugHelpers.h"

//It's for BindAction with parameter
DECLARE_DELEGATE_OneParam(CharInputBool, bool);

DEFINE_LOG_CATEGORY_STATIC(LogCharacter, Log, All);

AGameCharacterBase::AGameCharacterBase(const class FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	GetCapsuleComponent()->SetVisibility(false, false);
	GetCapsuleComponent()->SetHiddenInGame(false, false);

	SetupMovementDefaults();

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	//FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	//FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	SpringArm_FPCam = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm_FPCam"));
	SpringArm_FPCam->SetupAttachment(GetMesh());

	VitalityComponent = CreateDefaultSubobject<UVitalityComponent>(TEXT("VitalityComponent"));
	RPGStatsComponent = CreateDefaultSubobject<URPGStatsComponent>(TEXT("RPGStatsComponent"));

	InventoryManager = CreateDefaultSubobject<UInventoryManager>(TEXT("InventoryManager"));
	InventoryComponent_Pockets = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent_Pockets"));

	PrimaryGunSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Primary gun slot component"));
	SecondaryGunSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Secondary gun slot component"));
	//It is created so it is visibile in Blueprints, otherwise it's redundant
	ActiveSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Active slot component"));

	//ECC_GameTraceChannel3 is Interaction
	//See DefaultEngine.ini
	//[/Script/Engine.CollisionProfile]
	//GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);

	InHandsSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("InHandsSkeletalMesh"));
	InHandsSkeletalMesh->SetupAttachment(GetMesh(), TEXT("ik_hand_gun"));
	InHandsSkeletalMesh->SetOnlyOwnerSee(false);
	//InHandsSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);

	StowedOnBackSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StowedOnBackSkeletalMesh"));
	StowedOnBackSkeletalMesh->SetupAttachment(GetMesh(), TEXT("StowedOnBackSocket"));
	StowedOnBackSkeletalMesh->SetOnlyOwnerSee(false);
	//StowedOnBackSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);

	//VitalityComponent->GetOnNoHealth().AddUObject(this, &AGameCharacterBase::Death);
}

void AGameCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGameCharacterBase, bIsProne, COND_SimulatedOnly);
}

void AGameCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	InventoryManager->AddInventory(PrimaryGunSlot);
	InventoryManager->AddInventory(SecondaryGunSlot);
	InventoryManager->AddInventory(InventoryComponent_Pockets);

	//Active slot is PrimarySlot by default
	ActiveSlot = PrimaryGunSlot;

	GetWorld()->GetTimerManager().SetTimer(InteractionSweepTimerHandle, this, &AGameCharacterBase::SweepInteractionLoop, 1.f / 30.f, true, 0.f);
}

void AGameCharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	if (BitmaskLib::IsBitSetPowerOfTwo(MovementToggleFlags, EMovementInputToggleFlags::SlowWalk))
	{
		PlayerInputComponent->BindAction("SlowWalk", IE_Pressed, this, &AGameCharacterBase::ToggleSlowWalk);
	}
	else
	{
		PlayerInputComponent->BindAction("SlowWalk", IE_Pressed, this, &AGameCharacterBase::StartSlowWalk);
		PlayerInputComponent->BindAction("SlowWalk", IE_Released, this, &AGameCharacterBase::EndSlowWalk);
	}

	if (BitmaskLib::IsBitSetPowerOfTwo(MovementToggleFlags, EMovementInputToggleFlags::Sprint))
	{
		PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AGameCharacterBase::ToggleSprint);
	}
	else
	{
		PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AGameCharacterBase::StartSprint);
		PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AGameCharacterBase::EndSprint);
	}

	if (BitmaskLib::IsBitSetPowerOfTwo(MovementToggleFlags, EMovementInputToggleFlags::Prone))
	{
		PlayerInputComponent->BindAction("Prone", IE_Pressed, this, &AGameCharacterBase::ToggleProne);
		//PlayerInputComponent->RemoveActionBinding();
	}
	else
	{
		PlayerInputComponent->BindAction("Prone", IE_Pressed, this, &AGameCharacterBase::StartProne);
		PlayerInputComponent->BindAction("Prone", IE_Released, this, &AGameCharacterBase::EndProne);
	}

	if (BitmaskLib::IsBitSetPowerOfTwo(MovementToggleFlags, EMovementInputToggleFlags::Crouch))
	{
		PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AGameCharacterBase::ToggleCrouch);
	}
	else
	{
		PlayerInputComponent->BindAction<CharInputBool>("Crouch", IE_Pressed, this, &AGameCharacterBase::Crouch, false);
		PlayerInputComponent->BindAction<CharInputBool>("Crouch", IE_Released, this, &AGameCharacterBase::UnCrouch, false);
	}

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AGameCharacterBase::Jump);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AGameCharacterBase::StartFireActive);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AGameCharacterBase::EndFireActive);

	//PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AGameCharacterBase::OnAction);

	//PlayerInputComponent->BindAction("DropItem", IE_Pressed, this, &AGameCharacterBase::DropItem);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGameCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGameCharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AGameCharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AGameCharacterBase::LookUpAtRate);
}

void AGameCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UInventoryComponent* AGameCharacterBase::GetInventoryComponent(ECharacterInventoryType type) const
{
	switch (type)
	{
	case ECharacterInventoryType::None:
		return nullptr;
		break;
	case ECharacterInventoryType::Pockets:
		return InventoryComponent_Pockets;
		break;
	case ECharacterInventoryType::Backpack:
		return InventoryComponent_Backpack;
		break;
	case ECharacterInventoryType::ChestRig:
		return InventoryComponent_ChestRig;
		break;
	default:
		return nullptr;
		break;
	}
}

UCustomCharacterMovementComponent* AGameCharacterBase::GetCharacterMovement() const
{
	return Cast<UCustomCharacterMovementComponent>(ACharacter::GetCharacterMovement());
}

void AGameCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

//bool AGameCharacterBase::IsFlagSet_ToggleInputMovement(EMovementInputToggleFlags flag)
//{
//	return MovementToggleFlags & static_cast<decltype(MovementToggleFlags)>(flag);
//}

//void AGameCharacterBase::SetFlag_ToogleInputMovement(EMovementInputToggleFlags flag, bool value)
//{
//	//1UL - 1 unsigned long, it's for int width independency
//	MovementToggleFlags ^= (-static_cast<long>(value) ^ MovementToggleFlags) & (1UL < static_cast<decltype(MovementToggleFlags)>(flag));
//}

void AGameCharacterBase::OnRep_IsProne()
{
	auto CharacterMovementCustom = GetCharacterMovement();

	if (CharacterMovementCustom)
	{
		if (bIsProne)
		{
			CharacterMovementCustom->bWantsToProne = true;
			CharacterMovementCustom->Prone(true);
		}
		else
		{
			CharacterMovementCustom->bWantsToProne = false;
			CharacterMovementCustom->UnProne(true);
		}
		CharacterMovementCustom->bNetworkUpdateReceived = true;
	}
}

void AGameCharacterBase::StartFireActive()
{
	bFireButtonDown = true;

	if(ActiveSlot->IsOccupied())
	{
		Cast<UWeaponGun>(ActiveSlot->GetItem())->StartFire();
	}
}

void AGameCharacterBase::EndFireActive()
{
	if(bFireButtonDown == true)
	{
		UWeaponGun* gun = Cast<UWeaponGun>(ActiveSlot->GetItem());

		if(gun != nullptr)
		{
			gun->EndFire();
		}
	}

	bFireButtonDown = false;
}

bool AGameCharacterBase::EquipGun(UItemBase* item)
{
	if(item->IsA<UWeaponGun>() == false)
	{
		return false;
	}

	bool result = false;

	if(item->GetOuterUpstreamInventory().GetObject()->IsA<UInvSpecialSlotComponent>())
	{
		//De-equip
		//result = item->GetOuterUpstreamInventory()->MoveItemToInventory(item, InventoryComponent);
		result = InventoryManager->MoveItemToInventory(item, EManagerInventoryType::InventoryComponent);
	}
	else
	{
		//Equip (move to SpecialSlot)
		result = item->GetOuterUpstreamInventory()->MoveItemToInventory(item, PrimaryGunSlot);
		if(result == false)
		{
			result = item->GetOuterUpstreamInventory()->MoveItemToInventory(item, SecondaryGunSlot);
		}
	}

	UpdateAllWeaponSlots();

	return result;
}

void AGameCharacterBase::UpdateAllWeaponSlots()
{
	UpdateWeaponSlot(PrimaryGunSlot);
	UpdateWeaponSlot(SecondaryGunSlot);
}

void AGameCharacterBase::UpdateWeaponSlot(UInvSpecialSlotComponent* slot)
{
	if (slot == nullptr)
	{
		return;
	}

	auto* weapon = Cast<UWeaponGun>(slot->GetItem());

	if (weapon == nullptr)
	{
		return;
	}

	weapon->EndFire();

	UWeaponBase::DestroySKStatic(weapon->GetWeaponRepresentation());

	if (slot == ActiveSlot)
	{
		InHandsSkeletalMesh = weapon->CreateSKWeaponRepresentation(GetMesh());
		InHandsSkeletalMesh->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, "ik_hand_gun");
	}
	else
	{
		StowedOnBackSkeletalMesh = weapon->CreateSKWeaponRepresentation(GetMesh());
		StowedOnBackSkeletalMesh->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, "StowedOnBackSocket");
	}
}

//Not used but has to be functional
//void AGameCharacterBase::UpdateWeaponSlots(UInvSpecialSlotComponent* new_active_slot)
//{
//	if (new_active_slot == nullptr)
//	{
//		return;
//	}
//
//	EndFire();
//
//	if (new_active_slot == ActiveSlot)
//	{
//		//do nothing
//	}
//	else
//	{
//		ActiveSlot = new_active_slot;
//
//		UWeaponBase::DestroySKStatic(InHandsSkeletalMesh);
//		UWeaponBase::DestroySKStatic(StowedOnBackSkeletalMesh);
//
//		auto active_weapon = Cast<UWeaponBase>(ActiveSlot->GetItem());
//
//		if (active_weapon != nullptr)
//		{
//			InHandsSkeletalMesh = active_weapon->CreateSKWeaponRepresentation(GetMesh());
//			InHandsSkeletalMesh->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, "ik_hand_gun");
//		}
//
//		UInvSpecialSlotComponent* stowed_slot;
//
//		if (ActiveSlot == PrimaryGunSlot)
//		{
//			stowed_slot = SecondaryGunSlot;
//		}
//		else
//		{
//			stowed_slot = PrimaryGunSlot;
//		}
//
//		auto stowed_weapon = Cast<UWeaponBase>(stowed_slot->GetItem());
//
//		if (stowed_weapon != nullptr)
//		{
//			StowedOnBackSkeletalMesh = stowed_weapon->CreateSKWeaponRepresentation(GetMesh());
//			StowedOnBackSkeletalMesh->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, "StowedOnBackSocket");
//		}
//	}
//}

void AGameCharacterBase::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		FRotator controller_rotation = GetControlRotation();
		controller_rotation.Pitch = 0;
		controller_rotation.Roll = 0;

		AddMovementInput(UKismetMathLibrary::GetForwardVector(controller_rotation), Value);
	}
}

void AGameCharacterBase::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		FRotator controller_rotation = GetControlRotation();
		controller_rotation.Pitch = 0;
		controller_rotation.Roll = 0;

		AddMovementInput(UKismetMathLibrary::GetRightVector(controller_rotation), Value);
	}
}

void AGameCharacterBase::TurnAtRate(float Rate)
{
	//calculate delta for this frame from the rate information
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGameCharacterBase::LookUpAtRate(float Rate)
{
	//calculate delta for this frame from the rate information
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGameCharacterBase::Jump()
{
	bool is_jump = true;

	if (bIsCrouched)
	{
		UnCrouch();
		is_jump = false;
	}
	if (bIsProne)
	{
		EndProne();
		is_jump = false;
	}
	
	if(is_jump)
	{
		constexpr float speed_when_min_z = 0;
		constexpr float speed_when_max_z = 1000;
		constexpr float min_z_velocity = 380;
		constexpr float max_z_velocity = 500;

		float computed_z_velocity = FMath::GetMappedRangeValueClamped(FVector2D{ speed_when_min_z, speed_when_max_z }, FVector2D{ min_z_velocity, max_z_velocity }, GetVelocity().Length());
		GetCharacterMovement()->JumpZVelocity = computed_z_velocity;
		ACharacter::Jump();
	}
}

void AGameCharacterBase::EndJump(const FHitResult& hit)
{
	bIsJumping = false;
}

void AGameCharacterBase::OnJumped_Implementation()
{
	bIsJumping = true;
}

void AGameCharacterBase::ToggleCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AGameCharacterBase::Crouch(bool bClientSimulation)
{
	if (GetCharacterMovement())
	{
		if (CanCrouch())
		{
			EndSlowWalk();
			EndSprint();
			EndProne();

			GetCharacterMovement()->bWantsToCrouch = true;
		}
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		else if (!GetCharacterMovement()->CanEverCrouch())
		{
			UE_LOG(LogCharacter, Log, TEXT("%s is trying to crouch, but crouching is disabled on this character! (check CharacterMovement NavAgentSettings)"), *GetName());
		}
#endif
	}
}

void AGameCharacterBase::UnCrouch(bool bClientSimulation)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bWantsToCrouch = false;
	}
}

void AGameCharacterBase::ToggleProne()
{
	if (bIsProne)
	{
		EndProne();
	}
	else
	{
		StartProne();
	}
}

void AGameCharacterBase::StartProne()
{
	if (GetCharacterMovement())
	{
		if (CanProne())
		{
			EndSprint();
			EndSlowWalk();
			UnCrouch();

			GetCharacterMovement()->bWantsToProne = true;
		}
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		else if (!GetCharacterMovement()->CanEverProne())
		{
			UE_LOG(LogCharacter, Log, TEXT("%s is trying to prone, but prone is disabled on this character! (check CharacterMovement NavAgentSettings)"), *GetName());
		}
#endif
	}
}
void AGameCharacterBase::EndProne()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bWantsToProne = false;
	}
}

bool AGameCharacterBase::CanSlowWalk() const
{
	return !bIsSlowWalking && !bIsCrouched && !bIsProne && GetCharacterMovement() && GetCharacterMovement()->CanEverSlowWalk() && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

bool AGameCharacterBase::CanSprint() const
{
	return !bIsSprinting && !bIsCrouched && !bIsProne && GetCharacterMovement() && GetCharacterMovement()->CanEverSprint() && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

bool AGameCharacterBase::CanProne() const
{
	return !bIsProne && GetCharacterMovement() && GetCharacterMovement()->CanEverProne() && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

bool AGameCharacterBase::CanCrouch() const
{
	return !bIsCrouched && GetCharacterMovement() && GetCharacterMovement()->CanEverCrouch() && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

void AGameCharacterBase::OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const AGameCharacterBase* DefaultChar = GetDefault<AGameCharacterBase>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->BaseTranslationOffset.Z + HalfHeightAdjust;
	}

	K2_OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AGameCharacterBase::OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const AGameCharacterBase* DefaultChar = GetDefault<AGameCharacterBase>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->BaseTranslationOffset.Z;
	}

	K2_OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

//void AGameCharacterBase::RecalculateProneEyeHeight()
//{
//	if (GetCharacterMovement() != nullptr)
//	{
//		constexpr float EyeHeightRatio = 0.8f;	// how high the character's eyes are, relative to the crouched height
//
//		CrouchedEyeHeight = GetCharacterMovement()->GetProneHalfHeight() * EyeHeightRatio;
//	}
//}

void AGameCharacterBase::ToggleSprint()
{
	//if(GetCharacterMovement()->IsCrouching() == false && bProneButtonDown == false && bWalkButtonDown == false)
	//{
	//	bSprintButtonDown = !bSprintButtonDown;

	//	if(bSprintButtonDown)
	//	{
	//		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	//		VitalityComponent->StartUsingStamina();
	//	}
	//	else
	//	{
	//		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	//		VitalityComponent->StopUsingStamina();
	//	}
	//}

	if (bIsSprinting)
	{
		EndSprint();
	}
	else
	{
		StartSprint();
	}
}

void AGameCharacterBase::StartSprint()
{
	if (GetCharacterMovement())
	{
		if (CanSprint())
		{
			EndSlowWalk();

			GetCharacterMovement()->StartSprint();
		}
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		else if (!GetCharacterMovement()->CanEverSprint())
		{
			UE_LOG(LogCharacter, Log, TEXT("%s is trying to sprint, but sprint is disabled on this character!"), *GetName());
		}
#endif
	}

	//if(GetCharacterMovement()->IsCrouching() == false && GetCharacterMovement()->IsProne() == false && GetCharacterMovement()->IsSlowWalking() == false)
	//{
	//	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	//	bSprintButtonDown = true;
	//	VitalityComponent->StartUsingStamina();
	//}
}

void AGameCharacterBase::EndSprint()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->EndSprint();
	}

	//if(GetCharacterMovement()->IsCrouching() == false && GetCharacterMovement()->IsProne() && bWalkButtonDown == false)
	//{
	//	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	//	bSprintButtonDown = false;
	//	VitalityComponent->StopUsingStamina();
	//}
}

void AGameCharacterBase::ToggleSlowWalk()
{
	if (bIsSlowWalking)
	{
		EndSlowWalk();
	}
	else
	{
		StartSlowWalk();
	}
}

void AGameCharacterBase::StartSlowWalk()
{
	if (GetCharacterMovement())
	{
		if (CanSlowWalk())
		{
			GetCharacterMovement()->StartSlowWalk();
		}
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		else if (!GetCharacterMovement()->CanEverSlowWalk())
		{
			UE_LOG(LogCharacter, Log, TEXT("%s is trying to slow walk, but slow walk is disabled on this character!"), *GetName());
		}
#endif
	}
}

void AGameCharacterBase::EndSlowWalk()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->EndSlowWalk();
	}
}

//void AGameCharacterBase::OnAction()
//{
//	AActor* actor_ptr = TraceInteraction().GetActor();
//
//	if(actor_ptr != nullptr && actor_ptr->Implements<UInteractionInterface>())
//	{
//		IInteractionInterface* interaction = Cast<IInteractionInterface>(actor_ptr);
//		interaction->Execute_OnInteract(actor_ptr, this, {});
//	}
//}

void AGameCharacterBase::UseItem(UItemBase* item)
{
	if(item != nullptr)
	{
		//item->Use(this);
		item->OnUse(this); //BP event
	}
}

//void AGameCharacterBase::DropItem()
//{
//	//TODO; placeholder; for now it drops the first item
//	if(InventoryComponent->GetItems().Num() != 0)
//	{
//		InventoryComponent->DropItemToWorld(InventoryComponent->GetItems().begin().Key());
//	}
//}


float AGameCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount < 0)
	{
		return 0;
	}
	if (EventInstigator == GetController())
	{
		//In theory, only AOE weapons can damage Character itself
		//This checkf will remain until non-AOE weapon damage is set up properly
		checkf(false, TEXT("Warning: Character damages itself. Is this intended behaviour?"));
	}

	VitalityComponent->ChangeHealth(-1 * DamageAmount);
	return DamageAmount;
}

void AGameCharacterBase::EnableRagdoll()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	if (InHandsSkeletalMesh != nullptr)
	{
		InHandsSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		InHandsSkeletalMesh->SetEnableGravity(true);
		InHandsSkeletalMesh->SetSimulatePhysics(true);
	}

}

void AGameCharacterBase::DisableRagdoll()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetEnableGravity(false);
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	if (InHandsSkeletalMesh != nullptr)
	{
		InHandsSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InHandsSkeletalMesh->SetEnableGravity(false);
		InHandsSkeletalMesh->SetSimulatePhysics(false);
	}
}

void AGameCharacterBase::SetDeathState(bool is_dead)
{
	if (is_dead)
	{
		//GetCharacterMovement()->DisableMovement();
		DisableInput(GetController<APlayerController>());

		EnableRagdoll();

		//PrimaryActorTick.bCanEverTick = false;
	}
	else
	{
		EnableInput(GetController<APlayerController>());
		//PrimaryActorTick.bCanEverTick = true;
		//VitalityComponent->Revive();
	}
}

void AGameCharacterBase::SetupMovementDefaults()
{
	//BaseTurnRate = 120.f;
	//BaseLookUpRate = 120.f;

	//GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeedJog;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->NavAgentProps.bCanJump = true;
	GetCharacterMovement()->NavAgentProps.bCanSwim = true;
	GetCharacterMovement()->NavAgentProps.bCanWalk = true;
	GetCharacterMovement()->NavAgentProps.bCanFly = false;
	GetCharacterMovement()->bCanProne = true;
	GetCharacterMovement()->bCanSprint = true;
	GetCharacterMovement()->bCanWalkOffLedges = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

	LandedDelegate.AddDynamic(this, &AGameCharacterBase::EndJump);
}

FHitResult AGameCharacterBase::SweepInteractionFromView(ECollisionChannel collision_channel)
{
	//DO NOT trace from FirstPersonCamera - it changes rotation unexpectedly (~90deg) when weapon is equipped
	//Even though actual view is not affected and mesh head moves very slightly

	const float radius = 2.25f;
	FVector start;
	FRotator view_point_rot;
	Cast<APlayerController>(GetController())->GetPlayerViewPoint(start, view_point_rot);

	const auto shape_rot = view_point_rot.Quaternion().GetUpVector().Rotation().Quaternion();
	const FVector end = start + view_point_rot.Quaternion().GetForwardVector() * InteractionRange;

	FCollisionQueryParams collision_params;
	collision_params.AddIgnoredActor(this);
	//collision_params.AddIgnoredComponent(GetComponentByClass<UMeshComponent>());

	FHitResult hit_result;

	GetWorld()->SweepSingleByChannel(hit_result, start, end, shape_rot, collision_channel, FCollisionShape::MakeSphere(radius), collision_params);

	//DrawDebugLine(GetWorld(), start, end, FColor::Cyan, false, 2, 0, 1.5f);
	return hit_result;
}

void AGameCharacterBase::SweepInteractionLoop()
{
	AActor* swept_actor = SweepInteractionFromView().GetActor();
	//auto hit_comp = SweepInteractionFromView().GetComponent();
	//UE_LOG(LogTemp, Warning, TEXT("Hit comp: %s"), hit_comp ? *hit_comp->GetName() : TEXT("none"));
	//UE_LOG(LogTemp, Warning, TEXT("Swept actor: %s"), swept_actor ? *swept_actor->GetName() : TEXT("none"));

	if (swept_actor != nullptr)
	{
		if (swept_actor != interaction_actor)
		{
			interaction_actor = swept_actor;
			CloseInteractionUI();
		}

		if (swept_actor->Implements<UInteractionInterface>())
		{
			IInteractionInterface* interaction_interface = Cast<IInteractionInterface>(swept_actor);

			if (interaction_interface->Execute_IsInteractible(swept_actor))
			{
				interaction_interface->Execute_DrawInteractionOutline(swept_actor);
				OpenInteractionUI();

				//ConstructorHelpers::FClassFinder<UUserWidget> widget_class(TEXT("/Game/Blueprints/UI/WBP_InteractionMenu"));
				//UUserWidget* widget = CreateWidget(this, widget_class.Class);
			}
		}
	}
	else
	{
		CloseInteractionUI();
	}
}

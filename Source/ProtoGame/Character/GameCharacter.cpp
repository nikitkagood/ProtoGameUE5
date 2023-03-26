// Copyright Epic Games, Inc. All Rights Reserved.

//custom
#include "GameCharacter.h"
#include "Item/Projectile.h"
#include "Item/ItemBase.h"
#include "Item/ItemActor.h"
#include "Item/WeaponGun.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InvSpecialSlot.h"
#include "Components/VitalityComponent.h"
#include "Components/RPGStatsComponent.h"

//engine
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

//debug
#include "DrawDebugHelpers.h"

AGameCharacterBase::AGameCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	GetCapsuleComponent()->SetVisibility(false, false);
	GetCapsuleComponent()->SetHiddenInGame(false, false);

	SetupMovementDefaults();

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	VitalityComponent = CreateDefaultSubobject<UVitalityComponent>(TEXT("VitalityComponent"));
	RPGStatsComponent = CreateDefaultSubobject<URPGStatsComponent>(TEXT("RPGStatsComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	PrimaryGunSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Primary gun slot component"));
	SecondaryGunSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Secondary gun slot component"));
	ActiveSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Active slot component"));

	InHandsSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("InHandsSkeletalMesh"));
	InHandsSkeletalMesh->SetupAttachment(GetMesh(), TEXT("ik_hand_gun"));
	InHandsSkeletalMesh->SetOnlyOwnerSee(false);

	StowedOnBackSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StowedOnBackSkeletalMesh"));
	StowedOnBackSkeletalMesh->SetupAttachment(GetMesh(), TEXT("StowedOnBackSocket"));
	StowedOnBackSkeletalMesh->SetOnlyOwnerSee(false);

	//VitalityComponent->GetOnNoHealth().AddUObject(this, &AGameCharacterBase::Death);
}

void AGameCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//Active slot is PrimarySlot by default
	ActiveSlot = PrimaryGunSlot;
}

void AGameCharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AGameCharacterBase::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AGameCharacterBase::EndCrouch);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AGameCharacterBase::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AGameCharacterBase::EndSprint);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AGameCharacterBase::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AGameCharacterBase::EndFire);

	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AGameCharacterBase::OnAction);

	PlayerInputComponent->BindAction("DropItem", IE_Pressed, this, &AGameCharacterBase::DropItem);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGameCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGameCharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AGameCharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AGameCharacterBase::LookUpAtRate);
}

//void AGameCharacterBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

void AGameCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGameCharacterBase::StartFire()
{
	bFireButtonDown = true;

	if(ActiveSlot->IsOccupied())
	{
		Cast<UWeaponGun>(ActiveSlot->GetItem())->StartFire();
	}
}

void AGameCharacterBase::EndFire()
{
	if(bFireButtonDown == true)
	{
		UWeaponGun* temp = Cast<UWeaponGun>(ActiveSlot->GetItem());
		
		if(temp != nullptr)
		{
			temp->EndFire();
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
		result = item->GetOuterUpstreamInventory()->MoveItemToInventory(item, InventoryComponent);
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

	UpdateWeaponSlots();

	return result;
}

void AGameCharacterBase::UpdateWeaponSlots()
{
	UWeaponBase::DestroySKStatic(InHandsSkeletalMesh);
	UWeaponBase::DestroySKStatic(StowedOnBackSkeletalMesh);

	auto* primary_slot_weapon = Cast<UWeaponBase>(PrimaryGunSlot->GetItem());
	auto* secondary_slot_weapon = Cast<UWeaponBase>(SecondaryGunSlot->GetItem());

	if(ActiveSlot == PrimaryGunSlot)
	{
		if(primary_slot_weapon != nullptr)
		{
			InHandsSkeletalMesh = primary_slot_weapon->CreateSKWeaponRepresentation(GetMesh());
			InHandsSkeletalMesh->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, "ik_hand_gun");
		}

		if(secondary_slot_weapon != nullptr)
		{
			StowedOnBackSkeletalMesh = secondary_slot_weapon->CreateSKWeaponRepresentation(GetMesh());;
			StowedOnBackSkeletalMesh->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, "StowedOnBackSocket");
		}

	}
	else //ActiveSlot == SecondaryGunSlot 
	{
		if(secondary_slot_weapon != nullptr)
		{
			InHandsSkeletalMesh = secondary_slot_weapon->CreateSKWeaponRepresentation(GetMesh());;
			InHandsSkeletalMesh->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, "ik_hand_gun");
		}

		if(primary_slot_weapon != nullptr)
		{
			StowedOnBackSkeletalMesh = primary_slot_weapon->CreateSKWeaponRepresentation(GetMesh());;
			StowedOnBackSkeletalMesh->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, "StowedOnBackSocket");
		}
	}

	//auto* active_slot_weapon = Cast<UWeaponBase>(ActiveSlot->GetItem());

	////Check active slot
	//if(active_slot_weapon != nullptr)
	//{
	//	InHandsSkeletalMesh = active_slot_weapon->CreateWeaponSkeletalMeshComp(GetMesh());
	//	InHandsSkeletalMesh->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, "ik_hand_gun");
	//}
	//else
	//{
	//	DestroyComponentAndChildren(InHandsSkeletalMesh);
	//}

	////Check non-active slot
	//UInvSpecialSlotComponent* non_active_slot;
	//if(ActiveSlot == PrimaryGunSlot)
	//{
	//	non_active_slot = SecondaryGunSlot;
	//}
	//else //ActiveSlot == SecondaryGunSlot
	//{
	//	non_active_slot = PrimaryGunSlot;
	//}

	//if(non_active_slot->IsOccupied())
	//{
	//	StowedOnBackSkeletalMesh = Cast<UWeaponBase>(non_active_slot->GetItem())->CreateWeaponSkeletalMeshComp(GetMesh());
	//	StowedOnBackSkeletalMesh->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, "StowedOnBackSocket");
	//}
	//else
	//{
	//	DestroyComponentAndChildren(StowedOnBackSkeletalMesh);
	//}


}

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
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGameCharacterBase::LookUpAtRate(float Rate)
{
	//calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGameCharacterBase::ToggleCrouch()
{
	//TODO: Untested

	bCrouchButtonDown = !bCrouchButtonDown;

	if(bCrouchButtonDown)
	{
		ACharacter::Crouch();
		GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	}
	else
	{
		ACharacter::UnCrouch();
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	}

	bSprintButtonDown = false;
	bProneButtonDown = false;
	bWalkButtonDown = false;
}

void AGameCharacterBase::StartCrouch()
{
	ACharacter::Crouch();
	bCrouchButtonDown = true;

	bSprintButtonDown = false;
	bProneButtonDown = false;
	bWalkButtonDown = false;

	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
}

void AGameCharacterBase::EndCrouch()
{
	if(bProneButtonDown == false)
	{
		ACharacter::UnCrouch();
		bCrouchButtonDown = false;
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	}
}

void AGameCharacterBase::ToggleProne()
{
}

void AGameCharacterBase::StartProne()
{
}

void AGameCharacterBase::EndProne()
{
}

void AGameCharacterBase::ToggleSprint()
{
	//TODO: Untested

	if(GetCharacterMovement()->IsCrouching() == false && bProneButtonDown == false && bWalkButtonDown == false)
	{
		bSprintButtonDown = !bSprintButtonDown;

		if(bSprintButtonDown)
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
			VitalityComponent->StartUsingStamina();
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
			VitalityComponent->StopUsingStamina();
		}
	}
}

void AGameCharacterBase::StartSprint()
{
	if(GetCharacterMovement()->IsCrouching() == false && bProneButtonDown == false && bWalkButtonDown == false)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bSprintButtonDown = true;
		VitalityComponent->StartUsingStamina();
	}
}

void AGameCharacterBase::EndSprint()
{
	if(GetCharacterMovement()->IsCrouching() == false && bProneButtonDown == false && bWalkButtonDown == false)
	{
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
		bSprintButtonDown = false;
		VitalityComponent->StopUsingStamina();
	}
}

void AGameCharacterBase::ToggleWalk()
{
}

void AGameCharacterBase::StartWalk()
{
}

void AGameCharacterBase::EndWalk()
{
}

void AGameCharacterBase::OnAction()
{
	AActor* actor_ptr = GetInteractionInfo().GetActor();

	if(actor_ptr != nullptr)
	{
		auto name = actor_ptr->GetName();

		if(actor_ptr->Implements<UInteractionInterface>())
		{
			IInteractionInterface* interaction_interface = Cast<IInteractionInterface>(actor_ptr);

			//Cast to BP objects that implement the interface returns null, thus null -> call BP function
			if(interaction_interface != nullptr)
			{
				interaction_interface->OnInteract(this);
			}
			else
			{
				interaction_interface->Execute_OnInteractBP(actor_ptr, this);
			}

		}
	}
}

void AGameCharacterBase::UseItem(UItemBase* item)
{
	if(item != nullptr)
	{
		//item->Use(this);
		item->OnUse(this); //BP event
	}
}

void AGameCharacterBase::DropItem()
{
	//TODO; placeholder; for now it drops the first item
	if(InventoryComponent->GetItems().Num() != 0)
	{
		InventoryComponent->DropItemToWorld(InventoryComponent->GetItems().begin().Key());
	}
}


float AGameCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	VitalityComponent->ChangeHealth(-1 * DamageAmount);
	return DamageAmount;
}

void AGameCharacterBase::EnableRagdoll()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	//Mesh1P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//Mesh1P->SetSimulatePhysics(true);
	//Mesh1P->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
}

void AGameCharacterBase::DisableRagdoll()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	//Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Mesh1P->SetSimulatePhysics(false);
	//Mesh1P->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void AGameCharacterBase::Death()
{
	//GetCharacterMovement()->DisableMovement();
	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));


	EnableRagdoll();

	PrimaryActorTick.bCanEverTick = false;
}

void AGameCharacterBase::SetupMovementDefaults()
{
	// set our turn rates for input
	BaseTurnRate = 120.f;
	BaseLookUpRate = 120.f;

	bJumpButtonDown = false;
	bCrouchButtonDown = false;
	bProneButtonDown = false;
	bWalkButtonDown = false;
	bSprintButtonDown = false;
	bAimButtonDown = false;
	bFireButtonDown = false;
	JogSpeed = 500.f;
	WalkSpeed = 250.f;
	SprintSpeed = 1000.f;
	CrouchSpeed = 200.f;
	ProneSpeed = 120.f;
	AimDownSightsSpeed = 250.f;
	//JumpZVelocity = 420.f;

	//TODO: different JumpZVelocity when standing or moving moving

	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->NavAgentProps.bCanJump = true;
	GetCharacterMovement()->NavAgentProps.bCanSwim = true;
	GetCharacterMovement()->NavAgentProps.bCanWalk = true;
	GetCharacterMovement()->NavAgentProps.bCanFly = false;
	GetCharacterMovement()->bCanWalkOffLedges = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
}

FHitResult AGameCharacterBase::GetInteractionInfo()
{
	constexpr float interaction_range = 175.f;
	float radius = 2.1f;
	FVector start = FirstPersonCameraComponent->GetComponentLocation();
	FVector end = start + FirstPersonCameraComponent->GetForwardVector() * interaction_range;

	FCollisionQueryParams collision_params;
	collision_params.AddIgnoredActor(this);

	FHitResult hit_result;

	//DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 3, 0, 5);
	//GetWorld()->LineTraceSingleByChannel(hit_result, start, end, ECollisionChannel::ECC_Visibility, collision_params);
	GetWorld()->SweepSingleByChannel(hit_result, start, end, {}, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(radius), collision_params);
	DrawDebugSphere(GetWorld(), start, radius, 8, FColor::Green, false, 3, 0, 0.15);
	DrawDebugSphere(GetWorld(), start + FirstPersonCameraComponent->GetForwardVector() * (interaction_range / 4), radius, 8, FColor::Green, false, 3, 0, 0.15);
	DrawDebugSphere(GetWorld(), start + FirstPersonCameraComponent->GetForwardVector() * (interaction_range / 2), radius, 8, FColor::Green, false, 3, 0, 0.15);
	DrawDebugSphere(GetWorld(), start + FirstPersonCameraComponent->GetForwardVector() * (interaction_range / 1.5), radius, 8, FColor::Green, false, 3, 0, 0.15);
	DrawDebugSphere(GetWorld(), start + FirstPersonCameraComponent->GetForwardVector() * (interaction_range / 1.25), radius, 8, FColor::Green, false, 3, 0, 0.15);
	DrawDebugSphere(GetWorld(), end, radius, 8, FColor::Green, false, 3, 0, 0.15);

	return hit_result;
}

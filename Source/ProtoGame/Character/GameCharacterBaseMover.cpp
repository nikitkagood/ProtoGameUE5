// Nikita Belov, All rights reserved


#include "Character/GameCharacterBaseMover.h"

//custom
#include "Item/Projectile.h"
#include "Item/ItemBase.h"
#include "Item/InventoryItem.h"
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
#include "DefaultMovementSet/CharacterMoverComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"


//debug
#include "DrawDebugHelpers.h"

//DEFINE_LOG_CATEGORY_STATIC(LogCharacter, Log, All);

AGameCharacterBaseMover::AGameCharacterBaseMover(const class FObjectInitializer& ObjectInitializer)
//Example on how to re-initialize subobject in child class:   : Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision Comp Capsule"));
	Cast<UCapsuleComponent>(CollisionComponent)->InitCapsuleSize(30.f, 90.f); //about right size for default human
	CollisionComponent->SetVisibility(true, false);
	CollisionComponent->SetHiddenInGame(true, false);
	CollisionComponent->SetCollisionProfileName("Pawn");
	CollisionComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;
	SetRootComponent(CollisionComponent);

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
	CharacterMesh->SetupAttachment(CollisionComponent);
	CharacterMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	CharacterMesh->SetOnlyOwnerSee(false);

	//SetupMovementDefaults();

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCharacterMesh());
	//FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	//FirstPersonCameraComponent->AttachToComponent(GetCharacterMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	SpringArm_FPCam = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm_FPCam"));
	SpringArm_FPCam->SetupAttachment(GetCharacterMesh());


	VitalityComponent = CreateDefaultSubobject<UVitalityComponent>(TEXT("Vitality Component"));
	RPGStatsComponent = CreateDefaultSubobject<URPGStatsComponent>(TEXT("RPG Stats Component"));

	InventoryManager = CreateDefaultSubobject<UInventoryManager>(TEXT("Inventory Manager"));

	//InventoryComponent_Pockets = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent_Pockets"));
	//InventoryItemSlot_Backpack = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("InventoryItemSlot_Backpack"));
	//InventoryItemSlot_ChestRig = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("InventoryItemSlot_ChestRig"));

	//PrimaryGunSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Primary gun slot component"));
	//SecondaryGunSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Secondary gun slot component"));
	//It is created so it is visibile in Blueprints, otherwise it's redundant
	ActiveSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Active slot component"));

	//ECC_GameTraceChannel3 is Interaction
	//See DefaultEngine.ini
	//[/Script/Engine.CollisionProfile]
	//GetCharacterMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);

	InHandsSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("InHands SkeletalMesh"));
	InHandsSkeletalMesh->SetupAttachment(GetCharacterMesh(), TEXT("ik_hand_gun"));
	InHandsSkeletalMesh->SetOnlyOwnerSee(false);

	OnBackSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("OnBack SkeletalMesh"));
	OnBackSkeletalMesh->SetupAttachment(GetCharacterMesh(), TEXT("OnBack SkeletalMesh"));
	OnBackSkeletalMesh->SetOnlyOwnerSee(false);

	//BackpackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Backpack Mesh"));
	//BackpackMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//BackpackMesh->SetupAttachment(GetCharacterMesh(), TEXT("BackpackSocket"));
	//BackpackMesh->SetOnlyOwnerSee(false);

	//ChestRigMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chest Rig Mesh"));
	//ChestRigMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//ChestRigMesh->SetupAttachment(GetCharacterMesh(), TEXT("spine_03"));
	//ChestRigMesh->SetOnlyOwnerSee(false);

	//VitalityComponent->GetOnNoHealth().AddUObject(this, &AGameCharacterBase::Death);
}

void AGameCharacterBaseMover::BeginPlay()
{
	Super::BeginPlay();

	//InventoryManager->AddExistingInventory(PrimaryGunSlot);
	//InventoryManager->AddExistingInventory(SecondaryGunSlot);
	//InventoryManager->AddExistingInventory(InventoryComponent_Pockets);
	//InventoryManager->AddExistingInventory(InventoryItemSlot_Backpack);
	//InventoryManager->AddExistingInventory(InventoryItemSlot_ChestRig);

	//Active slot is PrimarySlot by default
	//ActiveSlot = PrimaryGunSlot;

	//Sweep in a loop for Interaction (outline primarily) to work
	//TODO: maybe there is a better way
	GetWorld()->GetTimerManager().SetTimer(InteractionSweepTimerHandle, this, &AGameCharacterBaseMover::SweepInteractionLoop, 1.f / 30.f, true, 0.f);
}

void AGameCharacterBaseMover::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AGameCharacterBaseMover::Tick(float DeltaTime)
{

}


//UInventoryComponent* AGameCharacterBaseMover::GetInventoryComponent(ECharacterInventoryType type) const
//{
//	UInventoryItem* temp_inventory_item;
//
//	switch (type)
//	{
//	case ECharacterInventoryType::None:
//		return nullptr;
//		break;
//	case ECharacterInventoryType::Pockets:
//		return InventoryComponent_Pockets;
//		break;
//	case ECharacterInventoryType::Backpack:
//		temp_inventory_item = Cast<UInventoryItem>(InventoryItemSlot_Backpack->GetItem());
//
//		if (temp_inventory_item == nullptr)
//		{
//			return nullptr;
//		}
//		else
//		{
//			return temp_inventory_item->GetInventoryComponent();
//		}
//		break;
//	case ECharacterInventoryType::ChestRig:
//		temp_inventory_item = Cast<UInventoryItem>(InventoryItemSlot_Backpack->GetItem());
//
//		if (temp_inventory_item == nullptr)
//		{
//			return nullptr;
//		}
//		else
//		{
//			return temp_inventory_item->GetInventoryComponent();
//		}
//		break;
//	default:
//		return nullptr;
//		break;
//	}
//}


bool AGameCharacterBaseMover::EquipGun(UItemBase* item)
{
	if (!IsValid(GetCharacterMesh()))
	{
		checkf(false, TEXT("AGameCharacterBaseMover::EquipGun: Main Mesh not valid"));
		return false;
	}

	if (item->IsA<UWeaponGun>() == false)
	{
		return false;
	}

	bool result = false;

	if (item->GetOuterUpstreamInventory().GetObject()->IsA<UInvSpecialSlotComponent>())
	{
		//De-equip
		//result = item->GetOuterUpstreamInventory()->MoveItemToInventory(item, InventoryComponent);
		//result = InventoryManager->MoveItemToInventory(item, EManagerInventoryType::InventoryComponent);
	}
	else
	{
		//Equip (move to SpecialSlot)
		//result = item->GetOuterUpstreamInventory()->MoveItemToInventory(item, PrimaryGunSlot);
		if (result == false)
		{
			//result = item->GetOuterUpstreamInventory()->MoveItemToInventory(item, SecondaryGunSlot);
		}
	}

	//UpdateAllWeaponSlots();

	return result;
}

void AGameCharacterBaseMover::UseItem(UItemBase* item)
{
	if (item != nullptr)
	{
		//item->Use(this);
		item->OnUse(this); //BP event
	}
}


void AGameCharacterBaseMover::ToggleSprint()
{
	//if (bIsSprinting)
	//{
	//	EndSprint();
	//}
	//else
	//{
	//	StartSprint();
	//}
}

void AGameCharacterBaseMover::StartSprint()
{
	//if (GetCharacterMovement())
	//{
	//	if (CanSprint())
	//	{
	//		EndSlowWalk();

	//		GetCharacterMovement()->StartSprint();
	//	}
	//	else if (!GetCharacterMovement()->CanEverSprint())
	//	{
	//		UE_LOG(LogCharacter, Log, TEXT("%s is trying to sprint, but sprint is disabled on this character!"), *GetName());
	//	}
	//}


}

void AGameCharacterBaseMover::EndSprint()
{
	//if (GetCharacterMovement())
	//{
	//	GetCharacterMovement()->EndSprint();
	//}
}



FHitResult AGameCharacterBaseMover::SweepInteractionFromView(ECollisionChannel collision_channel)
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

void AGameCharacterBaseMover::SweepInteractionLoop()
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
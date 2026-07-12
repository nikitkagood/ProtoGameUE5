// Nikita Belov, All rights reserved


#include "Character/GameCharacterBaseMover.h"

//custom
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
#include "Components/ShapeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "DefaultMovementSet/CharacterMoverComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "TimerManager.h"


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
	FirstPersonCameraComponent->SetupAttachment(GetMesh());
	//FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	//FirstPersonCameraComponent->AttachToComponent(GetCharacterMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	SpringArm_FPCam = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm_FPCam"));
	SpringArm_FPCam->SetupAttachment(GetMesh());


	VitalityComponent = CreateDefaultSubobject<UVitalityComponent>(TEXT("Vitality Component"));
	RPGStatsComponent = CreateDefaultSubobject<URPGStatsComponent>(TEXT("RPG Stats Component"));

	InventoryManager = CreateDefaultSubobject<UInventoryManager>(TEXT("Inventory Manager"));

	//InventoryComponent_Pockets = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent_Pockets"));
	//InventoryItemSlot_Backpack = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("InventoryItemSlot_Backpack"));
	//InventoryItemSlot_ChestRig = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("InventoryItemSlot_ChestRig"));

	//PrimaryGunSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Primary gun slot component"));
	//SecondaryGunSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Secondary gun slot component"));
	//It is created so it is visibile in Blueprints, otherwise it's redundant
	//ActiveSlot = CreateDefaultSubobject<UInvSpecialSlotComponent>(TEXT("Active slot component"));

	//ECC_GameTraceChannel3 is Interaction
	//See DefaultEngine.ini
	//[/Script/Engine.CollisionProfile]
	//GetCharacterMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);

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
}

void AGameCharacterBaseMover::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AGameCharacterBaseMover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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


//bool AGameCharacterBaseMover::EquipGun(UItemBase* item)
//{
//	if (!IsValid(GetMesh()))
//	{
//		checkf(false, TEXT("AGameCharacterBaseMover::EquipGun: Main Mesh not valid"));
//		return false;
//	}
//
//	if (item->IsA<UWeaponGun>() == false)
//	{
//		return false;
//	}
//
//	bool result = false;
//
//	if (item->GetOuterUpstreamInventory().GetObject()->IsA<UInvSpecialSlotComponent>())
//	{
//		//De-equip
//		//result = item->GetOuterUpstreamInventory()->MoveItemToInventory(item, InventoryComponent);
//		//result = InventoryManager->MoveItemToInventory(item, EManagerInventoryType::InventoryComponent);
//	}
//	else
//	{
//		//Equip (move to SpecialSlot)
//		//result = item->GetOuterUpstreamInventory()->MoveItemToInventory(item, PrimaryGunSlot);
//		if (result == false)
//		{
//			//result = item->GetOuterUpstreamInventory()->MoveItemToInventory(item, SecondaryGunSlot);
//		}
//	}
//
//	//UpdateAllWeaponSlots();
//
//	return result;
//}

void AGameCharacterBaseMover::UseItem(UItemBase* item)
{
	if (item != nullptr)
	{
		item->OnUse(this); //BP event
	}
}

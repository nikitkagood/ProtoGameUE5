// Nikita Belov, All rights reserved

#include "Item/WeaponGun.h"
#include "Item/WeaponGunActor.h"
#include "Item/Projectile.h"
#include "ItemActor.h"

#include "Item/WeaponAttachment.h"
#include "Item/WeaponAttachmentMagazine.h"
#include "Animation/GunAnimInstance.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/GameplayStatics.h"

UWeaponGun::UWeaponGun()
{
	if(weapon_gun_info.FireModesAvailable.Num() != 0)
	{
		weapon_gun_info.FireMode = weapon_gun_info.FireModesAvailable[0];
	}
}

//bool UWeaponGun::Initialize(FDataTableRowHandle handle)
//{ 
	//auto* ptr_row = handle.GetRow<FWeaponTable>("SetProperties_WeaponGun");

	//if(ptr_row != nullptr)
	//{
	//	inventory_item_info = ptr_row->inventory_item_info;
	//	weapon_gun_info = ptr_row->weapon_gun_info;

	//	for(auto& i : ptr_row->attachment_slots)
	//	{
	//		AddAttachmentSlot(i);
	//	}

	//	return true;
	//}

	//return false;
//} 

void UWeaponGun::StartFire()
{
	if(weapon_gun_info.bFunctional)
	{
		Firing = true;

		const float rate = 1 / (weapon_gun_info.FireRate / 60);

		switch(weapon_gun_info.FireMode)
		{
		case EWeaponFireMode::Single:
			OnFire();
			break;
		case EWeaponFireMode::Auto:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UWeaponGun::OnFire, rate, true, 0.f);
			break;
		case EWeaponFireMode::AutoSlow:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UWeaponGun::OnFire, rate / 2, true, 0.f);
			break;
		case EWeaponFireMode::TwoRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UWeaponGun::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UWeaponGun::EndFire, rate, false, rate * 2);
			break;
		case EWeaponFireMode::ThreeRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UWeaponGun::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UWeaponGun::EndFire, rate, false, rate * 3);
			break;
		case EWeaponFireMode::FourRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UWeaponGun::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UWeaponGun::EndFire, rate, false, rate * 4);
			break;
		case EWeaponFireMode::FiveRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UWeaponGun::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UWeaponGun::EndFire, rate, false, rate * 5);
			break;
		case EWeaponFireMode::SpecialMode1:
			OnFire();
			break;
		case EWeaponFireMode::SpecialMode2:
			OnFire();
			break;
		case EWeaponFireMode::SpecialMode3:
			OnFire();
			break;
		default:
			//also Safe
			break;
		}
	}
}

void UWeaponGun::EndFire()
{
	if(this != nullptr && OnFireTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(OnFireTimerHandle);
	}

	Firing = false;
}

void UWeaponGun::PrintWeaponStats()
{
	//auto* mag = Cast<UWeaponAttachmentMagazine>(attachment_slots[MagazineAttachmentIdx].Value);

	//FString msg { "Ammo in mag: " + FString::FromInt(mag->GetAmmoLeft())};
	//UKismetSystemLibrary::PrintString(GetWorld(), msg, true, true, FLinearColor(206, 245, 66), 2);
}

void UWeaponGun::OnDestroy()
{
	if (WeaponGunActorCached.IsValid())
	{
		WeaponGunActorCached->Destroy();
	}

	Super::OnDestroy();
}

AWeaponGunActor* UWeaponGun::GetWeaponGunActor()
{
	AWeaponGunActor* from_outer = Cast<AWeaponGunActor>(GetOuterItemActor());

	if (IsValid(from_outer))
	{
		return from_outer;
	}

	return WeaponGunActorCached.Get();
}

void UWeaponGun::OnFire()
{
	USkeletalMeshComponent* sk_comp = nullptr;
	auto gun_actor = GetWeaponGunActor();

	if (IsValid(gun_actor))
	{
		sk_comp = gun_actor->GetSkeletalMeshComp();
	}

	if(sk_comp == nullptr || weapon_gun_info.bFunctional == false)
	{
		return;
	}

	if(weapon_gun_info.bHasChamber)
	{
		if(weapon_gun_info.Chamber != nullptr)
		{
			if(weapon_gun_info.Chamber->GetProjectileClass() == nullptr)
			{
				checkf(false, TEXT("Projectile isn't assigned, can't fire."));
				return;
			}

			gun_actor->OnFire();

			LoadAmmoIntoChamberFromMag();
		}

	}
	//TODO
	else //if no chamber 
	{
		//FActorSpawnParameters ActorSpawnParams;
		//ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		////TODO: Temporary
		//World->SpawnActor<AProjectile>(weapon_gun_info.AmmoType, WeaponSkeletalMeshComp->GetSocketLocation("b_gun_muzzleflash"), WeaponSkeletalMeshComp->GetSocketRotation("b_gun_muzzleflash"), ActorSpawnParams);

		//SpawnMuzzleFlash();

		//if(weapon_gun_info.FireSound != nullptr)
		//{
		//	const FVector muzzle_sound_offset = {-5.f, 0.f, 0.f}; //play sound a bit behind the muzzle end
		//	UGameplayStatics::PlaySoundAtLocation(this, weapon_gun_info.FireSound, WeaponSkeletalMeshComp->GetSocketLocation("b_gun_muzzleflash") - muzzle_sound_offset);
		//}
	}
}


bool UWeaponGun::AddAttachmentMesh(USkeletalMeshComponent* sk_comp, UWeaponAttachment* attachment)
{
	UStaticMeshComponent* sm_comp = NewObject<UStaticMeshComponent>(sk_comp, UStaticMeshComponent::StaticClass(), attachment->GetSocketName());
	//We use GetSocketName to name SM_component aswell so we can find it later if needed

	if(sm_comp == nullptr)
	{
		checkf(false, TEXT("ERROR: AttachAttachmentMesh: Failed to create SM component"));
		return false;
	}

	//Set params for attachment mesh.
	//Beware that wrong params will cause this mesh to collide with character holding a weapon
	//or attachments to collide with the weapon, which will result in uncontrollable flight
	sm_comp->SetCollisionProfileName("Item_IgnoreItem");
	sm_comp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	sm_comp->SetSimulatePhysics(false);
	sm_comp->SetGenerateOverlapEvents(false);

	sm_comp->RegisterComponent();

	if(sm_comp->AttachToComponent(sk_comp, {EAttachmentRule::SnapToTarget, true}, attachment->GetSocketName()) == false)
	{
		checkf(false, TEXT("ERROR: AttachAttachmentMesh: Failed to attach to component"));
		return false;
	}

	//TODO: There is a rare nullptr bug here. When switching weapons maybe.

	sm_comp->SetStaticMesh(attachment->GetStaticMeshFromItemActorCDO());

	if(sm_comp->GetStaticMesh() == nullptr)
	{
		checkf(false, TEXT("ERROR: AttachAttachmentMesh: Failed to set static mesh"));
		return false;
	}

 	return true;
}


void UWeaponGun::RemoveAttachmentMesh(USkeletalMeshComponent* sk_comp, UWeaponAttachment* attachment)
{
	TArray<USceneComponent*> children;
	sk_comp->GetChildrenComponents(true, children);

	for(auto& i : children)
	{
		if(i != nullptr && i->GetName() == attachment->GetSocketName().ToString())
		{
			i->DestroyComponent();
		}
	}
}

void UWeaponGun::RemoveAllAttachmentMeshes(USkeletalMeshComponent* sk_comp)
{
	TArray<USceneComponent*> children;
	sk_comp->GetChildrenComponents(true, children);

	for(auto& i : children)
	{
		//sometimes this check is actually needed
		if(i != nullptr)
		{
			i->DestroyComponent();
		}
	}
}

bool UWeaponGun::AddAttachment(UWeaponAttachment* attachment)
{
	//Find free attachment slot
	TArray<UWeaponAttachment*> res;
	attachment_slots.MultiFind(attachment->GetAttachmentSlot(), res, true);

	for (auto i : res)
	{
		//if a slot is empty:
		if (i == nullptr)
		{
			i = attachment;
		}
	}

	//if we have ItemActor spawned, then attach to it
	if (GetOuterItemActor() != nullptr)
	{
		if (AddAttachmentMesh(GetOuterItemActor()->GetSkeletalMeshComp(), attachment) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AddAttachment: Failed to attach mesh to ItemActor"));
			check(false);
			return false;
		}
	}

	attachment->SetOuterWeapon(this);


	return true;
}

//bool UWeaponGun::AddAttachmentTo(UWeaponAttachment* attachment, FAttachmentSlot* slot)
//{
//	FindPair?
// 
//	TArray<UWeaponAttachment*> res;
//	attachment_slots.MultiFind(*slot, res, true);
//
//	return false;
//
//}

AItemActor* UWeaponGun::SpawnItemActor(const FVector& location, const FRotator& rotation, const FItemActorSpawnParameters& spawn_parameters)
{
	if (spawn_parameters.MoveOwnershipItemObject && WeaponGunActorCached.IsValid())
	{
		checkf(true, TEXT("Trying to spawn ItemActor and change ownership while WeaponGunActorCached still valid"));
	}

	AItemActor* weapon_item_actor = AItemActor::StaticCreateObject(GetWorld(), ItemActorClass, this, spawn_parameters, location, rotation);

	if(weapon_item_actor != nullptr)
	{
		WeaponGunActorCached = Cast<AWeaponGunActor>(weapon_item_actor);

		AddAllAttachmentMeshes(weapon_item_actor->GetSkeletalMeshComp());
	}

	return weapon_item_actor;
}

//AItemActor* UWeaponGun::SpawnItemActorVisualOnly(const FVector& location, const FRotator& rotation)
//{
//	AItemActor* weapon_item_actor = AItemActor::StaticCreateObjectVisualOnly(GetWorld(), ItemActorClass, this, location, rotation);
//
//	if(weapon_item_actor != nullptr)
//	{
//		AddAllAttachmentMeshes(weapon_item_actor->GetSkeletalMeshComp());
//	}
//
//	return weapon_item_actor;
//}

bool UWeaponGun::AddAttachmentSlot(const FAttachmentSlot& slot)
{
	auto current_attachment = attachment_slots.Emplace(slot, nullptr);

	return true;
}

void UWeaponGun::SetupAnimInstance(USkeletalMeshComponent* sk_comp)
{
	auto* anim_instance = Cast<UGunAnimInstance>(sk_comp->GetAnimInstance());

	if (!IsValid(anim_instance))
	{
		checkf(false, TEXT("ERROR: invalid anim instance. Either AnimBP isn't created for this SK mesh, or class not assigned or of wrong type."));
		return;
	}

	anim_instance->SetWeaponGun(this);
	anim_instance->SetFireMode(weapon_gun_info.FireMode);
}

//USkeletalMeshComponent* UWeaponGun::CreateSKWeaponRepresentation(USceneComponent* outer)
//{
//	if(outer == nullptr)
//	{
//		checkf(false, TEXT("ERROR: Outer is invalid. Trying NewObject with invalid outer will cause 'Object not packaged' crash."))
//		return nullptr;
//	}
//
//	//default way of getting SK mesh, there could be others theoretically
//	auto temp_SK_mesh = GetSkeletalMeshFromItemActorCDO();
//
//	if (!IsValid(temp_SK_mesh))
//	{
//		checkf(false, TEXT("ERROR: Skeletal mesh is invalid. Can't create SK component."))
//		return nullptr;
//	}
//
//	USkeletalMeshComponent* sk_comp = NewObject<USkeletalMeshComponent>(outer);
//	SK_WeaponRepresentation = sk_comp;
//
//	sk_comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	sk_comp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
//	sk_comp->SetSimulatePhysics(false);
//	sk_comp->SetGenerateOverlapEvents(false);
//
//	sk_comp->SetSkeletalMesh(temp_SK_mesh, true);
//	sk_comp->SetAnimInstanceClass(GetAnimClass());
//
//	sk_comp->RegisterComponent();
//
//	SetupAnimInstance(sk_comp);
//
//	AddAllAttachmentMeshes(sk_comp);
//
//	return sk_comp;
//}


//void UWeaponGun::SetupWeapon(USkeletalMeshComponent* sk_comp)
//{
//	if(sk_comp == nullptr)
//	{
//		return;
//	}
//
//	WeaponSkeletalMeshCompRef = sk_comp;
//
//	sk_comp->bCollideWithAttachedChildren = false;
//
//	sk_comp->SetSkeletalMesh(GetSkeletalMeshFromItemActorCDO());
//	sk_comp->SetAnimClass(GetAnimClass());
//
//	AddAllAttachmentMeshes(sk_comp);
//}
//
//void UWeaponGun::CleanWeapon(USkeletalMeshComponent* sk_comp)
//{
//	WeaponSkeletalMeshCompRef = nullptr;
//
//	sk_comp->SetSkeletalMesh(nullptr);
//	sk_comp->SetAnimClass(nullptr);
//
//	RemoveAllAttachmentMeshesStatic(sk_comp);
//}
//


bool UWeaponGun::LoadAmmoIntoChamberFromMag()
{
	auto* mag = Cast<UWeaponAttachmentMagazine>(MagazineAttachment);

	if(mag == nullptr)
	{
		return false;
	}

	weapon_gun_info.Chamber = mag->Pop();

	if(weapon_gun_info.Chamber == nullptr)
	{
		return false;
	}

	return true;
}

//bool UWeaponGun::OnUse(AActor* caller)
//{
//	AGameCharacterBase* game_character = Cast<AGameCharacterBase>(caller);
//
//	if(game_character != nullptr)
//	{
//		return game_character->EquipGun(this);
//	}
//
//	return false;
//}

//bool UWeaponGun::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination)
//{
//	auto* attachment = Cast<UWeaponAttachment>(item);
//
//	if(destination.GetObject() == this || attachment == nullptr)
//	{
//		return false;
//	}
//
//	int32 idx = attachment_slots.Find(TPair<FAttachmentSlot, UWeaponAttachment*>{ attachment->GetAttachmentSlot(), attachment });
//
//	if(idx != INDEX_NONE && destination->ReceiveItem(item))
//	{
//		attachment_slots[idx].Value = attachment;
//
//		return true;
//	}
//
//	return false;
//}

bool UWeaponGun::OnEquipped(AActor* caller, USceneComponent* attach_mesh, const FName& socket_name)
{
	if (!IsValid(attach_mesh))
	{
		return false;
	}

	if (WeaponGunActorCached.IsValid())
	{
		//There is already spawned ItemActor
		return false;
	}

	FItemActorSpawnParameters spawn_params;
	spawn_params.Interactible = false;
	spawn_params.MoveOwnershipItemObject = false;
	spawn_params.EnablePhysics = false;
	spawn_params.DisableCollision = true;

	//spawn location and rotation is rather irrelevant, but anyway
	auto spawned_actor = SpawnItemActor(caller->GetActorLocation(), caller->GetActorRotation(), spawn_params);

	if (spawned_actor)
	{
		auto attach_result = spawned_actor->AttachToComponent(attach_mesh, { EAttachmentRule::SnapToTarget, true }, socket_name);
		//attach_mesh->AddTickPrerequisiteComponent(spawned_actor->GetSkeletalMeshComp());
		//spawned_actor->SetTickGroup(ETickingGroup::TG_PrePhysics);

		WeaponGunActorCached = Cast<AWeaponGunActor>(spawned_actor);

		return true;
	}

	return false;
}

bool UWeaponGun::OnUnEquipped()
{
	if (WeaponGunActorCached.IsValid())
	{
		WeaponGunActorCached->Destroy();
	}

	return true;
}

bool UWeaponGun::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
{
	//auto* attachment = Cast<UWeaponAttachment>(item);

	////TODO: test
	//if(destination.GetObject() == this || attachment == nullptr)
	//{
	//	return false;
	//}

	//int32 idx = attachment_slots.Find(TPair<FAttachmentSlot, UWeaponAttachment*>{ attachment->GetAttachmentSlot(), attachment });

	//if(idx != INDEX_NONE && destination->ReceiveItem(item, new_upper_left_cell))
	//{
	//	attachment_slots[idx].Value = attachment;

	//	return true;
	//}

	return false;
}

bool UWeaponGun::AddItemFromWorld(UItemBase* item)
{
	//TODO: Untested

	auto* attachment = Cast<UWeaponAttachment>(item);

	if (attachment == nullptr)
	{
		return false;
	}

	return AddAttachment(attachment);
}

bool UWeaponGun::DropItemToWorld(UItemBase* item)
{
	auto* attachment = Cast<UWeaponAttachment>(item);

	if(attachment == nullptr)
	{
		return false;
	}

	//int32 idx = attachment_slots.Find(TPair<FAttachmentSlot, UWeaponAttachment*>{ attachment->GetAttachmentSlot(), attachment });

	//if(idx != INDEX_NONE)
	//{
	//	return false;
	//}

	//constexpr float DropDistance = 60;
	//FItemActorSpawnParameters spawn_params;
	//spawn_params.ChangeOuter = true;
	//spawn_params.EnablePhysics = true;
	//spawn_params.Interactible = true;

	//auto* item_actor = item->SpawnItemActor(GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * DropDistance, GetOwner()->GetActorRotation());
	//if(item_actor == nullptr)
	//{
	//	//can't spawn, do not delete from inventory
	//	UKismetSystemLibrary::PrintString(GetWorld(), "Drop to world is blocked", true, true, FLinearColor(130, 5, 255), 4);
	//	return false;
	//}

	//attachment_slots[idx].Value = nullptr;

	//item->SetOuterItemActor(item_actor);
	//SK_WeaponRepresentation = nullptr;

	return false;
}

bool UWeaponGun::ReceiveItem(UItemBase* item, FIntPoint new_upper_left_cell)
{
	auto* attachment = Cast<UWeaponAttachment>(item);
	if(attachment != nullptr)
	{
		return AddAttachment(attachment);
	}

	auto* ammo = Cast<UAmmoBase>(item);
	if(ammo != nullptr)
	{
		return LoadAmmoStraightIntoChamber(ammo);
	}

	return false;
}


void UWeaponGun::AddAllAttachmentMeshes(USkeletalMeshComponent* sk_comp)
{
	//Attach all attachments
	//for(int32 i = 0; i < attachment_slots.Num(); i++)
	//{
	//	if(attachment_slots[i].Value != nullptr)
	//	{
	//		AddAttachmentMesh(sk_comp, attachment_slots[i].Value);
	//	}
	//}
}

//void UWeaponGun::SetWeaponInfo(FWeaponInfo&& value)
//{
//	weapon_gun_info = value;
//}

bool UWeaponGun::CycleChargingHandle()
{
	if(weapon_gun_info.bHasChamber)
	{
		return LoadAmmoIntoChamberFromMag();
	}

	return false;
}

const UAmmoBase* UWeaponGun::CheckChamber() const
{
	return weapon_gun_info.Chamber;
}

bool UWeaponGun::Reload()
{
	return false;
}

bool UWeaponGun::ReloadFast()
{
	return false;
}

bool UWeaponGun::LoadAmmoStraightIntoChamber(UAmmoBase* ammo)
{
	if(ammo == nullptr || weapon_gun_info.Chamber != nullptr)
	{
		return false;
	}

	weapon_gun_info.Chamber = Cast<UAmmoBase>(ammo->StackGetSplit(1, this));

	if(ammo->GetCurrentStackSize() == 0)
	{
		//Delete
		return true;
	}
	else
	{
		//Signal that we don't need to delete the original from original inventory
		return false;
	}
}

bool UWeaponGun::AttachMagazine()
{
	return false;
}

bool UWeaponGun::DetachMagazine()
{
	return false;
}

void UWeaponGun::ChangeFireMode()
{
	EndFire();

	size_t i = weapon_gun_info.FireModesAvailable.Find(weapon_gun_info.FireMode);

	if(weapon_gun_info.FireModesAvailable.IsValidIndex(i+1))
	{
		weapon_gun_info.FireMode = weapon_gun_info.FireModesAvailable[i+1];
	}
	else
	{
		weapon_gun_info.FireMode = weapon_gun_info.FireModesAvailable[0];
	}


	//UGunAnimInstance* anim_instance = Cast<UGunAnimInstance>(SK_WeaponRepresentation->GetAnimInstance());

	//if(anim_instance == nullptr)
	//{
	//	checkf(false, TEXT("Warning: UWeaponGun::ChangeFireMode invalid anim instance"));
	//	return;
	//}

	//anim_instance->SetFireMode(weapon_gun_info.FireMode);
}

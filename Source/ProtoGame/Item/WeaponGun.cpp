// Nikita Belov, All rights reserved

#include "Item/WeaponGun.h"
#include "Item/Projectile.h"
#include "Character/GameCharacterBase.h"
#include "ItemActor.h"
#include "Item/WeaponAttachment.h"
#include "Item/WeaponAttachmentMagazine.h"
#include "Animation/GunAnimInstance.h"

#include "Kismet/GameplayStatics.h"

UWeaponGun::UWeaponGun()
{
	if(weapon_info.FireModesAvailable.Num() != 0)
	{
		weapon_info.FireMode = weapon_info.FireModesAvailable[0];
	}
}

//bool UWeaponGun::Initialize(FDataTableRowHandle handle)
//{ 
	//auto* ptr_row = handle.GetRow<FWeaponTable>("SetProperties_WeaponGun");

	//if(ptr_row != nullptr)
	//{
	//	inventory_item_info = ptr_row->inventory_item_info;
	//	weapon_info = ptr_row->weapon_info;

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
	if(weapon_info.bFunctional)
	{
		Firing = true;

		const float rate = 1 / (weapon_info.FireRate / 60);

		switch(weapon_info.FireMode)
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
	auto* mag = Cast<UWeaponAttachmentMagazine>(attachment_slots[MagazineAttachmentIdx].Value);

	FString msg { "Ammo in mag: " + FString::FromInt(mag->GetRoundsLeft())};
	UKismetSystemLibrary::PrintString(GetWorld(), msg, true, true, FLinearColor(206, 245, 66), 2);
}

void UWeaponGun::OnFire()
{
	//Default implementation; Isn't meant to be called directly but you can reference it: UChildClass::OnFire -> UWeaponGun::OnFire()

	if(World == nullptr || weapon_info.bFunctional != true)
	{
		return;
	}

	if(weapon_info.bHasChamber)
	{
		if(weapon_info.Chamber != nullptr)
		{
			if(weapon_info.Chamber->GetProjectileClass() == nullptr)
			{
				checkf(false, TEXT("Projectile isn't assigned, can't fire."));
				return;
			}

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ActorSpawnParams.bHideFromSceneOutliner = true;

			World->SpawnActor<AProjectile>(weapon_info.Chamber->GetProjectileClass(), SK_WeaponRepresentation->GetSocketLocation("b_gun_muzzleflash"), SK_WeaponRepresentation->GetSocketRotation("b_gun_muzzleflash"), ActorSpawnParams);
			weapon_info.Chamber->MarkAsGarbage();
			weapon_info.Chamber = nullptr;

			SpawnMuzzleFlash();

			if(weapon_info.FireSound != nullptr)
			{
				const FVector muzzle_sound_offset = {-5.f, 0.f, 0.f}; //play sound a bit behind the muzzle end
				UGameplayStatics::PlaySoundAtLocation(this, weapon_info.FireSound, SK_WeaponRepresentation->GetSocketLocation("b_gun_muzzleflash") - muzzle_sound_offset);
			}

			LoadAmmoIntoChamberFromMag();
		}
		else //click sound
		{
			UGameplayStatics::SpawnSoundAttached(weapon_info.EmptyClickSound, SK_WeaponRepresentation, "root", {}, EAttachLocation::SnapToTarget);
		}

	}
	//TODO
	else //if no chamber 
	{
		//FActorSpawnParameters ActorSpawnParams;
		//ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		////TODO: Temporary
		//World->SpawnActor<AProjectile>(weapon_info.AmmoType, WeaponSkeletalMeshComp->GetSocketLocation("b_gun_muzzleflash"), WeaponSkeletalMeshComp->GetSocketRotation("b_gun_muzzleflash"), ActorSpawnParams);

		//SpawnMuzzleFlash();

		//if(weapon_info.FireSound != nullptr)
		//{
		//	const FVector muzzle_sound_offset = {-5.f, 0.f, 0.f}; //play sound a bit behind the muzzle end
		//	UGameplayStatics::PlaySoundAtLocation(this, weapon_info.FireSound, WeaponSkeletalMeshComp->GetSocketLocation("b_gun_muzzleflash") - muzzle_sound_offset);
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
	int32 idx = attachment_slots.Find(TPair<FAttachmentSlot, UWeaponAttachment*>{ attachment->GetAttachmentSlot(), nullptr });

	if(idx != INDEX_NONE)
	{
		//If we have valid scene capture, then attach to it
		if(SK_SceneCapture != nullptr)
		{
			if(AddAttachmentMesh(SK_WeaponRepresentation, attachment) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("AddAttachment: Failed to attach mesh to SceneCapture"));
			}
		}

		//if we have ItemActor spawned, then attach to it
		if(GetOuterItemActor() != nullptr)
		{
			if(AddAttachmentMesh(GetOuterItemActor()->GetSkeletalMeshComp(), attachment) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("AddAttachment: Failed to attach mesh to ItemActor"));
				check(false);
				return false;
			}
		}
		//Otherwise check referenced skeletal mesh component
		//Usually this means we have SK in hands of a character and Outer is SpecialSlot (UpstreamInventory)
		else if(SK_WeaponRepresentation != nullptr)
		{
			if(AddAttachmentMesh(SK_WeaponRepresentation, attachment) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("AddAttachment: Failed to attach mesh to WeaponRepresentation"));
				check(false);
				return false;
			}
		}

		attachment_slots[idx].Value = attachment;
		attachment->SetOuterWeapon(this);

		return true;
	}

	return false;
}

bool UWeaponGun::AddAttachmentTo(UWeaponAttachment* attachment, TPair<FAttachmentSlot, UWeaponAttachment*>* slot_pair)
{
	return false;

	//if(!attachment->isAttachmentCompatible(slot_pair->Key))
	//{
	//	return false;
	//}

	//slot_pair->Value = attachment;
	//
	//attachment->SetOuterWeapon(this);

	//return true;
}

AItemActor* UWeaponGun::SpawnItemActor(const FVector& location, const FRotator& rotation)
{
	AItemActor* weapon_item_actor = AItemActor::StaticCreateObject(GetWorld(), ItemActorClass, this, location, rotation);

	if(weapon_item_actor != nullptr)
	{
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
	if(slot.slot_class == AttachmentSlotClass::None || slot.slot_type == AttachmentSlotType::None)
	{
		return false;
	}

	int32 idx = attachment_slots.Emplace(slot, nullptr);

	//Reference magazine for fast access
	if(slot.slot_type == AttachmentSlotType::Magazine || 
		slot.slot_type == AttachmentSlotType::MagazineSpecial || 
		slot.slot_type == AttachmentSlotType::Belt)
	{
		//*MagazineRef = TPair<FAttachmentSlot, UWeaponAttachmentMagazine*>{ attachment_slots[idx].Key, Cast<UWeaponAttachmentMagazine>(attachment_slots[idx].Value) };
		MagazineAttachmentIdx = idx;
	}

	return true;
}

void UWeaponGun::SetupAnimInstance(USkeletalMeshComponent* sk_comp)
{
	UGunAnimInstance* anim_instance = Cast<UGunAnimInstance>(sk_comp->GetAnimInstance());

	if(anim_instance == nullptr)
	{
		checkf(false, TEXT("ERROR: UWeaponGun - invalid anim instance. Probably it's not assigned or wrong type."));
		return;
	}

	anim_instance->SetWeaponGun(this);
	anim_instance->SetFireMode(weapon_info.FireMode);
}

USkeletalMeshComponent* UWeaponGun::CreateSKWeaponRepresentation(USceneComponent* outer)
{
	if(outer == nullptr)
	{
		checkf(false, TEXT("ERROR: Outer is invalid. Trying NewObject with invalid outer will cause 'Object not packaged' crash."))
		return nullptr;
	}

	USkeletalMeshComponent* sk_comp = NewObject<USkeletalMeshComponent>(outer);
	SK_WeaponRepresentation = sk_comp;

	sk_comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	sk_comp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	sk_comp->SetSimulatePhysics(false);
	sk_comp->SetGenerateOverlapEvents(false);

	sk_comp->SetSkeletalMesh(GetSkeletalMeshFromItemActorCDO(), true);
	sk_comp->SetAnimClass(GetAnimClass());

	sk_comp->RegisterComponent();

	SetupAnimInstance(sk_comp);

	AddAllAttachmentMeshes(sk_comp);

	return sk_comp;
}

USkeletalMeshComponent* UWeaponGun::CreateSKForSceneCapture(USceneComponent* outer)
{
	if(outer == nullptr)
	{
		checkf(false, TEXT("ERROR: Outer is invalid. Trying NewObject with invalid outer will cause 'Object not packaged' crash."))
		return nullptr;
	}

	USkeletalMeshComponent* sk_comp = NewObject<USkeletalMeshComponent>(outer);
	SK_SceneCapture = sk_comp;

	sk_comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	sk_comp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	sk_comp->SetSimulatePhysics(false);
	sk_comp->SetGenerateOverlapEvents(false);

	sk_comp->SetSkeletalMesh(GetSkeletalMeshFromItemActorCDO(), true);
	sk_comp->SetAnimClass(GetAnimClass());

	sk_comp->RegisterComponent();

	SetupAnimInstance(sk_comp);

	AddAllAttachmentMeshes(sk_comp);

	return sk_comp;
}

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

void UWeaponGun::SpawnMuzzleFlash() const
{
	if(weapon_info.MuzzleFlash != nullptr)
	{
		//Order: light first, emitter second. Otherwise a weird bug will occur: emitter location will be slightly off (UE 4.27)
		const FActorSpawnParameters actor_spawn_params;

		//Adjust location to account for muzzle device
		//hardcoded for now
		const float MOVE_LIGHT_SPAWN_LOCATION = -1.f; 

		const auto rotation = SK_WeaponRepresentation->GetSocketRotation("b_gun_muzzleflash");
		const auto location = SK_WeaponRepresentation->GetSocketLocation("b_gun_muzzleflash") + rotation.Vector() * MOVE_LIGHT_SPAWN_LOCATION;

		auto light_actor = GetWorld()->SpawnActor(weapon_info.MuzzleLight.Get(), &location, &rotation, actor_spawn_params);
		light_actor->AttachToComponent(SK_WeaponRepresentation, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "b_gun_muzzleflash");

		UGameplayStatics::SpawnEmitterAttached(weapon_info.MuzzleFlash, SK_WeaponRepresentation, "b_gun_muzzleflash", {}, rotation, EAttachLocation::KeepWorldPosition);
	}
}

bool UWeaponGun::LoadAmmoIntoChamberFromMag()
{
	auto* mag = Cast<UWeaponAttachmentMagazine>(attachment_slots[MagazineAttachmentIdx].Value);

	if(mag == nullptr)
	{
		return false;
	}

	weapon_info.Chamber = mag->Pop();

	if(weapon_info.Chamber == nullptr)
	{
		return false;
	}

	return true;
}

bool UWeaponGun::OnUse(AActor* caller)
{
	AGameCharacterBase* game_character = Cast<AGameCharacterBase>(caller);

	if(game_character != nullptr)
	{
		return game_character->EquipGun(this);
	}

	return false;
}

bool UWeaponGun::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination)
{
	auto* attachment = Cast<UWeaponAttachment>(item);

	if(destination.GetObject() == this || attachment == nullptr)
	{
		return false;
	}

	int32 idx = attachment_slots.Find(TPair<FAttachmentSlot, UWeaponAttachment*>{ attachment->GetAttachmentSlot(), attachment });

	if(idx != INDEX_NONE && destination->ReceiveItem(item))
	{
		attachment_slots[idx].Value = attachment;

		return true;
	}

	return false;
}

bool UWeaponGun::MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
{
	auto* attachment = Cast<UWeaponAttachment>(item);

	//TODO: test
	if(destination.GetObject() == this || attachment == nullptr)
	{
		return false;
	}

	int32 idx = attachment_slots.Find(TPair<FAttachmentSlot, UWeaponAttachment*>{ attachment->GetAttachmentSlot(), attachment });

	if(idx != INDEX_NONE && destination->ReceiveItemInGrid(item, new_upper_left_cell))
	{
		attachment_slots[idx].Value = attachment;

		return true;
	}

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

	int32 idx = attachment_slots.Find(TPair<FAttachmentSlot, UWeaponAttachment*>{ attachment->GetAttachmentSlot(), attachment });

	if(idx != INDEX_NONE)
	{
		return false;
	}

	constexpr float DropDistance = 60;

	auto* item_actor = item->SpawnItemActor(GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * DropDistance, GetOwner()->GetActorRotation());
	if(item_actor == nullptr)
	{
		//can't spawn, do not delete from inventory
		UKismetSystemLibrary::PrintString(GetWorld(), "Drop to world is blocked", true, true, FLinearColor(130, 5, 255), 4);
		return false;
	}

	attachment_slots[idx].Value = nullptr;

	item->SetOuterItemActor(item_actor);
	SK_WeaponRepresentation = nullptr;

	return false;
}

bool UWeaponGun::ReceiveItem(UItemBase* item)
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
	for(int32 i = 0; i < attachment_slots.Num(); i++)
	{
		if(attachment_slots[i].Value != nullptr)
		{
			AddAttachmentMesh(sk_comp, attachment_slots[i].Value);
		}
	}
}

//void UWeaponGun::SetWeaponInfo(FWeaponInfo&& value)
//{
//	weapon_info = value;
//}

bool UWeaponGun::CycleChargingHandle()
{
	if(weapon_info.bHasChamber)
	{
		return LoadAmmoIntoChamberFromMag();
	}

	return false;
}

const UAmmoBase* UWeaponGun::CheckChamber() const
{
	return weapon_info.Chamber;
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
	if(ammo == nullptr || weapon_info.Chamber != nullptr)
	{
		return false;
	}

	weapon_info.Chamber = Cast<UAmmoBase>(ammo->StackGet(1, this));

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

	size_t i = weapon_info.FireModesAvailable.Find(weapon_info.FireMode);

	if(weapon_info.FireModesAvailable.IsValidIndex(i+1))
	{
		weapon_info.FireMode = weapon_info.FireModesAvailable[i+1];
	}
	else
	{
		weapon_info.FireMode = weapon_info.FireModesAvailable[0];
	}


	UGunAnimInstance* anim_instance = Cast<UGunAnimInstance>(SK_WeaponRepresentation->GetAnimInstance());

	if(anim_instance == nullptr)
	{
		checkf(false, TEXT("ERROR: UWeaponGun - invalid anim instance."));
		return;
	}

	anim_instance->SetFireMode(weapon_info.FireMode);
}

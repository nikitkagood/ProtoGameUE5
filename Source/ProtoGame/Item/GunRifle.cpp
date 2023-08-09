// Nikita Belov, All rights reserved


#include "Item/GunRifle.h"

#include "Item/WeaponBase.h"
#include "Item/Projectile.h"
#include "Character/GameCharacterBase.h"
#include "ItemActor.h"
#include "Item/WeaponAttachment.h"
#include "Item/WeaponAttachmentMagazine.h"

#include "Kismet/GameplayStatics.h"

UGunRifle::UGunRifle()
{
}

bool UGunRifle::Initialize(FDataTableRowHandle handle)
{ 
	auto* ptr_row = handle.GetRow<DataTableType>("SetProperties_GunRifle");

	if(ptr_row != nullptr)
	{
		item_info = ptr_row->item_info;
		weapon_info = ptr_row->weapon_info;

		for(auto& i : ptr_row->attachment_slots)
		{
			AddAttachmentSlot(i);
		}

		return true;
	}

	return false;
}

FItemThumbnailInfo UGunRifle::GetItemThumbnailInfoFromDT()
{
	return GetItemThumbnailInfoFromDT_Impl<DataTableType>();
}


void UGunRifle::StartFire()
{
	if(weapon_info.bFunctional)
	{
		const float rate = 1 / (weapon_info.FireRate / 60);

		switch(weapon_info.FireMode)
		{
		case WeaponFireMode::Single:
			OnFire();
			break;
		case WeaponFireMode::Auto:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate, true, 0.f);
			break;
		case WeaponFireMode::AutoSlow:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate / 2, true, 0.f);
			break;
		case WeaponFireMode::TwoRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UGunRifle::EndFire, rate, false, rate * 2);
			break;
		case WeaponFireMode::ThreeRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UGunRifle::EndFire, rate, false, rate * 3);
			break;
		case WeaponFireMode::FourRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UGunRifle::EndFire, rate, false, rate * 4);
			break;
		case WeaponFireMode::FiveRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UGunRifle::EndFire, rate, false, rate * 5);
			break;
		case WeaponFireMode::SpecialMode1:
			OnFire();
			break;
		case WeaponFireMode::SpecialMode2:
			OnFire();
			break;
		case WeaponFireMode::SpecialMode3:
			OnFire();
			break;
		default:
			//also Safe
			break;
		}
	}
}

void UGunRifle::EndFire()
{
	if(this != nullptr && OnFireTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(OnFireTimerHandle);
	}
}

void UGunRifle::PrintWeaponStats()
{
	auto* mag = Cast<UWeaponAttachmentMagazine>(attachment_slots[MagazineAttachmentIdx].Value);

	FString msg { "Ammo in mag: " + FString::FromInt(mag->GetRoundsLeft())};
	UKismetSystemLibrary::PrintString(GetWorld(), msg, true, true, FLinearColor(206, 245, 66), 2);
}

void UGunRifle::OnFire()
{
	if(World == nullptr || weapon_info.bFunctional != true)
	{
		return;
	}

	if(weapon_info.bHasChamber)
	{
		//Unlimited ammo, for testing purposes only
		if(bTestFire && weapon_info.Chamber != nullptr)
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

			SpawnMuzzleFlash();

			if(weapon_info.FireSound != nullptr)
			{
				const FVector muzzle_sound_offset = {-5.f, 0.f, 0.f}; //play sound a bit behind the muzzle end
				UGameplayStatics::PlaySoundAtLocation(this, weapon_info.FireSound, SK_WeaponRepresentation->GetSocketLocation("b_gun_muzzleflash") - muzzle_sound_offset);
			}
		}
		else if(weapon_info.Chamber != nullptr)
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

bool UGunRifle::AddAttachment(UWeaponAttachment* attachment)
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

bool UGunRifle::AddAttachmentTo(UWeaponAttachment* attachment, TPair<FAttachmentSlot, UWeaponAttachment*>* slot_pair)
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

bool UGunRifle::OnUse(AActor* caller)
{
	return UWeaponGun::OnUse(caller);
}

bool UGunRifle::CycleChargingHandle()
{
	if(weapon_info.bHasChamber)
	{
		return LoadAmmoIntoChamberFromMag();
	}

	return false;
}

const UAmmoBase* UGunRifle::CheckChamber() const
{
	return weapon_info.Chamber;
}

bool UGunRifle::Reload()
{
	return false;
}

bool UGunRifle::ReloadFast()
{
	return false;
}

bool UGunRifle::AttachMagazine()
{
	return false;
}

bool UGunRifle::DetachMagazine()
{
	return false;
}

void UGunRifle::ChangeFireMode()
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
}

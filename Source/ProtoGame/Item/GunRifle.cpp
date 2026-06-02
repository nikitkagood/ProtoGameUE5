// Nikita Belov, All rights reserved


#include "Item/GunRifle.h"

#include "Item/WeaponBase.h"
#include "Item/Projectile.h"
#include "Character/GameCharacterBase.h"
#include "ItemActor.h"
#include "Item/WeaponAttachment.h"
#include "Item/WeaponAttachmentMagazine.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

bool UGunRifle::Initialize(FDataTableRowHandle handle)
{ 
	auto* ptr_row = handle.GetRow<DataTableType>("SetProperties_GunRifle");

	if(ptr_row != nullptr)
	{
		inventory_item_info = ptr_row->inventory_item_info;
		weapon_gun_info = ptr_row->weapon_gun_info;

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
	if(weapon_gun_info.bFunctional)
	{
		const float rate = 1 / (weapon_gun_info.FireRate / 60);

		switch(weapon_gun_info.FireMode)
		{
		case EWeaponFireMode::Single:
			OnFire();
			break;
		case EWeaponFireMode::Auto:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate, true, 0.f);
			break;
		case EWeaponFireMode::AutoSlow:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate / 2, true, 0.f);
			break;
		case EWeaponFireMode::TwoRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UGunRifle::EndFire, rate, false, rate * 2);
			break;
		case EWeaponFireMode::ThreeRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UGunRifle::EndFire, rate, false, rate * 3);
			break;
		case EWeaponFireMode::FourRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UGunRifle::EndFire, rate, false, rate * 4);
			break;
		case EWeaponFireMode::FiveRoundBurst:
			GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &UGunRifle::OnFire, rate, true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(BurstFireTimerHandle, this, &UGunRifle::EndFire, rate, false, rate * 5);
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

void UGunRifle::EndFire()
{
	if(this != nullptr && OnFireTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(OnFireTimerHandle);
	}
}

void UGunRifle::PrintWeaponStats()
{
	//auto* mag = Cast<UWeaponAttachmentMagazine>(attachment_slots[MagazineAttachmentIdx].Value);

	//FString msg { "Ammo in mag: " + FString::FromInt(mag->GetAmmoLeft())};
	//UKismetSystemLibrary::PrintString(GetWorld(), msg, true, true, FLinearColor(206, 245, 66), 2);
}

void UGunRifle::OnFire()
{
	Super::OnFire();
}

bool UGunRifle::AddAttachment(UWeaponAttachment* attachment)
{
	return Super::AddAttachment(attachment);
}

bool UGunRifle::AddAttachmentTo(UWeaponAttachment* attachment, FAttachmentSlot* slot)
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
	if(weapon_gun_info.bHasChamber)
	{
		return LoadAmmoIntoChamberFromMag();
	}

	return false;
}

const UAmmoBase* UGunRifle::CheckChamber() const
{
	return weapon_gun_info.Chamber;
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

	size_t i = weapon_gun_info.FireModesAvailable.Find(weapon_gun_info.FireMode);

	if(weapon_gun_info.FireModesAvailable.IsValidIndex(i+1))
	{
		weapon_gun_info.FireMode = weapon_gun_info.FireModesAvailable[i+1];
	}
	else
	{
		weapon_gun_info.FireMode = weapon_gun_info.FireModesAvailable[0];
	}
}

// Nikita Belov, All rights reserved

#include "WeaponGunActor.h"
#include "Animation/GunAnimInstance.h"
#include "Projectile.h"
#include "Item/WeaponInfo.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

#include "Item/WeaponGun.h"

AWeaponGunActor::AWeaponGunActor()
{

}

void AWeaponGunActor::OnFire()
{
	if (item_actor_mesh_type == ItemActorMeshType::SkeletalMesh)
	{
		auto sk_comp = GetSkeletalMeshComp();

		auto* weapon_gun = Cast<UWeaponGun>(ItemObject);

		if (!IsValid(weapon_gun))
		{
			return;
		}


		auto weapon_gun_info = weapon_gun->GetWeaponInfoConst();

		if (weapon_gun_info.bHasChamber)
		{
			if (weapon_gun_info.Chamber != nullptr)
			{
				if (weapon_gun_info.Chamber->GetProjectileClass() == nullptr)
				{
					checkf(false, TEXT("Projectile isn't assigned, can't fire."));
					return;
				}

				//Usually Rotation is simply relative X-forward, but not SocketRotation due to bones may be oriented wrong
				SpawnProjectile(weapon_gun_info.Chamber->GetProjectileClass(), { sk_comp->GetComponentRotation(), sk_comp->GetSocketLocation(GetWeaponGunActorInfo().gun_bone_names.MuzzleEnd), {1,1,1} });

				SpawnMuzzleFlash(GetWeaponGunActorInfo().MuzzleFlash, GetWeaponGunActorInfo().MuzzleLight);

				if (GetWeaponGunActorInfo().FireSound != nullptr)
				{
					const FVector muzzle_sound_offset = { -5.f, 0.f, 0.f }; //play sound a bit behind the muzzle end
					UGameplayStatics::PlaySoundAtLocation(this, GetWeaponGunActorInfo().FireSound, sk_comp->GetSocketLocation(GetWeaponGunActorInfo().gun_bone_names.MuzzleEnd) - muzzle_sound_offset);
				}

				//LoadAmmoIntoChamberFromMag();
			}
			else //dry fire sound
			{
				UGameplayStatics::SpawnSoundAttached(GetWeaponGunActorInfo().DryFireSound, sk_comp, "root", {}, EAttachLocation::SnapToTarget);
			}
		}
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

	
	
}

void AWeaponGunActor::BeginPlay()
{
	Super::BeginPlay();

	checkf(IsValid(gun_actor_data), TEXT("gun_actor_data is invalid"))
}

void AWeaponGunActor::SpawnMuzzleFlash(UParticleSystem* MuzzleFlash, TSubclassOf<AEffectBase> muzzle_light)
{
	if (MuzzleFlash != nullptr)
	{
		//Order: light first, emitter second. Otherwise a weird bug will occur: emitter location will be slightly off (UE 4.27)
		const FActorSpawnParameters actor_spawn_params;

		//Adjust location to account for muzzle device
		//hardcoded for now
		const float MOVE_LIGHT_SPAWN_LOCATION = -1.f;

		//const auto rotation = SK_WeaponRepresentation->GetSocketRotation("b_gun_muzzleflash");
		const auto rotation = GetSkeletalMeshComp()->GetComponentRotation(); //sometimes SocketRotation is wrong, most of the time it's x forward and that's it

		if (IsValid(muzzle_light))
		{
			const auto light_location = GetSkeletalMeshComp()->GetSocketLocation(GetWeaponGunActorInfo().gun_bone_names.MuzzleEnd) + rotation.Vector() * MOVE_LIGHT_SPAWN_LOCATION;

			auto light_actor = GetWorld()->SpawnActor(muzzle_light.Get(), &light_location, &rotation, actor_spawn_params);
			light_actor->AttachToComponent(GetSkeletalMeshComp(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GetWeaponGunActorInfo().gun_bone_names.MuzzleEnd);
		}

		//auto* emitter = UGameplayStatics::SpawnEmitterAttached(weapon_gun_info.MuzzleFlash, SK_WeaponRepresentation, "b_gun_muzzleflash", SK_WeaponRepresentation->GetSocketLocation("b_gun_muzzleflash"), rotation, EAttachLocation::SnapToTarget);
		auto* emitter = UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GetSkeletalMeshComp(), GetWeaponGunActorInfo().gun_bone_names.MuzzleEnd, GetSkeletalMeshComp()->GetSocketLocation(GetWeaponGunActorInfo().gun_bone_names.MuzzleEnd), rotation, EAttachLocation::KeepWorldPosition);

		if (emitter == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("UWeaponGun::SpawnMuzzleFlash: failed to spawn emitter"));
		}
	}

}

bool AWeaponGunActor::SpawnProjectile(TSubclassOf<AProjectile> projectile_class, const FTransform& transform)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ActorSpawnParams.bHideFromSceneOutliner = true;
	ActorSpawnParams.Instigator = Cast<APawn>(GetOwner()); //Simple attempt to get the Pawn as Instigator, may fail
	ActorSpawnParams.bAllowDuringConstructionScript = false; //We should not be shooting at this point

	//GetWorld()->SpawnActor<AProjectile>(weapon_gun_info.Chamber->GetProjectileClass(), sk_comp->GetSocketLocation("b_gun_muzzleflash"), sk_comp->GetComponentRotation(), ActorSpawnParams);
	return GetWorld()->SpawnActor<AProjectile>(projectile_class, transform, ActorSpawnParams) != nullptr;
}

const FWeaponGunActorInfo& AWeaponGunActor::GetWeaponGunActorInfo()
{
	return gun_actor_data->gun_actor_info;
}

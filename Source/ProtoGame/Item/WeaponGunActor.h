// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "WeaponBaseActor.h"

#include "WeaponGunActor.generated.h"

class AProjectile;
class AEffectBase;
class AProjectile;
class UParticleSystem;
class UFWeaponGunActorData;
struct FWeaponGunActorInfo;

//Represents gun in world. Mostly visuals, logic is in ItemObject
UCLASS(PrioritizeCategories = "WeaponGunActor ItemActor ItemObject")
class PROTOGAME_API AWeaponGunActor : public AWeaponBaseActor
{
	GENERATED_BODY()

public:
	AWeaponGunActor();

	virtual void OnAttack() { OnFire(); };

	UFUNCTION(BlueprintCallable)
	virtual void OnFire();

protected:
	virtual void BeginPlay() override;

	virtual void SpawnMuzzleFlash(UParticleSystem* MuzzleFlash, TSubclassOf<AEffectBase> muzzle_light = nullptr);

	//TODO: make it DOD
	//It's a temporary solution
	virtual bool SpawnProjectile(TSubclassOf<AProjectile> projectile_class, const FTransform& transform);

	const FWeaponGunActorInfo& GetWeaponGunActorInfo();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, NoClear, Category = "WeaponGunActor", meta = (AllowPrivateAccess = true))
	UFWeaponGunActorData* gun_actor_data;
};

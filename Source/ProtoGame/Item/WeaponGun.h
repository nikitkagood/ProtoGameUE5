// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Item/WeaponBase.h"

#include "WeaponGun.generated.h"

//Abstract. But not marked so for inventory class compatibility system to work
UCLASS()
class PROTOGAME_API UWeaponGun : public UWeaponBase
{
	GENERATED_BODY()

public:
	UWeaponGun();

	virtual bool Initialize(FDataTableRowHandle handle) override { check(false); return false; };

	const auto& GetWeaponInfoConst() const { return weapon_gun_info; };

	virtual FItemThumbnailInfo GetItemThumbnailInfoFromDT() override { check(false); return {}; };

	//virtual TSubclassOf<UAnimInstance> GetAnimClass() const { return weapon_gun_info.AnimClass; };

    UFUNCTION(BlueprintCallable, BlueprintPure)
	EWeaponFireMode GetFireMode() const { return weapon_gun_info.FireMode; }

	virtual AItemActor* SpawnItemActor(const FVector& location, const FRotator& rotation, const FItemActorSpawnParameters& spawn_parameters) override;

	UFUNCTION(BlueprintCallable)
	bool AddAttachmentSlot(const FAttachmentSlot& slot);

	virtual void SetupAnimInstance(USkeletalMeshComponent* sk_comp);

	//Create functional and animated SK mesh comp which represents Weapon

	//virtual USkeletalMeshComponent* CreateSKWeaponRepresentation(USceneComponent* outer) override;

    //TODO: the same code as CreateSKWeaponRepresentation; It's cause SceneCapture (item preview in UI) is still in progress
	//virtual USkeletalMeshComponent* CreateSKForSceneCapture(USceneComponent* outer) override;

	//Equips and and de-equips (by calling GameCharacter)
	//virtual bool OnUse(AActor* caller) override;

	virtual bool OnEquipped(AActor* caller, USceneComponent* attach_mesh, const FName& socket_name) override;

	//Inventory interface; This class supports only WeaponAttachment and AmmoBase items

	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool AddItemFromWorld(UItemBase* item) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	virtual bool ReceiveItem(UItemBase* item, FIntPoint new_upper_left_cell) override;
	virtual void UpdateInventory() override { OnInventoryUpdated.Broadcast(); };
	virtual FGameplayTagContainer GetInventoryTags() const override { return {}; }; //not supported

	UPROPERTY(BlueprintAssignable)
	FOnInventoryUpdated OnInventoryUpdated;

public:
	virtual void OnAttack() override { StartFire(); };

	//Weapon controls

	UFUNCTION(BlueprintCallable)
	bool CycleChargingHandle();

	UFUNCTION(BlueprintCallable)
	const UAmmoBase* CheckChamber() const;

	UFUNCTION(BlueprintCallable)
	bool Reload();

	//Reload a bit faster but drop the mag
	UFUNCTION(BlueprintCallable)
	bool ReloadFast();

	UFUNCTION(BlueprintCallable)
	bool LoadAmmoStraightIntoChamber(UAmmoBase* ammo);

	UFUNCTION(BlueprintCallable)
	bool AttachMagazine();

	UFUNCTION(BlueprintCallable)
	bool DetachMagazine();

	UFUNCTION(BlueprintCallable)
	void ChangeFireMode();

	//UFUNCTION(BlueprintCallable)
	//bool ChangeSight() const;

	//UFUNCTION(BlueprintCallable)
	//bool ChangeSightMode() const;

	//UFUNCTION(BlueprintCallable)
	//bool ChangeSightBrightness();

	//UFUNCTION(BlueprintCallable)
	//bool ChangeSightSpecialOption();

	//UFUNCTION(BlueprintCallable)
	//bool ChangeSightRange() const;

	//UFUNCTION(BlueprintCallable)
	//bool ToggleFunctionalAttachment();

	//UFUNCTION(BlueprintCallable)
	//bool ChangeFunctionalAttachmentMode();

	//Weapon function
	
	UFUNCTION(BlueprintCallable)
	virtual bool IsFunctional() { return weapon_gun_info.bFunctional; }

	virtual void StartFire();
	virtual void EndFire();

	UFUNCTION(BlueprintCallable)
	bool IsFiring() const { return Firing; };

	UFUNCTION(BlueprintCallable)
	float GetDispersionMOA() { return weapon_gun_info.Dispersion; }

	UFUNCTION(BlueprintCallable)
	float GetFireRate() { return weapon_gun_info.FireRate; }

	//class related

	//Debug
	UFUNCTION(BlueprintCallable)
	void PrintWeaponStats();

	virtual void OnDestroy() override;
protected:
	//Firstly will try to get from Outer
	//Secondly will get cached ptr
	UFUNCTION(BlueprintCallable)
	AWeaponGunActor* GetWeaponGunActor();

	//TODO: make it DOD and FireManager or smth
	//Should not be called directly, call StartFire
	virtual void OnFire();

	//void SpawnMuzzleFlash(USkeletalMeshComponent* sk_comp) const;

	bool LoadAmmoIntoChamberFromMag();

	bool AddAttachmentMesh(USkeletalMeshComponent* sk_comp, UWeaponAttachment* attachment);
	void RemoveAttachmentMesh(USkeletalMeshComponent* sk_comp, UWeaponAttachment* attachment);
	void AddAllAttachmentMeshes(USkeletalMeshComponent* sk_comp);
	void RemoveAllAttachmentMeshes(USkeletalMeshComponent* sk_comp);

	bool AddAttachment(UWeaponAttachment* attachment);

	//TODO: implement
	//bool AddAttachmentTo(UWeaponAttachment* attachment, FAttachmentSlot* slot);

	FAttachmentSlot* FindSlot(const FAttachmentSlot& slot) { return nullptr; };

	//TODO: implement
	static constexpr int32 GUN_MAX_SINGLE_FIRE_FIRERATE = 500;

	FTimerHandle OnFireTimerHandle;
	FTimerHandle BurstFireTimerHandle;

	//Weapon related

	//Cached weak pointer to weapon gun actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<class AWeaponGunActor> WeaponGunActorCached;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
	FWeaponGunInfo weapon_gun_info;

	//Class related

	TMultiMap<FAttachmentSlot, UWeaponAttachment*> attachment_slots;

	bool Firing = false;

	//Cached mag
	UWeaponAttachmentMagazine* MagazineAttachment;
};

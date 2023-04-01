// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Item/WeaponBase.h"

#include "WeaponGun.generated.h"


UCLASS(Abstract)
class PROTOGAME_API UWeaponGun : public UWeaponBase
{
	GENERATED_BODY()

public:
	UWeaponGun();

	virtual bool SetProperties(FDataTableRowHandle handle) override { check(false); return false; };

	virtual FItemThumbnailInfo GetItemThumbnailInfoFromDT() override { check(false); return {}; };

	virtual TSubclassOf<UAnimInstance> GetAnimClass() const { return weapon_info.AnimClass; };

    UFUNCTION(BlueprintCallable, BlueprintPure)
	WeaponFireMode GetFireMode() const { return weapon_info.FireMode; }

	virtual AItemActor* SpawnItemActor(const FVector& location, const FRotator& rotation) override;

	UFUNCTION(BlueprintCallable)
	bool AddAttachmentSlot(const FAttachmentSlot& slot);

	virtual void SetupAnimInstance(USkeletalMeshComponent* sk_comp);

	//Create functional and animated SK mesh comp which represents Weapon

	virtual USkeletalMeshComponent* CreateSKWeaponRepresentation(USceneComponent* outer) override;

	virtual USkeletalMeshComponent* CreateSKForSceneCapture(USceneComponent* outer) override;

	//Equips and and de-equips (by calling GameCharacter)
	virtual bool OnUse(AActor* caller) override;

	//Inventory interface; This class supports only WeaponAttachment and AmmoBase items
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) override;
	virtual bool MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	virtual bool ReceiveItem(UItemBase* item) override;
	virtual void UpdateInventory() override { OnInventoryUpdated.Broadcast(); };

	UPROPERTY(BlueprintAssignable)
	FOnInventoryUpdated OnInventoryUpdated;
private:
	//Not supported
	virtual bool ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell) override { check(false); return false; };

public: 	
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

	virtual void StartFire();
	virtual void EndFire();

	//class related

	//Debug
	UFUNCTION(BlueprintCallable)
	void PrintWeaponStats();
protected:
	virtual void OnFire();

	void SpawnMuzzleFlash() const;

	bool LoadAmmoIntoChamberFromMag();

	bool AddAttachmentMesh(USkeletalMeshComponent* sk_comp, UWeaponAttachment* attachment);
	void RemoveAttachmentMesh(USkeletalMeshComponent* sk_comp, UWeaponAttachment* attachment);
	void AddAllAttachmentMeshes(USkeletalMeshComponent* sk_comp);
	void RemoveAllAttachmentMeshes(USkeletalMeshComponent* sk_comp);

	bool AddAttachment(UWeaponAttachment* attachment);

	//TODO: implement
	bool AddAttachmentTo(UWeaponAttachment* attachment, TPair<FAttachmentSlot, UWeaponAttachment*>* slot_pair);

	//TODO: implement
	static constexpr int32 GUN_MAX_SINGLE_FIRE_FIRERATE = 500;

	FTimerHandle OnFireTimerHandle;
	FTimerHandle BurstFireTimerHandle;

	//Weapon related

	UPROPERTY(BlueprintReadWrite, EditAnywhere, NoClear, meta = (AllowPrivateAccess = true))
	FWeaponInfo weapon_info;

	//Class related

	TArray<TPair<FAttachmentSlot, UWeaponAttachment*>> attachment_slots;

	//Saved for quick access
	int32 MagazineAttachmentIdx;
};

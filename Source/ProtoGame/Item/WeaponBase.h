// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "Item/WeaponInfo.h"

#include "Interfaces/InventoryInterface.h"

#include "WeaponBase.generated.h"

class UWeaponAttachment;
class UWeaponAttachmentMagazine;

UCLASS(Abstract)
class PROTOGAME_API UWeaponBase : public UItemBase, public IInventoryInterface
{
	GENERATED_BODY()

public:
	UWeaponBase() = default;

	virtual bool SetProperties(FDataTableRowHandle handle) override { check(false); return nullptr; };

	virtual FItemThumbnailInfo GetItemThumbnailInfoFromDT() override { check(false); return {}; };

	UFUNCTION(BlueprintCallable)
	virtual TSubclassOf<UAnimInstance> GetAnimClass() const { check(false); return nullptr; };

	//virtual AItemActor* SpawnItemActor(const FVector& location, const FRotator& rotation) override;
	//virtual AItemActor* SpawnItemActorVisualOnly(const FVector& location, const FRotator& rotation) override;

	//Create functional and animated SK mesh comp which represents Weapon
	UFUNCTION()
	virtual USkeletalMeshComponent* CreateSKWeaponRepresentation(USceneComponent* outer) { check(false); return nullptr; };

	//virtual USkeletalMeshComponent* CreateSKForSceneCapture() override { check(false); return nullptr; };

	UFUNCTION(BlueprintCallable)
	static void DestroySKStatic(USkeletalMeshComponent* sk_comp);

	//Equips and and de-equips (by calling GameCharacter)
	//virtual bool OnUse(AActor* caller) override;

	//Inventory interface
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) override { check(false); return false; };
	virtual bool MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override { check(false); return false; };
	virtual bool DropItemToWorld(UItemBase* item) override { check(false); return false; };
	virtual bool ReceiveItem(UItemBase* item) override { check(false); return false; };
	virtual void UpdateInventory() override { check(false); return; };
private:
	//Not supported
	virtual bool ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell) override { check(false); return false; };

public: 	
	//Weapon controls

	//virtual void StartFire() { check(false); return; };
	//virtual void EndFire() { check(false); return; };

	//class related



protected:
	//void OnFire();

	//Reference to fully functional SK mesh component which is used to represent this weapon.
	//Used when Outer != ItemActor. For example when weapon is in a character's hands and Outer == SpecialSlot (UpstreamInventory).
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* SK_WeaponRepresentation;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* SK_SceneCapture;

};
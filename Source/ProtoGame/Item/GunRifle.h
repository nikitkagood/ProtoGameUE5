// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Item/WeaponGun.h"

#include "GunRifle.generated.h"

UCLASS()
class PROTOGAME_API UGunRifle : public UWeaponGun
{
	GENERATED_BODY()

private:
	using DataTableType = FWeaponTable;
public:
	UGunRifle();

	virtual bool Initialize(FDataTableRowHandle handle) override;

	virtual FItemThumbnailInfo GetItemThumbnailInfoFromDT() override;

	//Equips and and de-equips (by calling GameCharacter)
	virtual bool OnUse(AActor* caller) override;

public: 	
	//Weapon controls

	bool CycleChargingHandle();

	const UAmmoBase* CheckChamber() const;

	bool Reload();

	//Reload a bit faster but drop the mag
	bool ReloadFast();

	bool AttachMagazine();

	bool DetachMagazine();

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

	virtual void StartFire() override;
	virtual void EndFire() override;

	//class related

	//Debug
	void PrintWeaponStats();
protected:
	virtual void OnFire();
private:
	bool AddAttachment(UWeaponAttachment* attachment);

	//TODO: implement
	bool AddAttachmentTo(UWeaponAttachment* attachment, TPair<FAttachmentSlot, UWeaponAttachment*>* slot_pair);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bTestFire = true;
};

// Nikita Belov, All rights reserved

#pragma once

#include "Item/ItemBase.h"
#include "Item/AmmoInfo.h"

#include "AmmoBase.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent), ClassGroup=Item)
class PROTOGAME_API UAmmoBase : public UItemBase
{
	GENERATED_BODY()
private:
	using DataTableType = FAmmoTable;
public:
	UAmmoBase() = default;

	virtual bool SetProperties(FDataTableRowHandle handle) override;

	virtual FItemThumbnailInfo GetItemThumbnailInfoFromDT() override;

	UFUNCTION(BlueprintCallable)
	AmmoCaliber GetCaliber() const { return ammo_info.caliber; };

	UFUNCTION(BlueprintCallable)
	TSubclassOf<AProjectile> GetProjectileClass() const { return ammo_info.projectile_class; };

private:
	FAmmoInfo ammo_info;
};

#pragma once

UENUM(BlueprintType)
enum class ItemType : uint8
{
    None                UMETA(DisplayName = "None"),
    Misc                UMETA(DisplayName = "Misc"),
    Consumable          UMETA(DisplayName = "Consumable"),
    BuildingMaterial    UMETA(DisplayName = "Material"),
    CraftingMaterial    UMETA(DisplayName = "CraftingMaterial"),
    Clothing            UMETA(DisplayName = "Clothing"),
    Armor               UMETA(DisplayName = "Armor"),
    Plant               UMETA(DisplayName = "Plant"),
    RangedWeapon        UMETA(DisplayName = "RangedWeapon"),
    MeleeWeapon         UMETA(DisplayName = "MeleeWeapon"),
    ThrowingWeapon      UMETA(DisplayName = "ThrowingWeapon"),
    Explosive           UMETA(DisplayName = "ThrowingWeapon"),
    WeaponAttachment    UMETA(DisplayName = "WeaponAttachment"),
    Vehicle             UMETA(DisplayName = "Vehicle"),
    VehiclePart         UMETA(DisplayName = "VehiclePart"),
};

//Usage example:
//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
//TEnumAsByte<Status> status;

UENUM(BlueprintType)
enum class ItemSubtype : uint8
{
    None                UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class ItemSubtypeRangedWeapon : uint8
{
    AssaultRifle        UMETA(DisplayName = "AssaultRifle"),
    Machinegun          UMETA(DisplayName = "Machinegun"),
    Submachinegun       UMETA(DisplayName = "Submachinegun"),
    SniperRifle         UMETA(DisplayName = "SniperRifle"),
    Pistol              UMETA(DisplayName = "Pistol"),
};
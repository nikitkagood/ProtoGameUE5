#pragma once

#include "MassEntityTypes.h"

#include "FireSystemFragments.generated.h" 

//It's like bool isFiring 
//But instead of loading whole Actor in cache to read the bool,
//processor goes over collection of archetypes and instantly filters out entities
USTRUCT()
struct FMEFireTag : public FMassTag
{
    GENERATED_BODY()
};

//instance-only
USTRUCT(BlueprintType)
struct FBallisticEntityFragment : public FMassFragment
{
    GENERATED_BODY()

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    FVector Position = FVector::ZeroVector;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    FVector Velocity = FVector::ZeroVector;
};

//shared across instances
USTRUCT(BlueprintType)
struct FBallisticSharedFragment : public FMassSharedFragment
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector GravityDirection = FVector(0.f, 0.f, -1.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GravityMagnitude = 980.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DragCoefficient = 0;
};


//TODO: example only
USTRUCT(BlueprintType)
struct FFireTriggerFragment : public FMassFragment
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    bool bIsTriggered = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float TimeSinceLastShot = 0.f;
};

//TODO: example only
USTRUCT(BlueprintType)
struct FWeaponStatsFragment : public FMassFragment
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float FireRate = 0.1f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    int32 AmmoCount = 30;

    //UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    //float BaseDamage = 25.f; 

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float MuzzleVelocity = 6000.f;
};


//TODO: example only
USTRUCT(BlueprintType)
struct FWeaponTransformFragment : public FMassFragment
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FVector MuzzleLocation = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FVector FireDirection = FVector::ForwardVector;
};


// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "VBFUnitBase.h"
#include "VBFFaction.h"
#include "VBFMap.h"
#include "VBFCommander.h"

#include "VBFManager.generated.h"

//Manages virtual battlefield game
UCLASS(Blueprintable, BlueprintType)
class PROTOGAME_API UVBFManager : public UObject
{
	GENERATED_BODY()

public:
	UVBFManager() = default;
	UVBFManager(bool isSaveLoading);

	//void Initialize(bool isSaveLoading);

protected:
	UFUNCTION(BlueprintCallable)
	bool SetDefaultRelationsDT(UDataTable* default_relations_table);

	//Get relations between two factions
	UFUNCTION(BlueprintCallable)
	int32 GetFactionRelationsPair(const UVBFFaction* faction_l, const UVBFFaction* faction_r) const;

	//Get all relations of one faction
	//k: other faction, v: relations
	UFUNCTION(BlueprintCallable)
	TMap<UVBFFaction*, int32> GetFactionAllRelations(const UVBFFaction* faction) const;

	//Order-independent, will do: min faction ID left - right max
	UFUNCTION()
	bool CreateFactionRelationsEntry(const UVBFFaction* faction_l, const UVBFFaction* faction_r, int32 relations = 0);

	//Order-independent, will do: min faction ID left - right max
	//Doesn't create map entry, only sets existing ones
	UFUNCTION(BlueprintCallable)
	bool SetFactionRelations(const UVBFFaction* faction_l, const UVBFFaction* faction_r, int32 relations);

	//Order-independent, will do: min faction ID left - right max
	//Also checks whether faction relations exist
	UFUNCTION(BlueprintCallable)
	bool ChangeFactionRelations(const UVBFFaction* faction_l, const UVBFFaction* faction_r, int32 relations_change);

	//Order-independent, will do: min faction ID left - right max
	UFUNCTION()
	bool RemoveFactionRelations(const UVBFFaction* faction_l, const UVBFFaction* faction_r);

	UFUNCTION(BlueprintCallable)
	void SetFactionRelationsLocked(const UVBFFaction* faction_l, const UVBFFaction* faction_r, bool locked);

	UFUNCTION(BlueprintCallable)
	bool IsFactionRelationsLocked(const UVBFFaction* faction_l, const UVBFFaction* faction_r);

private:
	using FactionIdMin = int64;
	using FactionIdMax = int64;

	enum : int32 { RELATIONS_NONE = -9999999 };
	static const int32 GetRelationsValueMin() { return -1000; }
	static const int32 GetRelationsValueMax() { return 1000; }

	//Utility function to get a correct map key
	const TPair<FactionIdMin, FactionIdMax> GetFactionRelationsMapKey(const UVBFFaction* faction_l, const UVBFFaction* faction_r) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	TSet<UVBFFaction*> Factions;

	//Order-dependent. Expects min left - right max pair.
	//We assume: 
	// - 2 different factions always have relations
	// - if faction doesn't exist anymore, then we are allowed to delete relations records
	//v: value ranges -1000/+1000
	TMap<TPair<FactionIdMin, FactionIdMax>, int32> FactionRelations;

	//If there is entry - then it's locked
	TSet<TPair<FactionIdMin, FactionIdMax>> FactionRelationsLocked;

	//Both human and AI "players"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	TSet<UVBFCommander*> Commanders;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	UVBFMap* VBFMap;

	//TODO
	//int64 is UnitOwnerID
	//UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	//TMap<int64, UVBFUnitBase*> Units;
};

//DT must not have duplicates. Anyway the first encountered row will be used.
USTRUCT(BlueprintType)
struct PROTOGAME_API FFactionRelationsDT : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UVBFFaction* faction_l;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UVBFFaction* faction_r;

	//You can disallow change of relations
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool relations_locked = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = -1000, ClampMax = 1000))
	int32 default_relations;
};
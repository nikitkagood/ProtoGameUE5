// Nikita Belov, All rights reserved


#include "VBFManager.h"

UVBFManager::UVBFManager(bool isSaveLoading)
{
    if (isSaveLoading != true)
    {
        //SetDefaultRelations();
    }
}

int32 UVBFManager::GetFactionRelationsPair(const UVBFFaction* faction_l, const UVBFFaction* faction_r) const
{
    if (faction_l != nullptr && faction_r != nullptr)
    {
        return FactionRelations.FindRef(GetFactionsMapKey(faction_l, faction_r));
    }

    UE_LOG(LogTemp, Warning, TEXT("UVBFManager::GetFactionRelationsPair: invalid ptr"))
    return RELATIONS_NONE;
}

TMap<UVBFFaction*, int32> UVBFManager::GetFactionAllRelations(const UVBFFaction* faction) const
{
    TMap<UVBFFaction*, int32> result;

    for (const auto& i : Factions)
    {
        int32 current_relations = GetFactionRelationsPair(faction, i);

        if (current_relations != RELATIONS_NONE)
        {
            result.Add(i, current_relations);
        }
    }

    return result;
}

bool UVBFManager::SetFactionRelations(const UVBFFaction* faction_l, const UVBFFaction* faction_r, int32 relations)
{
   if (faction_l == nullptr && faction_r == nullptr)
   {
       UE_LOG(LogTemp, Error, TEXT("UVBFManager::ChangeFactionRelations: invalid ptr"));
       return false;
   }
   if (relations > GetRelationsValueMax() || relations < GetRelationsValueMin())
   {
       UE_LOG(LogTemp, Error, TEXT("UVBFManager::ChangeFactionRelations: relations value out of valid range"));
       return false;
   }

   FactionRelations.FindOrAdd(GetFactionsMapKey(faction_l, faction_r)) = relations;
   return true;
}

bool UVBFManager::ChangeFactionRelations(const UVBFFaction* faction_l, const UVBFFaction* faction_r, int32 relations_change)
{
    if (faction_l == nullptr && faction_r == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UVBFManager::ChangeFactionRelations: invalid ptr"));
        return false;
    }
    
    auto* value_ptr = FactionRelations.Find(GetFactionsMapKey(faction_l, faction_r));

    if (value_ptr == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UVBFManager::ChangeFactionRelations: relations value doesn't exist"));
        return false;
    }

    if ((*value_ptr + relations_change) > GetRelationsValueMax() || (*value_ptr + relations_change) < GetRelationsValueMin())
    {
        UE_LOG(LogTemp, Error, TEXT("UVBFManager::ChangeFactionRelations: end relations value out of valid range"));
        return false;
    }

    *value_ptr += relations_change;
    return true;
}

bool UVBFManager::RemoveFactionRelations(const UVBFFaction* faction_l, const UVBFFaction* faction_r)
{
    return FactionRelations.Remove(GetFactionsMapKey(faction_l, faction_r))
    != 0;
}

const TPair<UVBFManager::FactionIdMin, UVBFManager::FactionIdMax> UVBFManager::GetFactionsMapKey(const UVBFFaction* faction_l, const UVBFFaction* faction_r) const
{
    return { FMath::Min(faction_l->GetFactionId(), faction_r->GetFactionId()), FMath::Max(faction_l->GetFactionId(), faction_r->GetFactionId()) };
}
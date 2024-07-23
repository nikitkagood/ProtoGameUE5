// Nikita Belov, All rights reserved


#include "VBFManager.h"

UVBFManager::UVBFManager(bool isSaveLoading)
{
    if (isSaveLoading != true)
    {
        //SetDefaultRelations();
    }
}

bool UVBFManager::SetDefaultRelationsDT(UDataTable* default_relations_table)
{
    TArray<FFactionRelationsDT*> table_data;
    default_relations_table->GetAllRows<FFactionRelationsDT>("UVBFManager::SetDefaultRelations", table_data);

    if (table_data.IsEmpty())
    {
        return false;
    }

    for (const auto& row : table_data)
    {
        bool result = CreateFactionRelationsEntry(row->faction_l, row->faction_r, row->default_relations);

        if (result == false)
        {
            checkf(false, TEXT("UVBFManager::SetDefaultRelationsDT: couldn't create relations entry. It's likely DT has duplicates."));
            continue;
        }
    
        if (row->relations_locked == true)
        {
            SetFactionRelationsLocked(row->faction_l, row->faction_r, row->relations_locked);
        }
    }

    //if (ptr_row != nullptr)
    //{
    //    inventory_item_info = ptr_row->inventory_item_info;
    //    return true;
    //}

    return false;
}

int32 UVBFManager::GetFactionRelationsPair(const UVBFFaction* faction_l, const UVBFFaction* faction_r) const
{
    if (faction_l != nullptr && faction_r != nullptr)
    {
        return FactionRelations.FindRef(GetFactionRelationsMapKey(faction_l, faction_r));
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

bool UVBFManager::CreateFactionRelationsEntry(const UVBFFaction* faction_l, const UVBFFaction* faction_r, int32 relations)
{
    if (faction_l == nullptr && faction_r == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UVBFManager::CreateFactionRelationsEntry: invalid ptr"));
        return false;
    }
    if (relations > GetRelationsValueMax() || relations < GetRelationsValueMin())
    {
        UE_LOG(LogTemp, Error, TEXT("UVBFManager::CreateFactionRelationsEntry: relations value out of valid range"));
        return false;
    }

    auto entry = FactionRelations.Find(GetFactionRelationsMapKey(faction_l, faction_r));

    if (entry != nullptr)
    {
        //Map entry doesn't exist
        UE_LOG(LogTemp, Error, TEXT("UVBFManager::CreateFactionRelationsEntry: map entry already exists"));
        return false;
    }

    FactionRelations.Add(GetFactionRelationsMapKey(faction_l, faction_r)) = relations;

    return true;
}

bool UVBFManager::SetFactionRelations(const UVBFFaction* faction_l, const UVBFFaction* faction_r, int32 relations)
{

   if (faction_l == nullptr && faction_r == nullptr)
   {
       UE_LOG(LogTemp, Error, TEXT("UVBFManager::SetFactionRelations: invalid ptr"));
       return false;
   }

   if (IsFactionRelationsLocked(faction_l, faction_r) == true)
   {
       return false;
   }

   if (relations > GetRelationsValueMax() || relations < GetRelationsValueMin())
   {
       UE_LOG(LogTemp, Error, TEXT("UVBFManager::SetFactionRelations: relations value out of valid range"));
       return false;
   }

   auto entry = FactionRelations.Find(GetFactionRelationsMapKey(faction_l, faction_r));

   if (entry == nullptr)
   {
       //Map entry doesn't exist
       UE_LOG(LogTemp, Warning, TEXT("UVBFManager::SetFactionRelations: map entry doesn't exist"));
       return false;
   }

   *entry = relations;

   return true;
}

bool UVBFManager::ChangeFactionRelations(const UVBFFaction* faction_l, const UVBFFaction* faction_r, int32 relations_change)
{
    if (faction_l == nullptr && faction_r == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UVBFManager::ChangeFactionRelations: invalid ptr"));
        return false;
    }
    
    if (IsFactionRelationsLocked(faction_l, faction_r) == true)
    {
        return false;
    }

    auto* value_ptr = FactionRelations.Find(GetFactionRelationsMapKey(faction_l, faction_r));

    if (value_ptr == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UVBFManager::ChangeFactionRelations: relations entry doesn't exist"));
        return false;
    }

    if ((*value_ptr + relations_change) < GetRelationsValueMin())
    {
        *value_ptr = GetRelationsValueMin();
        return true;
    }

    if ((*value_ptr + relations_change) > GetRelationsValueMax())
    {
        *value_ptr = GetRelationsValueMax();
        return true;
    }

    *value_ptr += relations_change;

    return true;
}

bool UVBFManager::RemoveFactionRelations(const UVBFFaction* faction_l, const UVBFFaction* faction_r)
{
    return FactionRelations.Remove(GetFactionRelationsMapKey(faction_l, faction_r))
    != 0;
}

void UVBFManager::SetFactionRelationsLocked(const UVBFFaction* faction_l, const UVBFFaction* faction_r, bool locked)
{
    if (locked)
    {
        FactionRelationsLocked.Add(GetFactionRelationsMapKey(faction_l, faction_r));
    }
    else
    {
        FactionRelationsLocked.Remove(GetFactionRelationsMapKey(faction_l, faction_r));
    }
}

bool UVBFManager::IsFactionRelationsLocked(const UVBFFaction* faction_l, const UVBFFaction* faction_r)
{
    return FactionRelationsLocked.Contains(GetFactionRelationsMapKey(faction_l, faction_r));
}

const TPair<UVBFManager::FactionIdMin, UVBFManager::FactionIdMax> UVBFManager::GetFactionRelationsMapKey(const UVBFFaction* faction_l, const UVBFFaction* faction_r) const
{
    return { FMath::Min(faction_l->GetFactionId(), faction_r->GetFactionId()), FMath::Max(faction_l->GetFactionId(), faction_r->GetFactionId()) };
}
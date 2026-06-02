// Nikita Belov, All rights reserved


#include "InventoryComponent.h"
#include "Item/ItemBase.h"
#include "InvSpecialSlot.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "HAL/CriticalSection.h"
#include "Async/ParallelFor.h"

#include "Profiler/Profiler.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
    SetupDefaults();
}

void UInventoryComponent::Initialize(FIntPoint dimensions, FName name)
{
    if (IsGridInitialized() == true)
    {
        checkf(false, TEXT("%s object: InventoryGrid has been initialized already!"), *InventoryName.ToString());
        return;
    }

    FScopeLock Lock(&InventoryMutex);

    Rows = dimensions.X;
    Columns = dimensions.Y;

    InventoryName = name;

    InitializeInventoryGrid();
}

//void UInventoryComponent::Initialize(FIntPoint dimensions, FName name, float drop_distance)
//{
//    if (IsGridInitialized() == true)
//    {
//        checkf(false, TEXT("%s object: InventoryGrid has been initialized already!"), *InventoryName.ToString());
//        return;
//    }
//
//    Rows = dimensions.X;
//    Columns = dimensions.Y;
//
//    InventoryName = name;
//
//    DropDistance = drop_distance;
//}

bool UInventoryComponent::IsGridInitialized() const
{
    return InventoryGrid.Num() != 0;
}

bool UInventoryComponent::MoveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell, bool widget_rotation)
{
    bool result = false;

    FIntPoint prev_right_left_cell = item->GetLowerRightCell();
    
    bool was_rotated = false;

    was_rotated = item->GetRotated() != widget_rotation;
    if(was_rotated)
    {
        item->Rotate();
    }

    result = CheckSpaceMove(new_upper_left_cell, item, item->GetDimensions());

    if(result == true)
    {
        FillSpaceInGrid(item->GetUpperLeftCell(), prev_right_left_cell, GRID_EMPTY_SPACE);

        item->SetUpperLeftCell(new_upper_left_cell);

        FillSpaceInGrid(item->GetUpperLeftCell(), item->GetLowerRightCell(), FindItemLocalID(item));
    }

    if(was_rotated && result == false)
    {
        item->Rotate(); //rotate back
    }

    return result;
}

bool UInventoryComponent::RotateItem(UItemBase* item)
{
    bool result = false;

    FIntPoint prev_right_left_cell = item->GetLowerRightCell();

    item->Rotate();

    result = CheckSpaceMove(item->GetUpperLeftCell(), item, item->GetDimensions());

    if(result == true)
    {
        FillSpaceInGrid(item->GetUpperLeftCell(), prev_right_left_cell, GRID_EMPTY_SPACE);

        FillSpaceInGrid(item->GetUpperLeftCell(), item->GetLowerRightCell(), FindItemLocalID(item));
    }

    if(result == false)
    {
        item->Rotate(); //rotate back
    }

    return result;
}

int32 UInventoryComponent::FindItemLocalID(UItemBase* item) const
{
    return Items.Find({ item });
}

// Called when the game starts
//void UInventoryComponent::BeginPlay()
//{
//    Super::BeginPlay();
//
//    inventory_guid = CreateItemGuid();
//
//    InitializeInventoryGrid(Rows, Columns);
//}

void UInventoryComponent::PostInitProperties()
{
    Super::PostInitProperties();

    if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
    {
        inventory_guid = CreateItemGuid();

        InitializeInventoryGrid();
    }

}

bool UInventoryComponent::AddItem(UItemBase* item)
{
    if (IsGridInitialized() == false)
    {
        checkf(false, TEXT("Error: cannot use uninitialized InventoryComponent"));
        return false;
    }

    FScopeLock Lock(&InventoryMutex);

    if(!IsValid(item))
    {
        check(false);
        UE_LOG(LogTemp, Error, TEXT("AddItem: invalid item"));
        return false;
    }

    auto stack_result = TryToStack(item);
    
    if (stack_result == EItemStackResult::StackedFully)
    {
        //Just return, there is nothing else to do
        return true;
    } 
    //otherwise default item add logic


    if(FreeSpaceLeft >= (item->GetDimensions().X * item->GetDimensions().Y))
    {
        FIntPoint free_space_coords = FindFreeSpaceInGrid(item);

        bool was_rotated = false;

        if(free_space_coords == INDEX_NONE) //try rotated
        {
            was_rotated = true;
            item->Rotate();

            free_space_coords = FindFreeSpaceInGrid(item);
        }

        if(free_space_coords != INDEX_NONE && item->SetOuterUpstreamInventory(this))
        {
            UpdateFreeSpaceLeft(-1 * item->GetDimensions().X * item->GetDimensions().Y);
            int32 idx = GenerateLocalInvIndex();
            Items.Add({ item, idx });

            ChangeMass(item->GetMassTotal());

            item->SetUpperLeftCell(free_space_coords);

            FillSpaceInGrid(item->GetUpperLeftCell(), item->GetLowerRightCell(), idx);

            item->World = GetWorld();

            OnInventoryUpdated.Broadcast();
            return true;
        }
        else if(was_rotated == true)
        {
            item->Rotate(); //rotate back
            return false;
        }
        else
        {
            return false;
        }
    }

    return false;
}

bool UInventoryComponent::AddItemAt(UItemBase* item, FIntPoint new_upper_left_cell)
{
    if (IsGridInitialized() == false)
    {
        checkf(false, TEXT("Error: cannot use uninitialized InventoryComponent"));
        return false;
    }

    FScopeLock Lock(&InventoryMutex);

    if(!IsValid(item))
    {
        UE_LOG(LogTemp, Error, TEXT("AddItem: invald item"));
        return false;
    }

    auto stack_result = TryToStack(item);

    if (stack_result == EItemStackResult::StackedFully)
    {
        //Just return, there is nothing else to do
        return true;
    }
    //otherwise default item add logic


    if(FreeSpaceLeft >= (item->GetDimensions().X * item->GetDimensions().Y))
    {
        if(CheckSpace(new_upper_left_cell, item) && item->SetOuterUpstreamInventory(this))
        {
            UpdateFreeSpaceLeft(-1 * item->GetDimensions().X * item->GetDimensions().Y);
            int32 idx = GenerateLocalInvIndex();
            Items.Add({ item, idx });

            ChangeMass(item->GetMassTotal());

            item->SetUpperLeftCell(new_upper_left_cell);
            FillSpaceInGrid(item->GetUpperLeftCell(), item->GetLowerRightCell(), idx);

            item->World = GetWorld();

            OnInventoryUpdated.Broadcast();
            return true;
        }
    }

    return false;
}

bool UInventoryComponent::RemoveItem(UItemBase* item)
{
    if (IsGridInitialized() == false)
    {
        checkf(false, TEXT("Error: cannot use uninitialized InventoryComponent"));
        return false;
    }

    if(item == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("RemoveItem: invald item"));
        return false;
    }
    if(Items.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("RemoveItem: there is nothing to remove from inventory"));
        return false;
    }

    auto item_position = FindItemPosition(item); //we use find because UItemBase->GetUpperLeftCell might be overriden by another inventory
    FillSpaceInGrid(item_position.Key, item_position.Value, GRID_EMPTY_SPACE);

    UpdateFreeSpaceLeft(item->GetDimensions().X * item->GetDimensions().Y);

    free_indices.Push(FindItemLocalID(item));

    ChangeMass(-item->GetMassTotal());

    Items.RemoveSingleSwap({ item }, EAllowShrinking::Yes);

    OnInventoryUpdated.Broadcast();
    return true;
}

bool UInventoryComponent::RemoveItemAt(UItemBase* item, FIntPoint upper_left_cell, FIntPoint lower_right_cell)
{
    if (IsGridInitialized() == false)
    {
        checkf(false, TEXT("Error: cannot use uninitialized InventoryComponent"));
        return false;
    }

    if(!IsValid(item))
    {
        UE_LOG(LogTemp, Error, TEXT("RemoveItem: invalid item"));
        return false;
    }
    if(Items.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("RemoveItem: there is nothing to take from inventory!"));
        return false;
    }

    FillSpaceInGrid(upper_left_cell, lower_right_cell, GRID_EMPTY_SPACE);

    UpdateFreeSpaceLeft(item->GetDimensions().X * item->GetDimensions().Y);
    free_indices.Push(FindItemLocalID(item));

    ChangeMass(-item->GetMassTotal());

    Items.RemoveSingleSwap({ item }, EAllowShrinking::Yes);

    OnInventoryUpdated.Broadcast();
    return true;
}

void UInventoryComponent::ChangeMass(float value)
{
    FScopeLock Lock(&InventoryMutex);

    Mass += value;

    //To avoid floating point errors
    if (UKismetMathLibrary::NearlyEqual_FloatFloat(Mass, 0, FInventoryItemInfo::MassMaxPrecision))
    {
        Mass = 0;
    };

    //Switch to double if above fix isn't enough
    checkf(!(Mass < 0), TEXT("Error: UInventoryComponent: Mass is invalid. Probably this is result of missing/wrong caclulations. Or floating point error."))
}

EItemStackResult UInventoryComponent::TryToStack(UItemBase* item)
{
    //If stackable
    if (item->GetMaxStackSize() > 1)
    {
        //Check if can be added to an existing stack
        for (auto& i : Items)
        {
            if (i.Item->IsEqualForStack(item) && i.Item->GetMaxStackSize() < i.Item->GetCurrentStackSize())
            {
                if (i.Item->StackAdd(item) == false)
                {
                    return EItemStackResult::NotStacked;
                }

                if (item->GetCurrentStackSize() == 0)
                {
                    item->ConditionalBeginDestroy();

                    //Item was fully added to an existing stack
                    return EItemStackResult::StackedFully;
                }
                else //there is remainder of stack size
                {
                    return EItemStackResult::StackedPartially;
                }
            }
        }
    }

    return EItemStackResult::NotStacked;
}

void UInventoryComponent::SetupDefaults()
{
    InventoryName = "InventoryDefaultName";
    Rows = 1;
    Columns = 1;
    FreeSpaceLeft = 0;
    Mass = 0;
    DropDistance = 60;
}

void UInventoryComponent::InitializeInventoryGrid()
{
    if (Rows < 1 || Columns < 1)
    {
        checkf(false, TEXT("UInventoryComponent: Invalid dimensions"));
        return;
    }

    if(InventoryGrid.Num() != 0)
    {
        return; //Double initialization call is possible. If Initialize has been called manually and if BeginPlay->InitializeInventoryGrid
        //checkf(false, TEXT("%s object: InventoryGrid has been initialized already!"), *InventoryName.ToString() );
    }

    InventoryGrid.Init(GRID_EMPTY_SPACE, Rows * Columns);

    FreeSpaceLeft = Rows * Columns;
}

FIntPoint UInventoryComponent::FindFreeSpaceInGrid(UItemBase* item) const
{
    //LOG_DURATION(LogDurationChronoUnit::nanoseconds, __FUNCTION__);

    auto find = [](int32 first, int32 last, const TArray<int32>& container, int32 value)
    {
        for(; first <= last; ++first)
        {
            if(container[first] == value)
            {
                return first;
            }
        }
        return (int32)INDEX_NONE;
    };

    for(int32 row_idx = 0; row_idx < Rows; row_idx++)
    {
        int32 first_col_idx = 0;
        const int32 last_col_idx = Columns - 1;

        while (first_col_idx != INDEX_NONE && first_col_idx < Columns)
        {
            auto arr_idx = find(GridToArrayIndex(row_idx, first_col_idx), GridToArrayIndex(row_idx, last_col_idx), InventoryGrid, GRID_EMPTY_SPACE);

            first_col_idx = ArrayIndexToGrid(arr_idx).Y;

            if (first_col_idx != INDEX_NONE)
            {
                if (CheckSpace({ row_idx, first_col_idx }, item))
                {
                    return { row_idx, first_col_idx };
                }
                else
                {
                    first_col_idx++;
                }
            }
        }

    }

    return { INDEX_NONE, INDEX_NONE };
}

TPair<FIntPoint, FIntPoint> UInventoryComponent::FindItemPosition(UItemBase* item) const
{
    int32 item_idx = FindItemLocalID(item);

    FIntPoint upper_left_cell{};
    //for(size_t i = 0; i < InventoryGrid.Num(); i++) {}
     
    //Has to find first entry, otherwise will break
    int32 grid_idx = InventoryGrid.Find(item_idx);

    if (grid_idx != INDEX_NONE)
    {
        upper_left_cell.X = ArrayIndexToGrid(grid_idx).X;
        upper_left_cell.Y = ArrayIndexToGrid(grid_idx).Y;
        FIntPoint lower_right_cell{ upper_left_cell.X + item->GetDimensions().X - 1, upper_left_cell.Y + item->GetDimensions().Y - 1 };
        return TPair<FIntPoint, FIntPoint>{ upper_left_cell, lower_right_cell };
    }
    
    return {};
}

bool UInventoryComponent::CheckSpace(FIntPoint upper_left_cell, UItemBase* item) const
{
    if (IsGridInitialized() == false)
    {
        checkf(false, TEXT("Error: cannot use uninitialized InventoryComponent"));
        return false;
    }

    //whether valid cell idx at all 
    if(upper_left_cell.X < 0 || upper_left_cell.Y < 0 || upper_left_cell.X > Rows - 1 || upper_left_cell.Y > Columns - 1)
    {
        return false;
    }

    //wether can even fit within inventory dimensions with such starting cell
    FIntPoint new_lower_right_cell{ upper_left_cell.X + item->GetDimensions().X - 1, upper_left_cell.Y + item->GetDimensions().Y - 1 };

    if(new_lower_right_cell.X >= Rows || new_lower_right_cell.Y >= Columns)
    {
        return false;
    }

    for(int32 i = upper_left_cell.X; i <= new_lower_right_cell.X; i++)
    {
        for(int32 j = upper_left_cell.Y; j <= new_lower_right_cell.Y; j++)
        {
            if(InventoryGrid[GridToArrayIndex(i, j)] != GRID_EMPTY_SPACE)
            {
                return false;
            }
        }
    }

    return true;

}

bool UInventoryComponent::CheckSpaceMove(const FIntPoint upper_left_cell, UItemBase* item, FIntPoint dimensions)
{
    if (IsGridInitialized() == false)
    {
        checkf(false, TEXT("Error: cannot use uninitialized InventoryComponent"));
        return false;
    }

    if (CheckSelfRecursion(item))
    {
        return false;
    }

    
    FScopeLock Lock(&InventoryMutex);

    //whether valid cell idx at all 
    if(upper_left_cell.X < 0 || upper_left_cell.Y < 0 || upper_left_cell.X > Rows - 1 || upper_left_cell.Y > Columns - 1)
    {
        return false;
    }

    FIntPoint new_lower_right_cell {upper_left_cell.X + dimensions.X - 1, upper_left_cell.Y + dimensions.Y - 1};

    //whether can even fit within inventory dimensions with such starting cell
    if(new_lower_right_cell.X >= Rows || new_lower_right_cell.Y >= Columns)
    {
        return false;
    }

    //UE_LOG(LogTemp, Warning, TEXT("CheckSpaceMove: new_lower_right_cell: X: %i, Y: %i "), new_lower_right_cell.X, new_lower_right_cell.Y)

    auto local_idx = FindItemLocalID(item);

    //check for item idx and empty space
    if(local_idx != INDEX_NONE) 
    {
        for(int32 i = upper_left_cell.X; i <= new_lower_right_cell.X; i++)
        {
            for(int32 j = upper_left_cell.Y; j <= new_lower_right_cell.Y; j++)
            {
                if(InventoryGrid[GridToArrayIndex(i, j)] != local_idx && InventoryGrid[GridToArrayIndex(i, j)] != GRID_EMPTY_SPACE)
                {
                    return false;
                }
            }
        }

    }
    else //check for empty space only
    {
        for(int32 i = upper_left_cell.X; i <= new_lower_right_cell.X; i++)
        {
            for(int32 j = upper_left_cell.Y; j <= new_lower_right_cell.Y; j++)
            {
                if(InventoryGrid[GridToArrayIndex(i, j)] != GRID_EMPTY_SPACE)
                {
                    return false;
                }
            }
        }

    }

    return true;
}

void UInventoryComponent::FillSpaceInGrid(FIntPoint upper_left_cell, FIntPoint lower_right_cell, int32 item_idx)
{
    FScopeLock Lock(&InventoryMutex);

    for(int32 row_idx = upper_left_cell.X; row_idx <= lower_right_cell.X; row_idx++)
    {
        //TODO: Multithread maybe
        for(int32 col_idx = upper_left_cell.Y; col_idx <= lower_right_cell.Y; col_idx++)
        {
            InventoryGrid[GridToArrayIndex(row_idx, col_idx)] = item_idx;
        }
    }
}

void UInventoryComponent::UpdateFreeSpaceLeft(int32 space_change)
{
    FScopeLock Lock(&InventoryMutex);

    FreeSpaceLeft += space_change;
}

int32 UInventoryComponent::GenerateLocalInvIndex()
{
    FScopeLock Lock(&InventoryMutex);

    if(free_indices.Num() > 0)
    {
        return free_indices.Pop();
    }
    else
    {
        return Items.Num();
    }       
}

bool UInventoryComponent::CheckSelfRecursion(UItemBase* item) const
{
    bool result = false;
    UInventoryManager* manager = Cast<UInventoryManager>(GetOuter());

    //if owned by InventoryManager
    if (manager != nullptr) 
    {
        //check who owns manager
        result = Cast<UItemBase>(manager->GetOuter()) == item;
    }

    if (result == false)
    {
        //If inventory component is directly owned by Item
        Cast<UItemBase>(GetOuter());
    }

    return result;
}


int32 UInventoryComponent::GridToArrayIndex(int32 row_idx, int32 col_idx) const
{
    if (row_idx < 0 || col_idx < 0)
    {
        return INDEX_NONE;
    }
    return row_idx * Columns + col_idx;
}

FIntPoint UInventoryComponent::ArrayIndexToGrid(int32 idx) const
{
    //Rows, columns
    if (idx < 0)
    {
        return { INDEX_NONE, INDEX_NONE };
    }
    return { idx / Columns, idx % Columns };
}

//void UInventoryComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//    Super::PostEditChangeProperty(PropertyChangedEvent);
//
//    //It's only to avoid assertion fail
//}

bool UInventoryComponent::Contains(UItemBase* item) const
{
    return Items.Contains(FInventoryGridItemWrapper{item});
}

void UInventoryComponent::PrintInventory()
{
    //for(int32 i = InventoryGrid.Num() - 1; i != -1; i--)
    //{
    //    int32 symbols_added = 0; //how many symbols a number got converted into
    //    int32 prev_length = 0;
    //    FString str;
    //    for(const int32& j : InventoryGrid[i])
    //    {
    //        prev_length = str.Len();
    //        str.AppendInt(j);
    //        symbols_added = str.Len() - prev_length;
    //        if(symbols_added == 1)
    //        {
    //            str.Append(" ");
    //            str.Append(" ");
    //            str.Append(" ");
    //            str.Append(" ");
    //        }
    //        else if(symbols_added == 2)
    //        {
    //            str.Append(" ");
    //            str.Append(" ");
    //            str.Append(" ");
    //        }
    //        else
    //        {
    //            str.Append(" "); //1 space
    //            str.Append(" "); //1 space
    //        }

    //    }
    //    UKismetSystemLibrary::PrintString(GetWorld(), str, true, false, FColor::White, 5);
    //}

}

void UInventoryComponent::PrintDebugInfo()
{
    FString str;
    str.Append("InventoryName: ");
    str.Append(InventoryName.ToString());
    str.Append("\n");
    str.Append("Rows: ");
    str.AppendInt(Rows);
    str.Append("  Columns: ");
    str.AppendInt(Columns);
    str.Append("\n");
    str.Append("FreeSpaceLeft: ");
    str.AppendInt(FreeSpaceLeft);
    str.Append("\n");
    str.Append("Mass: ");
    str.AppendInt(Mass);
    str.Append("\n");
    str.Append("DropDistance: ");
    str.AppendInt(DropDistance);
    str.Append("\n");

    UKismetSystemLibrary::PrintString(GetOuter()->GetWorld(), str, true, false, FColor::White, 5);
}

//bool UInventoryComponent::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination)
//{
//    if (IsGridInitialized() == false)
//    {
//        checkf(false, TEXT("Error: cannot use uninitialized InventoryComponent"));
//        return false;
//    }
//
//    if(destination.GetObject() == this)
//    {
//        checkf(false, TEXT("Warning: can't move item to itself"));
//        return false;
//    }
//
//    auto temp_upper_l_cell = item->GetUpperLeftCell();
//    auto temp_lower_r_cell = item->GetLowerRightCell();
//
//    if(item != nullptr && Contains(item) && destination->ReceiveItem(item))
//    {
//        RemoveItemAt(item, temp_upper_l_cell, temp_lower_r_cell);
//        return true;
//    }
//
//    return false;
//}

bool UInventoryComponent::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
{
    if (IsGridInitialized() == false)
    {
        checkf(false, TEXT("Error: cannot use uninitialized InventoryComponent"));
        return false;
    }

    //TODO: test
    if(destination.GetObject() == this)
    {
        return false;
    }

    auto temp_upper_l_cell = item->GetUpperLeftCell();
    auto temp_lower_r_cell = item->GetLowerRightCell();

    if(item != nullptr && Contains(item) && destination->ReceiveItem(item, new_upper_left_cell))
    {
        RemoveItemAt(item, temp_upper_l_cell, temp_lower_r_cell);
        return true;
    }

    return false;
}

bool UInventoryComponent::AddItemFromWorld(UItemBase* item)
{
    if (item == nullptr)
    {
        return false;
    }
    if (AddItem(item) == false)
    {
        return false;
    }

    //item->ItemActor->Destroy(); //currently ItemActors destroy themselves

    return true;
}

bool UInventoryComponent::DropItemToWorld(UItemBase* item)
{
    if(item == nullptr || Contains(item) == false)
    {
        return false;
    }

    AActor* owner_actor = nullptr;
    UInventoryManager* inv_manager = nullptr;
    float EndDropDistance = 0;

    if (GetInventoryOwner()->IsA(UInventoryManager::StaticClass()))
    {
        inv_manager = Cast<UInventoryManager>(GetInventoryOwner());
        owner_actor = Cast<AActor>(inv_manager->GetOwner());

        if (DropDistanceOverride)
        {
            EndDropDistance = DropDistance;
        }
        else
        {
            EndDropDistance = inv_manager->GetDropDistance();
        }
    }
    else
    {
        owner_actor = Cast<AActor>(GetInventoryOwner());

        EndDropDistance = DropDistance;
    }

    if (owner_actor == nullptr)
    {
        //Either owner is invalid
        //or it's not Actor at all - thus doesn't know what "World" is
        return false;
    }

    FItemActorSpawnParameters spawn_params;
    spawn_params.MoveOwnershipItemObject = true;
    spawn_params.EnablePhysics = true;
    spawn_params.Interactible = true;

    auto* item_actor = item->SpawnItemActor(owner_actor->GetActorLocation() + owner_actor->GetActorForwardVector() * EndDropDistance, owner_actor->GetActorRotation(), spawn_params);
    if(IsValid(item_actor) == false)
    {
        //can't spawn, do not delete from inventory
        //UKismetSystemLibrary::PrintString(GetWorld(), "Actor failed to spawn. Probably because it collides with something.", true, true, FLinearColor(130, 5, 255), 4);
        return false;
    }

    item->SetOuterItemActor(item_actor);

    RemoveItemAt(item, item->GetUpperLeftCell(), item->GetLowerRightCell());

    return true;
}

bool UInventoryComponent::ReceiveItem(UItemBase* item, FIntPoint new_upper_left_cell)
{
    if(!IsValid(item) || Contains(item))
    {
        return false;
    }

    if (new_upper_left_cell.X <= -1 || new_upper_left_cell.Y <= -1)
    {
        return AddItem(item);
    }

    return AddItemAt(item, new_upper_left_cell);
}

void UInventoryComponent::UpdateStackDependencies(UItemBase* item, int32 new_stack_size)
{
    if(item == nullptr)
    {
        checkf(false, TEXT("Error: UpdateStackDependencies: Invalid item"));
        return;
    }

    if(Contains(item) == false)
    {
        checkf(false, TEXT("Error: UpdateStackDependencies: Item doesn't belong to this inventory"));
        return;
    }

    //0 is valid input, just make sure the Item is removed after
    if(new_stack_size < 0)
    {
        checkf(false, TEXT("Error: UpdateStackDependencies: Invalid new_stack_size"));
        return;
    }

    ChangeMass(item->GetMassOneUnit() * new_stack_size - item->GetMassTotal());
}


//UItemBase* UInventoryComponent::GetOuterItem() const
//{
//    return Cast<UItemBase>(GetOuter());
//}

bool UInventoryComponent::SetInventoryOwner(UObject* new_owner)
{
    if (IsValid(new_owner) == false)
    {
        checkf(false, TEXT("New owner is not valid"))
        return false;
    }
    
    if (Rename(nullptr, new_owner) == false)
    {
        return false;
    }

    //GetInventoryOwner()->RemoveOwnedComponent(this);

    //if (auto new_owner_actor = Cast<AActor>(new_owner))
    //{
    //    new_owner_actor->AddOwnedComponent(this);
    //}
    //
    return true;
}

//TScriptInterface<IInventoryInterface> UInventoryComponent::GetOuterUpstreamInventory() const
//{
//    return GetOuter();
//}

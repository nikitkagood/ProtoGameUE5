// Nikita Belov, All rights reserved


#include "InventoryComponent.h"
#include "Item/ItemBase.h"
#include "InvSpecialSlot.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "HAL/CriticalSection.h"

#include "Profiler/Profiler.h"

constexpr int32 EMPTY_SPACE = -1;

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
    SetupDefaults();

    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.TickInterval = 0.04f; //25hz; if we ever need it

    Rows = 1;
    Columns = 1;
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

    InitializeInventoryGrid(Rows, Columns);
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
        FillSpaceInGrid(item->GetUpperLeftCell(), prev_right_left_cell, EMPTY_SPACE);

        item->SetUpperLeftCell(new_upper_left_cell);

        FillSpaceInGrid(item->GetUpperLeftCell(), item->GetLowerRightCell(), Items[item]);
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
        FillSpaceInGrid(item->GetUpperLeftCell(), prev_right_left_cell, EMPTY_SPACE);

        FillSpaceInGrid(item->GetUpperLeftCell(), item->GetLowerRightCell(), Items[item]);
    }

    if(result == false)
    {
        item->Rotate(); //rotate back
    }

    return result;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    InitializeInventoryGrid(Rows, Columns);
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

    //If stackable
    if(item->GetMaxStackSize() > 1)
    {
        //Check if can be added to an existing stack

        for(auto& i : Items)
        {
            if(*i.Key == *item)
            {
                i.Key->StackAdd(item);

                if(item->GetCurrentStackSize() == 0)
                {
                    item->ConditionalBeginDestroy();

                    //Item fully added to an existing stack
                    return true;
                }
            }
        }
    }


    if(FreeSpaceLeft >= (item->GetDimensions().X * item->GetDimensions().Y))
    {
        constexpr int32 NONE = -9999;
        FIntPoint free_space_coords = FindFreeSpaceInGrid(item);

        bool was_rotated = false;

        if(free_space_coords == NONE) //try rotated
        {
            was_rotated = true;
            item->Rotate();

            free_space_coords = FindFreeSpaceInGrid(item);
        }

        if(free_space_coords != NONE && item->SetOuterUpstreamInventory(this))
        {
            UpdateFreeSpaceLeft(-1 * item->GetDimensions().X * item->GetDimensions().Y);
            int32 idx = GenerateIndex();
            Items.Add(item, idx);

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

    //If stackable
    if(item->GetMaxStackSize() > 1)
    {
        //Check if can be added to an existing stack

        bool item_filled_existing_stacks = false;

        for(auto& i : Items)
        {
            if(*i.Key == *item)
            {
                i.Key->StackAdd(item);

                if(item->GetCurrentStackSize() == 0)
                {
                    item->ConditionalBeginDestroy();

                    item_filled_existing_stacks = true;
                    return true;
                }
            }
        }
    }


    if(FreeSpaceLeft >= (item->GetDimensions().X * item->GetDimensions().Y))
    {
        if(CheckSpace(new_upper_left_cell, item) && item->SetOuterUpstreamInventory(this))
        {
            UpdateFreeSpaceLeft(-1 * item->GetDimensions().X * item->GetDimensions().Y);
            int32 idx = GenerateIndex();
            Items.Add(item, idx);

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
    FillSpaceInGrid(item_position.Key, item_position.Value, EMPTY_SPACE);

    UpdateFreeSpaceLeft(item->GetDimensions().X * item->GetDimensions().Y);
    free_indices.Push(Items[item]);

    ChangeMass(-item->GetMassTotal());

    Items.Remove(item);

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

    FillSpaceInGrid(upper_left_cell, lower_right_cell, EMPTY_SPACE);

    UpdateFreeSpaceLeft(item->GetDimensions().X * item->GetDimensions().Y);
    free_indices.Push(Items[item]);

    ChangeMass(-item->GetMassTotal());

    Items.Remove(item);

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

void UInventoryComponent::SetupDefaults()
{
    InventoryName = "InventoryDefaultName";
    Rows = 1;
    Columns = 1;
    FreeSpaceLeft = 0;
    Mass = 0;
    DropDistance = 60;
}

void UInventoryComponent::InitializeInventoryGrid(int32 rows, int32 columns)
{
    if (rows < 1 || columns < 1)
    {
        checkf(false, TEXT("Invalid dimensions"));
        return;
    }

    if(InventoryGrid.Num() != 0)
    {
        return; //Double initialization call is possible. If Initialize has been called manually and if BeginPlay->InitializeInventoryGrid
        //checkf(false, TEXT("%s object: InventoryGrid has been initialized already!"), *InventoryName.ToString() );
    }

    InventoryGrid.Reserve(rows);

    ParallelFor(rows, [&](int32 idx) 
        {
            InventoryGrid[idx].Init(EMPTY_SPACE, columns);
        });

    FreeSpaceLeft = Rows * Columns;
}

FIntPoint UInventoryComponent::FindFreeSpaceInGrid(UItemBase* item) const
{
    //LOG_DURATION(LogDurationChronoUnit::nanoseconds, __FUNCTION__);

    constexpr int32 NONE = -9999;
    auto find = [NONE](int32 first, int32 last, const TArray<int32>& container, int32 value)
    {
        for(; first <= last; ++first)
        {
            if(container[first] == value)
            {
                return first;
            }
        }
        return NONE;
    };

    for(int32 row_idx = 0; row_idx < InventoryGrid.Num(); row_idx++)
    {
        int32 first_col_idx = 0;
        int32 last_col_idx = InventoryGrid[row_idx].Num() - 1;

        while(first_col_idx != NONE)
        {
            first_col_idx = find(first_col_idx, last_col_idx, InventoryGrid[row_idx], -1);
            
            if(first_col_idx != NONE)
            {
                if(CheckSpace({ row_idx, first_col_idx }, item))
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

    return { NONE, NONE };
}

TPair<FIntPoint, FIntPoint> UInventoryComponent::FindItemPosition(UItemBase* item) const
{
    int32 item_idx = Items[item];

    FIntPoint upper_left_cell;
    for(size_t i = 0; i < InventoryGrid.Num(); i++)
    {
        upper_left_cell.X = InventoryGrid[i].Find(item_idx);

        if(upper_left_cell.X != INDEX_NONE)
        {
            upper_left_cell.Y = i;
            FIntPoint lower_right_cell { upper_left_cell.X + item->GetDimensions().X - 1, upper_left_cell.Y + item->GetDimensions().Y - 1 };
            return TPair<FIntPoint, FIntPoint>{ upper_left_cell, lower_right_cell };
        }
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
            if(InventoryGrid[i][j] != EMPTY_SPACE)
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

    if(Contains(item) == true) //check for item idx and empty space
    {
        for(int32 i = upper_left_cell.X; i <= new_lower_right_cell.X; i++)
        {
            for(int32 j = upper_left_cell.Y; j <= new_lower_right_cell.Y; j++)
            {
                if(InventoryGrid[i][j] != Items[item] && InventoryGrid[i][j] != EMPTY_SPACE)
                {
                    return false;
                }
            }
        }

    }
    else //check for empty space only (checking for item idx for non-existent item will result in exception)
    {
        for(int32 i = upper_left_cell.X; i <= new_lower_right_cell.X; i++)
        {
            for(int32 j = upper_left_cell.Y; j <= new_lower_right_cell.Y; j++)
            {
                if(InventoryGrid[i][j] != EMPTY_SPACE)
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

    for(int32 row = upper_left_cell.X; row <= lower_right_cell.X; row++)
    {
        //TODO: Multithread maybe
        for(int32 column = upper_left_cell.Y; column <= lower_right_cell.Y; column++)
        {
            InventoryGrid[row][column] = item_idx;
        }
    }
}

void UInventoryComponent::UpdateFreeSpaceLeft(int32 space_change)
{
    FScopeLock Lock(&InventoryMutex);

    FreeSpaceLeft += space_change;
}

int32 UInventoryComponent::GenerateIndex()
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
    return Cast<UItemBase>(GetOuter()) == item;
}

bool UInventoryComponent::Contains(UItemBase* item) const
{
    return Items.Contains(item);
}

void UInventoryComponent::PrintInventory()
{
    for(int32 i = InventoryGrid.Num() - 1; i != -1; i--)
    {
        int32 symbols_added = 0; //how many symbols a number got converted into
        int32 prev_length = 0;
        FString str;
        for(const int32& j : InventoryGrid[i])
        {
            prev_length = str.Len();
            str.AppendInt(j);
            symbols_added = str.Len() - prev_length;
            if(symbols_added == 1)
            {
                str.Append(" ");
                str.Append(" ");
                str.Append(" ");
                str.Append(" ");
            }
            else if(symbols_added == 2)
            {
                str.Append(" ");
                str.Append(" ");
                str.Append(" ");
            }
            else
            {
                str.Append(" "); //1 space
                str.Append(" "); //1 space
            }

        }
        UKismetSystemLibrary::PrintString(GetWorld(), str, true, false, FColor::White, 5);
    }

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

    UKismetSystemLibrary::PrintString(GetWorld(), str, true, false, FColor::White, 5);
}

bool UInventoryComponent::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination)
{
    if (IsGridInitialized() == false)
    {
        checkf(false, TEXT("Error: cannot use uninitialized InventoryComponent"));
        return false;
    }

    if(destination.GetObject() == this)
    {
        checkf(false, TEXT("Warning: can't move item to itself"));
        return false;
    }

    auto temp_upper_l_cell = item->GetUpperLeftCell();
    auto temp_lower_r_cell = item->GetLowerRightCell();

    if(item != nullptr && Contains(item) && destination->ReceiveItem(item))
    {
        RemoveItemAt(item, temp_upper_l_cell, temp_lower_r_cell);
        return true;
    }

    return false;
}

bool UInventoryComponent::MoveItemToInventoryDestination(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
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

    if(item != nullptr && Contains(item) && destination->ReceiveItemInGrid(item, new_upper_left_cell))
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

    auto* item_actor = item->SpawnItemActor(GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * DropDistance, GetOwner()->GetActorRotation());
    if(IsValid(item_actor) == false)
    {
        //can't spawn, do not delete from inventory
        UKismetSystemLibrary::PrintString(GetWorld(), "Actor failed to spawn. Probably because it collides with something.", true, true, FLinearColor(130, 5, 255), 4);
        return false;
    }

    item->SetOuterItemActor(item_actor);

    RemoveItemAt(item, item->GetUpperLeftCell(), item->GetLowerRightCell());

    return true;
}

bool UInventoryComponent::ReceiveItem(UItemBase* item)
{
    if(!IsValid(item) || Items.Contains(item))
    {
        return false;
    }

    return AddItem(item);
}

bool UInventoryComponent::ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell)
{
    if(!IsValid(item) || Items.Contains(item))
    {
        return false;
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

    GetOwner()->RemoveOwnedComponent(this);

    if (auto new_owner_actor = Cast<AActor>(new_owner))
    {
        new_owner_actor->AddOwnedComponent(this);
    }
    
    return true;
}

//TScriptInterface<IInventoryInterface> UInventoryComponent::GetOuterUpstreamInventory() const
//{
//    return GetOuter();
//}

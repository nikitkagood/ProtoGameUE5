// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "VBFUnitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UVBFUnitInterface : public UInterface
{
	GENERATED_BODY()
};

class PROTOGAME_API IVBFUnitInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Get")
	FText GetName() const;															 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Get")
	FVector GetLocation() const;													 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Get")
	FRotator GetRotation() const;													 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Get")
	double GetSpeed() const;														 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Get")
	double GetMaxSpeed() const;														 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Set")
	bool SetName(const FText& name);												 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Set")
	bool SetLocation(const FVector& new_position);									 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Set")
	bool SetRotation(const FRotator& new_rotation);									 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Set")
	bool SetSpeed();

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	//bool GetCustomValue(const FString& value_name);

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	//bool SetCustomValue(const FString& value_name, FTuple value);

	//Utility

	//Destroy this unit for gameplay purposes
	//Cannot fail
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Utility")
	void DestroyUnit();

	//Override to set unit actor class
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Utility")
	TSubclassOf<AActor> GetUnitActorClass();

	//Only world representation (i.e. actor) is affected, should not affect unit for gameplay purposes
	//In BP there is no easy way to get World, so it comes from Unit Object
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Utility")
	bool SpawnUnitActor(const FTransform& transform, FDataTableRowHandle handle, UWorld* world_optional = nullptr);

	//Only world representation (i.e. actor) is affected, should not affect unit for gameplay purposes
	//Cannot fail
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Utility")
	void DespawnUnitActor();

	//What to display in UI
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Utility")
	TArray<FString> GetDisplayData();

	//False if unit is purely virtual
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Utility")
	bool IsInWorld() const;

	//Commands

	//Only tries to reach destination, not guaranteed
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Commands")
	bool TryMoveTo(FVector location);												 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Commands")
	bool Stop();																	 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Commands")
	bool MoveAndAttack(FVector location);											 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Commands")
	bool Attack(UObject* target);													 
																					 
	//Gameplay																		 
																					 
	//Won't be selectable, won't take commands (ever)								 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Gameplay")
	bool IsEverCommandable() const;													 
																					 
	bool IsEverCommandable_Implementation() const { return true; };					 
																					 
	//Won't be selectable, won't take commands (now)								 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Gameplay")
	bool IsCommandable() const;														 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Gameplay")
	bool IsEverMovable() const;														 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Gameplay")
	bool CanMove() const;															 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Gameplay")
	bool CanEverAttack() const;														 
																					 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IVBFUnitInterface Gameplay")
	bool CanAttack() const;


	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Gameplay")
	//bool ReceiveDamage();

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Gameplay")
	//bool SetEngagementRules();

	//TODO: is not actually implemented, just an idea
	// -optional_namespace_ID: to avoid command_name collisions
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Commands")
	//bool IssueCustomCommand(const FString& command_name, int64 optional_namespace_ID, FTuple optional_params);
};

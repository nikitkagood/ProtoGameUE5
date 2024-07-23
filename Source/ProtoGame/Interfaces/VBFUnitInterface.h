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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	FText GetName() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	FVector GetLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	FRotator GetRotation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	double GetSpeed() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	double GetMaxSpeed() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	bool SetName(const FText& name);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	bool SetLocation(const FVector& new_position);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	bool SetRotation(const FRotator& new_rotation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	bool SetSpeed();

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	//bool GetCustomValue(const FString& value_name);

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GetSet")
	//bool SetCustomValue(const FString& value_name, FTuple value);

	//Utility

	//Destroy this unit for gameplay purposes
	//Cannot fail
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Utility")
	void DestroyUnit();

	//Override to set unit actor class
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Utility")
	TSubclassOf<AActor> GetUnitActorClass();

	//Only world representation (i.e. actor) is affected, should not affect unit for gameplay purposes
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Utility")
	bool SpawnUnitActor(UWorld* world, const FVector& location, const FRotator& rotation);

	//Only world representation (i.e. actor) is affected, should not affect unit for gameplay purposes
	//Cannot fail
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Utility")
	void DespawnUnitActor();

	//What to display in UI
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Utility")
	TArray<FString> GetDisplayData();

	//Commands

	//Only tries to reach destination, not guaranteed
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Commands")
	bool TryMoveTo(FVector location);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Commands")
	bool Stop();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Commands")
	bool MoveAndAttack(FVector location);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Commands")
	bool Attack(UObject* target);

	//Gameplay
	
	//Won't be selectable, won't take commands
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Gameplay")
	bool IsEverCommandable() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Gameplay")
	bool IsEverMovable() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Gameplay")
	bool CanMove() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Gameplay")
	bool CanEverAttack() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Gameplay")
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

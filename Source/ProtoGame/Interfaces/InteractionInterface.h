// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InteractionInterface.generated.h"

//Right now this is used purely visually - for outlines with custom depth stencil
//Do not change int values
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None = 0					UMETA(DisplayName = "None"),
	Item = 1					UMETA(DisplayName = "Item"),
	WorldInteractible = 2		UMETA(DisplayName = "World Interactible"),
	Quest = 3					UMETA(DisplayName = "Quest"),
};

//UENUM(BlueprintType, meta = (Bitflags))
UENUM(BlueprintType)
enum class EInteractionActions : uint8
{
	Unspecified	= 0				UMETA(DisplayName = "Unspecified"),
	Use							UMETA(DisplayName = "Use"),
	Consume						UMETA(DisplayName = "Consume"),
	Open						UMETA(DisplayName = "Open"), //both open something like door and something like crate
	Close						UMETA(DisplayName = "Close"),
	Equip						UMETA(DisplayName = "Equip"), //into inventory but into special slot
	Take 						UMETA(DisplayName = "Take"), //into inventory
};
//ENUM_CLASS_FLAGS(EInteractionActions);

UINTERFACE(Blueprintable, MinimalAPI)
class UInteractionInterface : public UInterface
{
	//has to be empty
	GENERATED_BODY()
};

//Both C++ and BP interface, thus may have some redundancy
class PROTOGAME_API IInteractionInterface
{
	GENERATED_BODY()

public:
	//Actions array can be empty
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool OnInteract(AActor* caller, EInteractionActions action);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool IsInteractible() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	EInteractionType GetInteractionType() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void DrawInteractionOutline();

	//BP objects aren't obliged to implement this function 
	//They can use Retrigerrable Delay within "DrawInteractionOutline" to stop drawing outline for example
	//C++ needs it since here Retrigerable Delay can't just continue execution and has to bind to some functon
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void StopDrawingOutline();

	//Note that UI will show array elements in the order, so place actions accordingly
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	TArray<EInteractionActions> GetInteractionActions();
	//UPARAM(meta = (Bitmask, BitmaskEnum = EInteractionActions)) int32 GetInteractionActions();
};

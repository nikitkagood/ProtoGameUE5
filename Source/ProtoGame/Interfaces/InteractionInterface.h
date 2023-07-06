// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InteractionInterface.generated.h"

//Right now this is used purely visually - for outlines with custom depth stencil
//Do not change int values
UENUM(BlueprintType)
enum class InteractionType : uint8
{
	None = 0					UMETA(DisplayName = "None"),
	Item = 1					UMETA(DisplayName = "Item"),
	WorldInteractible = 2		UMETA(DisplayName = "World Interactible"),
	Quest = 3					UMETA(DisplayName = "Quest"),
};

UINTERFACE(Blueprintable, MinimalAPI)
class UInteractionInterface : public UInterface
{
	//has to be empty
	GENERATED_BODY()
};

class PROTOGAME_API IInteractionInterface
{
	GENERATED_BODY()

public:
	//Calls only C++ Interact
	virtual bool OnInteract(AActor* caller) = 0; //to be overriden in C++ classes that implement this interface
	
	//Calls only BP Interact
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Interaction") //to be implemented in BP
	bool OnInteractBP(AActor* caller);

	//Combines OnInteract C++ and OnInteractBP for code reusability reasons
	//Doesn't check for nullptr
	static void InteractCombined(AActor* iteracted_actor, AActor* caller);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool IsInteractible() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	InteractionType GetInteractionType() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void DrawInteractionOutline();

	//CPP only, BP objects must use DrawInteractionOutline. With retrigerrable delay to stop drawing outline for example.
	UFUNCTION(Category = "Interaction")
	virtual void StopDrawingOutline() { checkf(false, TEXT("IInteractionInterface::StopDrawingOutline is not meant to be called")); };
};

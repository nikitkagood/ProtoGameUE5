// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CustomHUD.generated.h"

UCLASS(Blueprintable)
class PROTOGAME_API ACustomHUD : public AHUD
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HUD_Custom)
	FVector2D GetMousePositionHUD(bool& is_valid);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HUD_Custom)
	void GetActorsInSelectionRectangleCustom(TSubclassOf<class AActor> ClassFilter, const FVector2D& FirstPoint, const FVector2D& SecondPoint, TArray<AActor*>& OutActors, bool bIncludeNonCollidingComponents, bool bActorMustBeFullyEnclosed, bool DebugDraw = false);
};

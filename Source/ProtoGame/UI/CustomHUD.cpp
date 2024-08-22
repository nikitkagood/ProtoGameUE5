// Nikita Belov, All rights reserved


#include "UI/CustomHUD.h"

#include "Engine/Canvas.h"
#include "EngineUtils.h"

FVector2D ACustomHUD::GetMousePositionHUD(bool& is_valid)
{
	ULocalPlayer* LocalPlayer = GetOwningPlayerController() ? Cast<ULocalPlayer>(GetOwningPlayerController()->Player) : nullptr;

	FVector2D MousePosTemp;

	if (LocalPlayer == nullptr)
	{
		is_valid = false;
		return {};
	}
	else
	{
		is_valid = true;
	}

	LocalPlayer->ViewportClient->GetMousePosition(MousePosTemp);

	return MousePosTemp;
}

void ACustomHUD::GetActorsInSelectionRectangleCustom(TSubclassOf<class AActor> ClassFilter, const FVector2D& FirstPoint, const FVector2D& SecondPoint, TArray<AActor*>& OutActors, bool bIncludeNonCollidingComponents, bool bActorMustBeFullyEnclosed, bool DebugDraw)
{
	// Because this is a HUD function it is likely to get called each tick,
	// so make sure any previous contents of the out actor array have been cleared!
	OutActors.Reset();

	if (DebugDraw)
	{
		DrawRect(FLinearColor::Blue, FirstPoint.X, FirstPoint.Y, (FirstPoint.X - SecondPoint.X) * -1, (FirstPoint.Y - SecondPoint.Y) * -1);
	}

	//Create Selection Rectangle from Points
	FBox2D SelectionRectangle(ForceInit);

	//This method ensures that an appropriate rectangle is generated, 
	//no matter what the coordinates of first and second point actually are.
	SelectionRectangle += FirstPoint;
	SelectionRectangle += SecondPoint;


	//The Actor Bounds Point Mapping
	const FVector BoundsPointMapping[8] =
	{
		FVector(1.f, 1.f, 1.f),
		FVector(1.f, 1.f, -1.f),
		FVector(1.f, -1.f, 1.f),
		FVector(1.f, -1.f, -1.f),
		FVector(-1.f, 1.f, 1.f),
		FVector(-1.f, 1.f, -1.f),
		FVector(-1.f, -1.f, 1.f),
		FVector(-1.f, -1.f, -1.f) };

	//~~~

	//For Each Actor of the Class Filter Type
	for (TActorIterator<AActor> Itr(GetWorld(), ClassFilter); Itr; ++Itr)
	{
		AActor* EachActor = *Itr;

		//Get Actor Bounds				//casting to base class, checked by template in the .h
		const FBox EachActorBounds = EachActor->GetComponentsBoundingBox(bIncludeNonCollidingComponents); /* All Components? */

		//Center
		const FVector BoxCenter = EachActorBounds.GetCenter();

		//Extents
		const FVector BoxExtents = EachActorBounds.GetExtent();

		// Build 2D bounding box of actor in screen space
		FBox2D ActorBox2D(ForceInit);
		for (uint8 BoundsPointItr = 0; BoundsPointItr < 8; BoundsPointItr++)
		{
			// Project vert into screen space.
			const FVector ProjectedWorldLocation = Project(BoxCenter + (BoundsPointMapping[BoundsPointItr] * BoxExtents), true);
			// Add to 2D bounding box if point is on the front side of the camera
			if (ProjectedWorldLocation.Z > 0.f)
			{
				ActorBox2D += FVector2D(ProjectedWorldLocation.X, ProjectedWorldLocation.Y);
			}
		}
		// Only consider actor boxes that have valid points inside
		if (ActorBox2D.bIsValid)
		{
			//Selection Box must fully enclose the Projected Actor Bounds
			if (bActorMustBeFullyEnclosed && SelectionRectangle.IsInside(ActorBox2D))
			{
				OutActors.Add(EachActor);
			}
			//Partial Intersection with Projected Actor Bounds
			else if (SelectionRectangle.Intersect(ActorBox2D))
			{
				OutActors.Add(EachActor);
			}
		}
	}
}
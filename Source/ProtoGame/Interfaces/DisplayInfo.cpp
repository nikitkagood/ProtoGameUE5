// Nikita Belov, All rights reserved


#include "Interfaces/DisplayInfo.h"

//TMap<FText, UClass*> IGetDisplayInfo::GetDisplayInfo_Implementation() const
//{
//    TMap<FString, UClass*> example_map;
//
//    FString example_str{ "ExampleStr1" };
//    UClass* example_str_class = nullptr;
//    example_map.Add(example_str, example_str_class);
//
//    FText example_str2 = FText::FromString("ExampleStr2");
//    FImage* example_image{};
//    UClass* example_str_class = example_image;
//
//    FCurve ads;
//
//    return example_map;
//}

//void IDisplayInfo::GetDisplayInfo_Implementation(TArray<FText>& OutTextArr, TArray<UObject*>& OutObjectArr)
//{
//    //Example of implementation
//
//    OutTextArr.Add(FText::FromString("ExampleTextFromString"));
//    OutTextArr.Add(FText::FromString("ExampleTexty 2"));
//    OutTextArr.Add(FText::FromString("ExampleTextFS 3"));
//
//    UCurveFloat* ExampleFloatCurve = NewObject<UCurveFloat>();
//
//    ExampleFloatCurve->FloatCurve.AddKey(0, 2);
//    ExampleFloatCurve->FloatCurve.AddKey(1, 4);
//    ExampleFloatCurve->FloatCurve.AddKey(2, 8);
//    ExampleFloatCurve->FloatCurve.AddKey(10, 10);
//    OutObjectArr.Add(MoveTemp(ExampleFloatCurve));
//
//    auto ExampleTexture = LoadObject<UTexture2D>(GetTransientPackage(), TEXT("Game/Pictures/Crab_gun_tyranny.Crab_gun_tyranny"));
//    OutObjectArr.Add(ExampleTexture);
//
//
//    //UTexture2D* ExampleTexture; 
//    //if (ExampleTextureSoft.Object.Get()->IsValid())
//    //{
//    //    ExampleTexture = ExampleTextureSoft.Object.Get()->Get();
//    //    ObjectArr.Add(ExampleTexture);
//    //}
//    //else
//    //{
//    //    ExampleTexture = ExampleTextureSoft.Object.Get()->LoadSynchronous();
//    //    if (ensure(ExampleTextureSoft.Object.Get()->IsValid()))
//    //    {
//    //        ObjectArr.Add(ExampleTexture);
//    //    }
//    //}
//
//}

TArray<FDisplayInfoPayload> IDisplayInfo::GetDisplayInfo_Implementation()
{
    //Example of implementation 

    TArray<FDisplayInfoPayload> payload;

    payload.Add({ nullptr, FText::FromString("Example Text From String"), {0, 0} });
    payload.Add({ nullptr, FText::FromString("Example Text Row 1 Column 0"), {1, 0} });
    payload.Add({ nullptr, FText::FromString("Example Text Column idx1"), {0, 1} });
    payload.Add({ nullptr, FText::FromString("Example Text From Stringyy col idx2"), {0, 2} });

    UCurveFloat* ExampleFloatCurve = NewObject<UCurveFloat>();
    ExampleFloatCurve->FloatCurve.AddKey(0, 2);
    ExampleFloatCurve->FloatCurve.AddKey(1, 4);
    ExampleFloatCurve->FloatCurve.AddKey(2, 8);
    ExampleFloatCurve->FloatCurve.AddKey(10, 10);

    payload.Add({ MoveTemp(ExampleFloatCurve), {}, {3, 0} }); //text is empty on purpose

    auto ExampleTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Pictures/Crab_gun_tyranny.Crab_gun_tyranny"));
    if (IsValid(ExampleTexture))
    {
        payload.Add({ MoveTemp(ExampleTexture), FText::FromString("Crab Pic"), {2, 0} });
    }

    return payload;
}

TArray<FName> IDisplayInfo::GetSupportedClassNames_Implementation()
{
    //Example and default implementation

    return { UTexture2D::StaticClass()->GetFName(), UCurveFloat::StaticClass()->GetFName()};
}


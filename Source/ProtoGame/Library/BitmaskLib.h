// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"

//Natural example:
// i.e. there is no need to explicitly assign numbers, UE only
//UENUM(BlueprintType, Meta = (Bitflags))
//enum class EGreekLetters : uint8
//{
//	Alpha /* = 0 */,
//	Beta  /* = 1 */,
//	Gamma /* = 2 */,
//	Delta /* = 3 */
//};

//Power of two example:
//UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
//enum class EMovementInputToggleFlags : uint8
//{
//	None = 0			UMETA(DisplayName = "None", Hidden),
//	SlowWalk = 1		UMETA(DisplayName = "SlowWalk"),
//	Sprint = 2			UMETA(DisplayName = "Sprint"),
//	Prone = 4			UMETA(DisplayName = "Prone"),
//	Crouch = 8			UMETA(DisplayName = "Crouch"),
//};
//ENUM_CLASS_FLAGS(EMovementInputToggleFlags);


enum class EBitmaskType : uint8
{
	None = 0,
	Natural = 1,
	PowerOfTwo = 2
};

class PROTOGAME_API BitmaskLib
{
public:
	BitmaskLib() = delete;
	~BitmaskLib() = delete;

	template<typename BitmaskEnum>
	static bool IsBitSetNatural(uint8 mask, BitmaskEnum what_bit);

	template<typename BitmaskEnum>
	static bool IsBitSetPowerOfTwo(uint8 mask, BitmaskEnum what_bit);

	template<typename BitmaskEnum>
	static void SetBitNatural(uint8& mask, BitmaskEnum what_bit, bool value);
	
	template<typename BitmaskEnum>
	static void SetBitPowerOfTwo(uint8& mask, BitmaskEnum what_bit, bool value);
};

//int-independent code: static_cast<decltype(mask)>(what_bit)

template<typename BitmaskEnum>
inline bool BitmaskLib::IsBitSetNatural(uint8 mask, BitmaskEnum what_bit)
{
	return mask & (1 << static_cast<int32>(what_bit));
}

template<typename BitmaskEnum>
inline bool BitmaskLib::IsBitSetPowerOfTwo(uint8 mask, BitmaskEnum what_bit)
{
	return mask & static_cast<uint8>(what_bit);
}

template<typename BitmaskEnum>
inline void BitmaskLib::SetBitNatural(uint8& mask, BitmaskEnum what_bit, bool value)
{
	//TODO: not implemented, see if PowerOfTwo implementation would work
	check(false);
}

template<typename BitmaskEnum>
inline void BitmaskLib::SetBitPowerOfTwo(uint8& mask, BitmaskEnum what_bit, bool value)
{
	//1UL - 1 unsigned long, it's for int width independency
	mask ^= (-static_cast<long>(value) ^ mask) & (1UL < static_cast<uint8>(what_bit));
}

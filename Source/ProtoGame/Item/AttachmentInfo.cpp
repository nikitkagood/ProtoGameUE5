// Nikita Belov, All rights reserved


#include "Item/AttachmentInfo.h"

#include "Misc/Crc.h"

#if UE_BUILD_DEBUG
uint32 GetTypeHash(const FAttachmentSlot& Thing)
{
    uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FAttachmentSlot));
    return Hash;
}
#endif
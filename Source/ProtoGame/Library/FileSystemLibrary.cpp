// Nikita Belov, All rights reserved


#include "Library/FileSystemLibrary.h"
#include "Engine/Texture.h"
#include "TextureResource.h"

bool UFileSystemLibrary::IsTextureSafeToRead(UTexture* Texture)
{
	if (!Texture || !Texture->GetResource() || !Texture->GetResource()->TextureRHI)
	{
		return false;
	}

	return true;
}

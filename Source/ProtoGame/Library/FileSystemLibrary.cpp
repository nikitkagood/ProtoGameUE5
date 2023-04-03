// Nikita Belov, All rights reserved


#include "Library/FileSystemLibrary.h"

bool UFileSystemLibrary::IsTextureSafeToRead(UTexture* Texture)
{
	if (!Texture || !Texture->GetResource() || !Texture->GetResource()->TextureRHI)
	{
		return false;
	}

	return true;
}

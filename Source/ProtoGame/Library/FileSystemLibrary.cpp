// Nikita Belov, All rights reserved


#include "Library/FileSystemLibrary.h"

bool UFileSystemLibrary::IsTextureSafeToRead(UTexture* Texture)
{
	if (!Texture || !Texture->Resource || !Texture->Resource->TextureRHI)
	{
		return false;
	}

	return true;
}

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <string>

#include <GL/glew.h>

#include "JShader.h"
#include "JTypes.h"


class JResourceManager
{
public:
	static JTexture* LoadTextureFromFile(const char* filename);

	// Loads (and generates) a shader program from file loading vertex, fragment shader's source code.
	static JShader LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
	
	// Retrieves a stored sader
	static JShader GetShader(std::string name);

	// Properly de-allocates all loaded resources
	static void Clear();

private:
	struct TextureInfo
	{
		u8 *mBits;
		int mWidth;
		int mHeight;
		int mTexWidth;
		int mTexHeight;
		bool mVRAM;
	};

	// Resource storage
	static std::map<std::string, JShader>    Shaders;

	// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
	JResourceManager() { }

	static bool LoadPNG(TextureInfo &textureInfo, const char *filename);

	// Loads and generates a shader from file
	static JShader LoadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
};

#endif
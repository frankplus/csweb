#include "../include/JResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <png.h>
#include "../include/JFileSystem.h"

std::map<std::string, JShader> JResourceManager::Shaders;

JShader JResourceManager::LoadShader(const GLchar * vShaderFile, const GLchar * fShaderFile, const GLchar * gShaderFile, std::string name)
{
	Shaders[name] = LoadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

JShader JResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

void JResourceManager::Clear()
{
	// (Properly) delete all shaders	
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.Program);
}

JShader JResourceManager::LoadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	try
	{
		// Open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();
	// 2. Now create shader object from source code
	JShader shader;
	shader.Compile(vShaderCode, fShaderCode);
	return shader;
}

JTexture* JResourceManager::LoadTextureFromFile(const char* filename)
{
	TextureInfo textureInfo;
	
	textureInfo.mBits = NULL;
	
	if( !LoadPNG(textureInfo, filename) )
		printf("Failed to load png %s \n", filename);

	if (textureInfo.mBits == NULL)
	{
		printf("Failed to load texture %s \n", filename);
		return NULL;
	}

	bool ret = false;

	JTexture *tex = new JTexture();

 	if (tex)
	{
		tex->mFilter = TEX_FILTER_LINEAR;
		tex->mWidth = textureInfo.mWidth;
		tex->mHeight = textureInfo.mHeight;
		tex->mTexWidth = textureInfo.mTexWidth;
		tex->mTexHeight = textureInfo.mTexHeight;

		GLuint texid; 
		glGenTextures(1, &texid);
		tex->mTexId = texid;

		if (texid != 0)
		{
			// OpenGL texture has (0,0) at lower-left
			// Pay attention when doing texture mapping!!!

			glBindTexture(GL_TEXTURE_2D, texid);								// Bind To The Texture ID

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureInfo.mTexWidth, textureInfo.mTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureInfo.mBits);

			ret = true;
		}
	}

	delete [] textureInfo.mBits;

	if (!ret)
	{
		if (tex)
			delete tex;
		tex = NULL;
		printf("Failed to load texture %s \n", filename);
	}

	return tex;
}

static int getNextPower2(int width)
{
	int b = width;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * width) b >>= 1;
	return b;
}

static void PNGCustomWarningFn(png_structp png_ptr, png_const_charp warning_msg)
{
        // ignore PNG warnings
}


static void PNGCustomReadDataFn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_voidp io_ptr = png_get_io_ptr(png_ptr);

	JFileSystem &fileSystem = *(JFileSystem *)io_ptr;
	const size_t bytesRead = fileSystem.ReadFile(
		(unsigned char *)data,
		(size_t)length);

	if ((png_size_t)bytesRead != length)
		png_error(png_ptr, "Read Error!");
}

bool JResourceManager::LoadPNG(TextureInfo &textureInfo, const char *filename)
{
	textureInfo.mBits = NULL;

	DWORD* p32;

    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    png_uint_32 width, height, tw, th;
    int bit_depth, color_type, interlace_type, x, y;
    DWORD* line;

	JFileSystem* fileSystem = JFileSystem::GetInstance();
	if (!fileSystem->OpenFile(filename)) return false;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) 
	{
        //fclose(fp);
		fileSystem->CloseFile();

        return false;
    }

    png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, PNGCustomWarningFn);
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) 
	{
        //fclose(fp);
		fileSystem->CloseFile();

        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);

        return false;
    }
    png_init_io(png_ptr, NULL);
	png_set_read_fn(png_ptr, (png_voidp)fileSystem, PNGCustomReadDataFn);

    png_set_sig_bytes(png_ptr, sig_read);
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, (int*)NULL, (int*)NULL);
    png_set_strip_16(png_ptr);
    png_set_packing(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
    png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
    
	line = (DWORD*) malloc(width * 4);
    if (!line) 
	{
        //fclose(fp);
		fileSystem->CloseFile();
		
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return false;
    }
   

	tw = getNextPower2(width);
	th = getNextPower2(height);
		
	int size = tw * th * 4;			// RGBA

	u8* buffer = new u8[size];

	//JTexture *tex = new JTexture();

	if (buffer)
	{
		// OpenGL texture has (0,0) at lower-left
		// Pay attention when doing texture mapping!!!

		p32 = (DWORD*) buffer;// + (height-1)*width;	

		for (y = 0; y < (int)height; y++) 
		{
			png_read_row(png_ptr, (u8*) line, (png_bytep)NULL);
			for (x = 0; x < (int)width; x++) 
			{
				DWORD color32 = line[x];
				//u16 color16;
				int a = (color32 >> 24) & 0xff;
				int r = color32 & 0xff;
				int g = (color32 >> 8) & 0xff;
				int b = (color32 >> 16) & 0xff;

				color32 = r | (g << 8) | (b << 16) | (a << 24);
				*(p32+x) = color32;
			
			}
			p32 += tw;
			
		}
	}

    free (line);

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	
    //fclose(fp);
	fileSystem->CloseFile();

	textureInfo.mBits = buffer;
	textureInfo.mWidth = width;
	textureInfo.mHeight = height;
	textureInfo.mTexWidth = tw;
	textureInfo.mTexHeight = th;

	printf("loaded png %s w=%d h=%d \n", filename, tw, th);

	return true;
}
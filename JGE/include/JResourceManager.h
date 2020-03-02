//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include <stdio.h>
#include <vector>
#include <map>
#include <string>

using namespace std;


#define INVALID_ID				-1

class JRenderer;
class JMotionEmitter;
class JSample;
class JMusic;
class JTexture;
class JQuad;

class JResourceManager
{
public:
	JResourceManager();
	~JResourceManager();

	//void SetResourceRoot(const string& resourceRoot);
	bool LoadResource(const string& resourceName);

	void RemoveAll();
	void RemoveGraphics();
	void RemoveSound();
	void RemoveFont();
	
	int CreateTexture(const string &textureName);
	JTexture* GetTexture(const string &textureName);
	JTexture* GetTexture(int id);

	int CreateQuad(const string &quadName, const string &textureName, float x, float y, float width, float height);
	JQuad* GetQuad(const string &quadName);
	JQuad* GetQuad(int id);

	int LoadFont(const string &fontName);
	JFont* GetFont(const string &fontName);
	JFont* GetFont(int id);

	int LoadMusic(const string &musicName);
	JMusic* GetMusic(const string &musicName);
	JMusic* GetMusic(int id);

	int LoadSample(const string &sampleName);
	JSample* GetSample(const string &sampleName);
	JSample* GetSample(int id);

private:

	//JRenderer *mRenderer;
	
	//string mResourceRoot;

	vector<JTexture *> mTextureList;
	map<string, int> mTextureMap;

	vector<JQuad *> mQuadList;
	map<string, int> mQuadMap;

	vector<JFont *> mFontList;
	map<string, int> mFontMap;

	vector<JMusic *> mMusicList;
	map<string, int> mMusicMap;

	vector<JSample *> mSampleList;
	map<string, int> mSampleMap;
};

#endif

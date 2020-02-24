//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#pragma warning(disable : 4786)

#include "../include/JGE.h"
#include "../include/JFileSystem.h"
#include "tinyxml/tinyxml.h"

#include <stdio.h>
#include <vector>
#include <map>
#include <string>

JFileSystem* JFileSystem::mInstance = NULL;

JFileSystem* JFileSystem::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new JFileSystem();
	}

	return mInstance;
}


void JFileSystem::Destroy()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = NULL;
	}
}


JFileSystem::JFileSystem()
{
	mFile = NULL;
	mPassword = NULL;
	mFileSize = 0;

	mResourceRoot = "";//"Res/";				// default root folder 
}


JFileSystem::~JFileSystem()
{

}


bool JFileSystem::OpenFile(const string &filename)
{

	string path = mResourceRoot + filename;

	mFile = fopen(path.c_str(), "rb");
	if (mFile != NULL)
	{
		fseek(mFile, 0, SEEK_END);
		mFileSize = ftell(mFile);
		fseek(mFile, 0, SEEK_SET);
		return true;
	}	
	
	return false;
			
}


void JFileSystem::CloseFile()
{
	if (mFile != NULL)
		fclose(mFile);
}


int JFileSystem::ReadFile(void *buffer, int size)
{
	return fread(buffer, 1, size, mFile);
}


int JFileSystem::GetFileSize()
{
	return mFileSize;
}


void JFileSystem::SetResourceRoot(const string& resourceRoot)
{
	mResourceRoot = resourceRoot;
}

//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------


#ifndef _JGUI_H
#define _JGUI_H

#include "JGE.h"

#define MAX_GUIOBJECT			1024

#define JGUI_STYLE_LEFTRIGHT	0x01
#define JGUI_STYLE_UPDOWN		0x02

#define JGUI_INITIAL_DELAY		800
#define JGUI_REPEAT_DELAY		100

class JGuiListener
{
public:
	virtual ~JGuiListener() {}
	virtual void ButtonPressed(int controllerId, int controlId) = 0;
};


class JGuiObject
{
protected:
	static JGE*	mEngine;
	bool mHasFocus;

private:
	int mId;
	

public:
	JGuiObject(int id, bool hasFocus);
	virtual ~JGuiObject();

	virtual void Render(float x, float y) = 0;
	virtual void Update(float dt);

	virtual void Entering();			// when focus is transferring to this obj
	virtual bool Leaving(u32 key);		// when focus is transferring away from this obj, true to go ahead

	int GetId();
	bool HasFocus();
};


class JGuiController
{
private:
	static JGE*	mEngine;

	int mId;
	bool mActive;
	
	JGuiObject* mObjects[MAX_GUIOBJECT];
	int mCount;
	u32	mActionButton;
	int mCurr;
	int mDefault;
	bool mWrapping;
	int mDirection;
	
	int mBgX;
	int mBgY;
	const JTexture* mBg;
	PIXEL_TYPE mShadingColor;
	Rect* mShadingBg;
	
	JGuiListener* mListener;

	bool KeyRepeated(u32 key, float dt);

public:
	u32 mLastKey;
	float mKeyRepeatDelay;

	JGuiController(int id, JGuiListener* listener, int direction = JGUI_STYLE_UPDOWN);
	~JGuiController();

	void Render(float x = 0, float y = 0);
	void Update(float dt);

	void Add(JGuiObject* ctrl);
	void Remove(int id);

	void SetActionButton(u32 button);

	// If wrapping is enabled then last object+1 = first object and first object-1 = last object
	void SetWrapping(bool flag); 
	
	// Get controller enabled flag
	bool IsActive(); 

	// Set controller enabled flag
	void SetActive(bool flag); 

	// SetCurr to default object
	void Reset(); 

	// Get current object with focus
	int GetCurr(); 

	// Enter object and set object with focus 
	void SetCurr(int curr); 

	// Get number of Objects
	int GetCount();

	// Get object by index
	JGuiObject* GetGuiObject(int index);
};


#endif

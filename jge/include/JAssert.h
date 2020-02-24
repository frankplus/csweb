#ifndef _JASSERT_H_
#define _JASSERT_H_

#include "JGE.h"

	#ifdef NDEBUG
		
		#define JASSERT(p)			((void)0)
		
	#else
		#define JASSERT(e)		if (!(e)) { JGE::GetInstance()->Assert(__FILE__, __LINE__); }
	#endif

#endif

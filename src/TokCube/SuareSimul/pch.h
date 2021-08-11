// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#include "KwLib64/ktypedef.h"
#ifdef _Use_OpenSsl
#pragma message( "pch #define _Use_OpenSsl")
#endif

#include "KwLib64/Kw_tool.h"
#include "KwLib64/DlgTool.h"
#include "KwLib64/ThreadPool.h"
#include "KwLib64/Lock.h"
#include "KwLib64/KJson.h"
using namespace Kw;

#endif //PCH_H

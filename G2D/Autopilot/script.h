/*
	Author: Anh-Dzung Doan
*/
#pragma once
#include "..\dependencies\inc\natives.h"
#include "..\dependencies\inc\types.h"
#include "..\dependencies\inc\enums.h"
#include "..\dependencies\inc\main.h"
#include <string>
#include <fstream>
#include <vector>
#include <stdio.h>
#pragma comment(lib, "gdiplus.lib")
#include <atlimage.h>

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#define HRCHECK(__expr) {hr=(__expr);if(FAILED(hr)){wprintf(L"FAILURE 0x%08X (%i)\n\tline: %u file: '%s'\n\texpr: '" WIDEN(#__expr) L"'\n",hr, hr, __LINE__,__WFILE__);goto cleanup;}}
#define RELEASE(__p) {if(__p!=nullptr){__p->Release();__p=nullptr;}}

#define PI 3.14159265359
#define DIRECTX 1
#define GDI 2

/*********************************************************************/
/******************************** FUNCTIONS **************************/
/*********************************************************************/

/******************************** Functions for drawing menu ********************************/
void drawMenuLine(std::string caption, float lineWidth, float lineHeight, float lineTop,
	float lineLeft, float textLeft, bool active, bool title, bool rescaleText = true);
void drawRect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7);

/******************************** Functions for listening keyboard ********************************/
bool switchPressed(); // listen if users open main menu
void getButtonState(bool *select, bool *back, bool *up, bool *down); // listen keyboard buttons

/******************************** Functions that process menu ********************************/
void handleMainMenu(); // display main menu

/******************************** Other functions ****************************************************/
void ScriptMain(); // Main functions
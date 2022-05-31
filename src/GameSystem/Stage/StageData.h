#pragma once

enum E_STAGE_TYPE_ID
{
	eNONE = -1,		//ブロックなし
	eNORMAL = 0,	//通常ブロック
	eFIRE,
	eWATER,
	
	eSTAGE_MAX,

	eSTART,
	eGOAL,
	eTYPE_MAX
};
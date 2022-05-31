#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GimmickGoal.h
	@brief		ゴール
	@author		shiba
	@date		2015/07/15 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "GimmickBase.h"


class CGimmickGoal : public CGimmickBase
{
public:
	CGimmickGoal();
	~CGimmickGoal();

	virtual bool Init();
	virtual void Update(const f32 fDelta);
	virtual void Destroy();

private:
	void Term();
};

//EOF

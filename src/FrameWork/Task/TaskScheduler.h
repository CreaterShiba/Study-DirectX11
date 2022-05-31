#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		TaskManager.h
	@brief		タスクマネージャー
	@author		shiba
	@date		2012/10/7 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include <list>
#include "FrameWork/Utility/UtilMacro.h"
#include "FrameWork/Task/Task.h"

class CTask;
class CTaskScheduler
{
	//シングルトン
	NS_SINGLETON(CTaskScheduler);
	CTaskScheduler();
	~CTaskScheduler();

public:
	//@brief 初期化
	bool Init();
	//@brief 更新
	void Update();

	//@brief ポーズ
	void OnPause(const CTask::E_TASK_LOCK_LV eLockLv);
	void OffPause();
	bool IsPause();

	//@brief タスクに追加する
	inline void AddTask(CTask* pTask);

	//@brief タスクから取り除く
	inline void DelTask(CTask* pTask);

private:
	std::list< CTask* > m_paTaskList;		//!< タスクリスト @todo 自前リスト使わないと
	CTask::E_TASK_LOCK_LV	m_ePauseLv;		//!< 

private:
	void Term();
};

void CTaskScheduler::AddTask(CTask* pTask)
{
	m_paTaskList.push_back(pTask);
}

void CTaskScheduler::DelTask(CTask* pTask)
{
	m_paTaskList.remove(pTask);
}

//EOF

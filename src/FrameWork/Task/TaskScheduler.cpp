/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		TaskScheduler.cpp
	@brief		タスクマネージャー
	@author		shiba
	@date		2012/10/7 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#include "TaskScheduler.h"
#include "FrameWork/Utility/UtilInput.h"

NS_SINGLETON_INSTANCE_DEFINE(CTaskScheduler);

CTaskScheduler::CTaskScheduler():
	m_ePauseLv(CTask::e_Task_Lock_01)
{
	m_paTaskList.clear();
}

CTaskScheduler::~CTaskScheduler()
{
	Term();
}

bool CTaskScheduler::Init()
{
	return true;
}

void CTaskScheduler::Update()
{
#if defined(_DEBUG)
	//デバッグ用ポーズ
	if (CUtilInput::IsTrigger(E_KEYBOARD_BUTTON::eKEYBOARD_P))
	{
		if (IsPause())
		{
			OffPause();
		}
		else
		{
			OnPause(CTask::e_Task_Lock_06);
		}
	}
#endif

	std::list< CTask* >::iterator it = m_paTaskList.begin();
	for (; it != m_paTaskList.end(); ++it)
	{
		if ((*it)->GetTaskEnable())
		{
			if ((*it)->GetLockLv() < m_ePauseLv)
			{
				//ポーズされているので、処理を飛ばす。
				continue;
			}

			(*it)->Update(0.0f);
		}
	}

	//解除命令が出ているものは、削除する。
	it = m_paTaskList.begin();
	for (; it != m_paTaskList.end();)
	{
		if ((*it)->IsDelTaskEnable())
		{
			//消す
			CTask* pTask = *it;
			SAFE_DELETE(pTask);	
			it = m_paTaskList.erase(it);
			continue;
		}
		it++;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	ポーズする
	@param[in]	eLockLv	ポーズレベル
*/
//---------------------------------------------------------------------------
void CTaskScheduler::OnPause(const CTask::E_TASK_LOCK_LV eLockLv)
{
	m_ePauseLv = eLockLv;
}

//---------------------------------------------------------------------------
/*!
	@brief	ポーズ解除
*/
//---------------------------------------------------------------------------
void CTaskScheduler::OffPause()
{
	m_ePauseLv = CTask::e_Task_Lock_01;
}

bool CTaskScheduler::IsPause()
{
	return m_ePauseLv > CTask::e_Task_Lock_01;
}

void CTaskScheduler::Term()
{
	std::list< CTask* >::iterator it = m_paTaskList.begin();
	for (; it != m_paTaskList.end();)
	{
		//消す
		SAFE_DELETE(*it);
		++it;
	}
	m_paTaskList.clear();
}

//EOF
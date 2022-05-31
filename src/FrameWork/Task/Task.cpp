/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Task.cpp
	@brief		タスク基底
	@author		shiba
	@date		2012/9/29 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "Task.h"
#include "TaskScheduler.h"

CTask::CTask()
	: m_eLockLevel(e_Task_Lock_01)
	, m_unTaskUIID(0)
	, m_bTaskEnable(true)
	, m_bDelTask()
{
	AddTask();
}

CTask::~CTask()
{

}

void CTask::AddTask()
{
	CTaskScheduler* pcTaskM = CTaskScheduler::GetInstance();
	if (pcTaskM)
	{
		pcTaskM->AddTask(this);
	}
}

void CTask::DelTask()
{
	m_bDelTask = true;
}

//EOF
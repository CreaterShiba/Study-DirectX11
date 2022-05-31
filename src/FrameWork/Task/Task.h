#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Task.h
	@brief		タスク基底
	@author		shiba
	@date		2012/9/9 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

//==========================================================================*/
/*!
	@class	CTask
	@brief	タスクインターフェイス
	@note	ゲーム内で動作を行うものは、すべてこのインターフェイスを継承する。
*/
//==========================================================================*/
class CTask
{
public:
	//タスクロックレベル
	//ポーズをかけたとき、どれくらいロックさせる？
	enum E_TASK_LOCK_LV
	{
		e_Task_Lock_01 = 0,
		e_Task_Lock_02 = 1,
		e_Task_Lock_03 = 1 << 1,
		e_Task_Lock_04 = 1 << 2,
		e_Task_Lock_05 = 1 << 3,
		e_Task_Lock_06 = 1 << 4,
		e_Task_Lock_07 = 1 << 5
	};

public:
	CTask();
	virtual ~CTask();

	virtual bool Init() = 0;
	virtual void Update(const f32 fDelta) = 0;
	virtual void Destroy() = 0;	//仮想関数はコンストラクタやデストラクタから呼び出さないよう注意

	inline void SetTaskEnable(const bool bEnable);
	inline bool GetTaskEnable() const;

	inline void SetLockLv(const E_TASK_LOCK_LV eLockLv);
	inline E_TASK_LOCK_LV  GetLockLv() const;

	bool IsDelTaskEnable()
	{
		return m_bDelTask;
	}

	void AddTask();	//@brief 自分をタスクに追加する
	void DelTask();	//@brief 自分をタスクから削除する

protected:

	E_TASK_LOCK_LV		m_eLockLevel;	//!< タスクのロックレベル

	u32					m_unTaskUIID;	//!< 一意のタスクID
	bool				m_bTaskEnable;	//!< 更新する？
	bool				m_bDelTask;		//!< 削除命令
};

void CTask::SetTaskEnable(const bool bEnable)
{
	m_bTaskEnable = bEnable;
}

bool CTask::GetTaskEnable() const
{
	return m_bTaskEnable;
}

void CTask::SetLockLv(const E_TASK_LOCK_LV eLockLv)
{
	m_eLockLevel = eLockLv;
}

CTask::E_TASK_LOCK_LV CTask::GetLockLv() const
{
	return m_eLockLevel;
}

//EOF
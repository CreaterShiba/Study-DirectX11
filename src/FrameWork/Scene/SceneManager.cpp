#include "SceneManager.h"

#include "GameSystem/Scene/SceneTitle.h"
#include "GameSystem/Scene/SceneMain.h"
#include "GameSystem/Scene/SceneDebug.h"

NS_SINGLETON_INSTANCE_DEFINE(CSceneManager);

CSceneManager::CSceneManager()
#if defined(_DEBUG)
	: m_eIgnitionScene(CSceneManager::eMAIN)
#else
	: m_eIgnitionScene(CSceneManager::eMAIN)
#endif
, m_pActiveScene( nullptr )
{
}

CSceneManager::~CSceneManager()
{
	Term();
}

bool CSceneManager::Init()
{
	LoadScene(m_eIgnitionScene);

	return true;
}

void CSceneManager::LoadScene( const E_SCENE eScene )
{
	//シーンの切り替え
	SAFE_DELETE_TASK(m_pActiveScene);

	switch (eScene)
	{
	case E_SCENE::eTITLE:
		CLASS_NEW(m_pActiveScene, CSceneTitle);
		break;
	case E_SCENE::eMAIN:
		CLASS_NEW(m_pActiveScene, CSceneMain);
		break;
#if defined(_DEBUG)
	case E_SCENE::eDEBUG:
		CLASS_NEW(m_pActiveScene, CSceneDebug);
		break;
#endif
	default:
		break;
	}
}

bool CSceneManager::IsSceneLoadEnd()
{
	bool bEndLoad = false;

	return bEndLoad;
}

void CSceneManager::Term()
{
	SAFE_DELETE_TASK(m_pActiveScene);
}

//EOF

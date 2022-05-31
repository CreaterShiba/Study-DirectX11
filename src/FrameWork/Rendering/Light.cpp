#include "Light.h"

using namespace DirectX;

CLight::CLight() :
	m_eLightType(E_LIGHT_TYPE::eDIRECTIONAL)
#if defined(_DEBUG)
	, m_pcDebugGeom(nullptr)
#endif
	, m_fIntensity(0.0f)
{
	m_vDir.SetZero();
}

CLight::~CLight()
{
	Term();
}

bool CLight::Init()
{
//#if defined(_DEBUG)
//	CLASS_NEW(m_pcDebugGeom, CDebugGeometry);
//	m_pcDebugGeom->SetupGeom(CDebugGeometry::eCUBE);
//#endif

	m_cLightColor = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	return true;
}

void CLight::Update(const f32 fDelta)
{
	NS_UNUSED(fDelta);
#if defined(_DEBUG)
	if(m_pcDebugGeom)
		m_pcDebugGeom->SetPos(GetPos());
#endif
}

void CLight::Destroy()
{
	Term();
}

void CLight::Term()
{
#if defined(_DEBUG)
	CLASS_DESTROY(m_pcDebugGeom);
#endif
}

//EOF

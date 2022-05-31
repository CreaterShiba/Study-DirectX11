#ifndef __COMPUTEPARTICLE_H__
#define __COMPUTEPARTICLE_H__

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ComputeParticle.h
	@brief		コンピュートパーティクル
	@author		ta.Ishii
	@date		2012/12/2 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "FrameWork/Compute/ComputeBase.h"

/*==========================================================================*/
/*!
	@class	CComputeParticle
	@brief	コンピュートテストパーティクル
*/
/*==========================================================================*/
class CComputeParticle : public CComputeBase
{
public:
	CComputeParticle();
	~CComputeParticle();

	virtual bool Init();
	virtual void Calc( const f32 fDelta );
	virtual void Destroy();	

	struct Particle
	{
		NSVector4 pos;
		NSVector3 vel;
		f32	angle;
		f32 deltaangle;
		f32 size;
	};

protected:
	
private:
};

#endif // __COMPUTEPARTICLE_H__

//EOF

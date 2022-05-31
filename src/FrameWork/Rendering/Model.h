#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Model.cpp
	@brief		モデルクラス。(ユーザー使用)
	@author		shiba
	@date		2012/7/23 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "FrameWork/Rendering/Render.h"

class CAssetModel;
class CJoint;
class CModel;
class CMotion;

//==========================================================================*/
/*!
@class	CModel
@brief	モデルクラス
*/
//==========================================================================*/
class CModel : public CRender
{
	//ローダクラスはアクセスできる。
	friend class CLoaderModel;

#if defined( _DEBUG )
	friend class CDebugGeometry;
#endif

	//コピー禁止
	void operator =(const CModel& src) { NS_UNUSED(src); }
	CModel(const CModel& src);

public:
	// @brief コンストラクタ
	CModel();

	// @brief デストラクタ
	~CModel();

	//@brief ロード
	bool LoadModel(const TCHAR* pStrFileName, const TCHAR* pAnimSetCSVFileName = nullptr);

	//@brief 初期化
	virtual bool Init();

	//@brief 更新
	virtual void Update(const f32 fDelta);

	//@brief 破棄
	virtual void Destroy();

	//@brief 描画
	virtual void Draw();

	// @brief ジョイント取得
	CJoint* GetJointByIdx(const u32 unJointIdx);
	CJoint* GetJoint(const TCHAR* strJointName);

	// @brief ルートジョイント獲得
	CJoint* GetRootJoint() const;

	// @brief 全ジョイント数
	u32 GetJointNum() const;

	// @brief モデルを指定ボーンにアタッチする
	void AttachModelToJoint(CModel *pcAttachModel, const TCHAR* strBoneName);

	// @brief モーション
	void PlayMotionForward(const u32 unMotionID, const bool bLoop, const bool bEndLoop = false, const f32 fSpeed = 1.0f);
	void PlayMotionBack(const u32 unMotionID, const bool bLoop, const bool bEndLoop = false, const f32 fSpeed = 1.0f);
	void StopPlayingMotion();
	inline bool IsPlayingMotion() const;

	// @brief　直接テクスチャID指定
	void SetDiffuseTexID(const s32 nMeshID, const u32 unTexUIID);
	void SetNormalTexID(const s32 nMeshID, const u32 unTexUIID);

	// @brief　合成行列取得
	inline NSMatrix* GetCombMatrix() const;

	// @brife メッシュ単位の描画On/Off切り替え。
	void SetFlagDrawMesh(const u32 nMeshID, const bool bDrawFlag);

	// ----- 以下インスタンシング用の処理
	// @brief インスタンシング用行列の生成
	void CreateInstancingMatrix( s32 nNum );

	// @brief インスタンシング用行列の設定
	void SetInstancingMatrix(const s32 nIndex, NSMatrix& cMat);

	// @brief　インスタシング行列取得
	inline NSMatrix* GetInstancingMatrix() const;

	// @brief　インスタンシングあり？
	inline bool IsInstancingModel() const;

	// @brief インスタンシング用行列のサイズ
	inline s32 GetInstancingNum() const;

	// @brief モデルデータ獲得
	inline CAssetModel* GetModelData() const;

#if defined(_DEBUG)
	void OutputVertexData();
#endif

private:
	//brief 破棄処理
	void Term();

	// @brief モデルデータ
	void SetModelData(CAssetModel* pcModelData);

	// @brief モーション取得
	CMotion* GetMotion(const TCHAR* strMotionName);

private:
	//!< 描画対象のモデルデータアセット
	class CAssetModel* m_pcModelData;

	//!< スキンメッシュ用の合成マトリクス(メッシュ毎にいる)
	NSMatrix* m_paCombMatrix;

	//!< インスタンシング用のワールド変換マトリクス
	NSMatrix* m_paInstancingMatrix;

	//!< インスタンシング用のワールド変換マトリクスサイズ
	s32 m_nInstancingMatrixNum;

	//!< 再生中モーションID
	u32 m_unPlayingMotionID;

	//!< メッシュ毎の描画のOn/off切り替えフラグ
	bool* m_baMeshDrawFlag;
};

NSMatrix* CModel::GetCombMatrix() const
{
	return m_paCombMatrix;
}

NSMatrix* CModel::GetInstancingMatrix() const
{
	return m_paInstancingMatrix;
}

bool CModel::IsInstancingModel() const
{
	return m_paInstancingMatrix != nullptr;
}

s32 CModel::GetInstancingNum() const
{
	return m_nInstancingMatrixNum;
}

bool CModel::IsPlayingMotion() const
{
	return m_unPlayingMotionID != INVALID_U32;
}

CAssetModel* CModel::GetModelData() const
{
	return m_pcModelData;
}

//EOF

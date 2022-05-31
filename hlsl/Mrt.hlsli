
//行列系バッファ
cbuffer cbMatrixs : register(b0)
{
	matrix world;		//ワールドマトリクス
	matrix SkinMat[32];	//スキン用マトリクス
	matrix view;		//ビュー
	matrix proj;		//プロジェクション
	matrix Shadow;		//テクスチャ変換行列
};

//パラメータ
cbuffer cbParam : register(b1)
{
	int nSkinTrue;		//0: 1: 2: 3:
	float3 LightPos;	//ライトワールド座標
}

//インスタンシング行列
cbuffer cbInstancingParam : register(b2)
{
	matrix insWorld[128];		//インスタンシング用ローカルマトリクス
}


//各種変数
Texture2D AlbedoTex : register(t0);	//アルベド
Texture2D NormalTex : register(t1);		//ノーマルマップ
Texture2D ShadowMap : register(t2);		//シャドウ用 Zテクスチャ

SamplerState MySamp : register(s0);					//サンプラ
SamplerComparisonState ShadowSmp : register(s1);	//シャドウ判定用の比較サンプラ

// 頂点シェーダ入力
struct VS_INPUT
{
	float3 Pos : POSITION;				// 頂点座標(モデル座標系)
	float2 Tex : TEXCOORD;				// UVSV_InstanceID
	float3 Normal : NORMAL;				// 法線データ
	float3 blend : BLENDWEIGHT;			// ブレンド比重
	int4 idx : BLENDINDICES;			// ブレンドインデックス
	uint InstanceId : SV_InstanceID;	// インスタンスＩＤ
};

//ピクセルシェーダ入力
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD1;
	float3 Normal : NORMAL;
	float4 LocalPos : LOCAL_POSITION;	//モデル→ワールド行列をかけたもの
	float4 WorldPos : WORLD_POSITION;		//MVP行列をかけたもの
	float4 projuv : SHADOW_COORD;
	float4 WorldNormal : WORLD_NORMAL;	//ワールド変換済み法線
};

// ピクセル シェーダ出力(MRT)
struct PS_OUTPUT
{
	float4 vColor : SV_Target0;
	float4 vNormal : SV_Target1;
	float4 vPosition : SV_Target2;
};


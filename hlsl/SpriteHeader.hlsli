//特になにもしないスプライトシェーダ

//透視行列のみ。ビュー行列は単位行列。かけなくてもよい？
cbuffer cbNeverChanges : register(b0)
{
	matrix Proj;   	// 透視変換行列
	matrix View; 	  	// ビュー変換行列
};

//ワールドマトリクスバッファ
cbuffer cbChangesEveryObject : register(b1)
{
	matrix World;			// ワールド変換行列
};

Texture2D MyTex : register(t0);
SamplerState MySamp : register(s0);

// 頂点シェーダの入力データ定義
struct VS_INPUT
{
	float3 Pos : POSITION;		// 頂点座標
	float2 Tex : TEXCOORD;		// UV
};

// ピクセル シェーダの入力データ定義
struct PS_INPUT
{
	float4 Pos : SV_POSITION;	// 頂点座標
	float2 Tex : TEXCOORD0;		// UV
};

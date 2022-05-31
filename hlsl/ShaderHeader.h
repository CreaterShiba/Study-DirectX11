// 定数バッファのデータ定義①　変更しないデータ
// 常にスロット「0」を使う
cbuffer cbNeverChanges : register(b0)
{
	matrix Projection;	// 透視変換行列
};

// 定数バッファのデータ定義②　変更頻度の低いデータ
// 常にスロット「1」を使う
cbuffer cbChangesEveryFrame : register(b1)
{
	matrix View;  		 // ビュー変換行列
};

// 定数バッファのデータ定義③　変更頻度の高いデータ
// 常にスロット「2」を使う
cbuffer cbChangesEveryObject : register(b2)
{
	matrix World;		// ワールド変換行列
	matrix WLP;		// ワールド・”ライトビュー”・プロジェクションの合成
	matrix WLPT;		// ワールド、ライト、プロジェクシション、UV
	float4 vLightDir;	// ライトベクトル
	float4 vEyePos;		// 視線座標
	matrix wvp;		// 視線座標
};

// テクスチャ
Texture2D MyTex : register(t0);		//アルベド
Texture2D ZTex : register(t1);		//深度マップ
Texture2D Normal : register(t2);		//ノーマルマップ
SamplerState MySamp : register(s0);

// 頂点シェーダ入力
struct VS_INPUT
{
	float3 Pos : POSITION;		// 頂点座標(モデル座標系)
	float2 Tex : TEXCOORD;		// UV
	float3 Normal : NORMAL;		// 法線データ
};

// ピクセル シェーダ入力
struct PS_INPUT
{
	float4 Pos : SV_POSITION; 		// 頂点座標(透視座標系)
	float2 Tex : TEXCOORD0;		// UV
	float3 Normal : NORMAL;      		// 法線
};
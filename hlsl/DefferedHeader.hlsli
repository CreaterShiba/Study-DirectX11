Texture2D colorTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D posTex : register(t2);

SamplerState MySamp : register(s0);

static const int NUM_LIGHT = 12;

cbuffer cbLight : register(b0)
{
	float4 LightPos[NUM_LIGHT];
	float4 LightColor[NUM_LIGHT];
	float4 camPos;
};

struct VS_INPUT
{
	float3 pos : POSITION;		// 頂点座標
	float2 texel : TEXCOORD;	// テクセル
};

// 頂点シェーダーの出力パラメータ
struct VS_OUT
{
	float4 pos : SV_POSITION;	// 頂点座標
	float2 texel : TEXCOORD0;	// テクセル
};

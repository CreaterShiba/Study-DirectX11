
Texture2D MyTex : register(t0);		//アルベド
SamplerState MySamp : register(s0);

cbuffer per_mesh : register(b0)
{
	matrix mat[32];
	matrix view;
	matrix proj;
};

//バーテックスシェーダー出力構造体
struct VS_IN
{
	float3 pos : POSITION;
	float3 Normal : NORMAL;		// 法線データ
	float2 Tex : TEXTURE;		// UV
	float3 blend : BLENDWEIGHT;
	int4 idx : BLENDINDICES;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 Tex : TEXCOORD;		// UV
};

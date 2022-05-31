#include "Mrt.hlsli"

PS_INPUT VS(VS_INPUT input) 
{
	PS_INPUT output = (PS_INPUT)0;
	float4 localPos = float4(input.Pos, 1.0f);	//ローカル値

	if (nSkinTrue == 0)
	{
		//スキンありモデル描画
		output.Pos = mul(localPos, world);
		//output.WorldNormal = mul(float4(input.Normal, 1.0f), world);
	}
	else if (nSkinTrue == 2)
	{
		//スキンなしインスタンス描画。頂点シェーダは一回しか通らない。
		output.Pos = mul(localPos, insWorld[input.InstanceId]);
		//output.WorldNormal = mul(float4(input.Normal, 1.0f), insWorld[input.InstanceId]);
	}
	else if(nSkinTrue == 1 || nSkinTrue == 3)
	{	
		//スキン用の頂点変換処理
		//@todo インスタンスを用いたスキン描画できるように
		float w[3] = (float[3])input.blend;	//ブレンド率代入
		float4x4 comb = (float4x4)0;		//合成マトリクス

		for (int i = 0; i < 3; i++)
		{
			//行列ブレンド。SkinMatはボーン毎の行列
			comb += SkinMat[input.idx[i]] * w[i];
		}
		comb += SkinMat[input.idx[3]] * (1.0f - w[0] - w[1] - w[2]);	//最後の重さは1.0fから引いたものでいける。

		output.Pos = mul(localPos, comb);
		//output.WorldNormal = mul(float4(input.Normal, 1.0f), comb);
	}

	output.LocalPos = output.Pos;	// Defferd用の、ポリゴン座標にローカル行列をかけたもの
	output.Normal = input.Normal;	// 法線はそのまま入れてる

	//output.projuv = mul(Shadow, output.LocalPos);	//影描画用UV.ShadowにはライトのView,Projが入ってる
	output.projuv = mul(output.LocalPos, Shadow);	//影描画用UV.ShadowにはライトのView,Projが入ってる

	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, proj);
	output.WorldPos = output.Pos;	// Zレンダリング用に、MVPかけたもの保存しておく

	// UV
	output.Tex = input.Tex;

	return output;
}

//EOF

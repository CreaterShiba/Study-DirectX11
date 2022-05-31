#include "PostEffectHeader.hlsli"

float4 PS(PS_INPUT input) : SV_TARGET
{
	float4 fix = MyTex.Sample(MySamp, input.Tex);

	if (nBloomOnOff == 1)
	{
		fix += BloomTex.Sample(MySamp, input.Tex);
	}

	// 色の反転(ネガポジ)
	if (nPostEffectType == 1)
	{
		float3 negaColor = 1.0 - fix.rgb;
		fix.rgb = negaColor.rgb;
	}
	else if(nPostEffectType == 2)
	{
		const float redScale = 0.298912;
		const float greenScale = 0.586611;
		const float blueScale = 0.114478;

		// モノクロ
		// 内積を求める
		float mono = dot(fix.rgb, float3(redScale, greenScale, blueScale));
		fix = float4(mono, mono, mono, fix.a);
	}
	else if (nPostEffectType == 3)
	{
		const float redScale = 0.298912;
		const float greenScale = 0.586611;
		const float blueScale = 0.114478;

		const float sRedScale = 1.07;
		const float sGreenScale = 0.74;
		const float sBlueScale = 0.43;
		const float3  monochromeScale = float3(redScale, greenScale, blueScale);

		float grayColor = dot(fix.rgb, monochromeScale);

		float3 sepiaScale = float3(sRedScale, sGreenScale, sBlueScale);
		float3 monoColor = float3(grayColor, grayColor, grayColor) * sepiaScale;
		fix = float4(monoColor.r, monoColor.g, monoColor.b, 1.0);
	}

	float4 ui = uiTex.Sample(MySamp, input.Tex);	//UIテクスチャ。
	if (ui.a > 0.0f)
	{
		fix = ui;
	}

	//Bloomとか

	return fix;
}


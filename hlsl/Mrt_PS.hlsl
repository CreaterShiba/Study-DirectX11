#include "Mrt.hlsli"

PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT pOut = (PS_OUTPUT)0;

#if 1
	//DX10以降シャドウはハードウェアでサポートされてるらしい。PCFもかってにやってくれる
	//@note http://www.project-asura.com/program/d3d11/d3d11_008.html 参考
	// ディフューズマップをフェッチ.
	float4 diffuse = AlbedoTex.Sample(MySamp, input.Tex);
	float3 shadowColor = float3(1.0f, 1.0f, 1.0f);

	// アルファテスト.
	//clip((diffuse.a < 0.125f) ? -1.0f : 1.0f);

	// シャドウマップの深度値と比較する.
	float3 shadowCoord = input.projuv.xyz / input.projuv.w;
	//if (shadowCoord.x >= 0.0f && shadowCoord.x <= 1.0f && shadowCoord.y >= 0.0f && shadowCoord.y <= 1.0f)
	{
		float shadowBias = 0.0f;

		// より良いバイアス値を得るための手法
		//https://msdn.microsoft.com/ja-jp/library/cc308048(v=vs.85).aspx

		// 最大深度傾斜を求める.
		float  maxDepthSlope = max(abs(ddx(shadowCoord.z)), abs(ddy(shadowCoord.z)));

		float  bias = 0.01f;				// 固定バイアスです.
		float  slopeScaledBias = 0.01f;     // 深度傾斜.
		float  depthBiasClamp = 0.1f;      // バイアスクランプ値.

		shadowBias = bias + slopeScaledBias * maxDepthSlope;
		shadowBias = min(shadowBias, depthBiasClamp);

#if 1
		shadowColor = float3(0.0f, 0.0f, 0.0f);
		float  shadowThreshold = 1.0f;      // シャドウにするかどうかの閾値です.
		shadowThreshold = ShadowMap.SampleCmpLevelZero(ShadowSmp, shadowCoord.xy, shadowCoord.z - shadowBias);

		shadowColor = lerp(shadowColor, float3(1.0f, 1.0f, 1.0f), shadowThreshold);
		//shadowColor = pow(shadowColor, 3);
		//pOut.vColor.rgb = shadowThreshold;
#else
		// --------------------------- ライト目線によるZ値の再算出 ---------------------------
		// ライト目線によるZ値の再算出
		float ZValue = shadowCoord.z;	//これが書き込むピクセルの、ライト方向からみたZ値になる。

		// --------------------------- ZテクスチャからのZ値取り出し ---------------------------
		// 射影空間のXY座標をテクスチャ座標に変換
		float2 TransTexCoord;
		TransTexCoord.x = shadowCoord.x;
		TransTexCoord.y = shadowCoord.y;

		// リアルZ値抽出
		float SM_Z = ShadowMap.Sample(MySamp, TransTexCoord).x;	//事前にZバッファに入れておいたZ値。

		// --------------------------- ２つのZ値を比較して演算する。 ---------------------------
		// 算出点がシャドウマップのZ値よりも大きければ影と判断
		if (ZValue > SM_Z + shadowBias)
		{
			shadowColor = 0.5f;
		}
#endif
	}

	//適当にハーフランバート
	float p = dot(input.Normal, LightPos.xyz);
	p = p * 0.5f + 0.5f;
	p = p * p;

	pOut.vColor.rgb = shadowColor * diffuse.rgb * p * 0.2f;	//ちょっとくらく
#else
	// レンダリングピクセル
	pOut.vColor = AlbedoTex.Sample(MySamp, input.Tex);
#endif

	pOut.vColor.a = 1.0f;

	//法線
	float3 nom = input.Normal;
	nom = nom * 0.5f + 0.5f;	//-1～1を0～1に変換
	pOut.vNormal = float4(nom, 1.0f);

	//ワールド座標
	pOut.vPosition = float4(input.LocalPos.xyz, 1.0f);

	return pOut;
}

//EOF

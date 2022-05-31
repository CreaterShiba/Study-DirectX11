#include "DefferedHeader.hlsli"

float4 ps_main(VS_OUT In) : SV_TARGET
{
	float4 color = float4(0,0,0,0);

#if 1

	float3 pos = posTex.Sample(MySamp, In.texel).rgb;
	float3 nom = float3(0,0,0);
	//float d = 0.0f;
	{
		float4 nomdepth = normalTex.Sample(MySamp, In.texel);
		nom = nomdepth.rgb * 2.0f - 1.0f;	//-1~1に復元
		//d = nomdepth.a;	//この値余ってるんで何かに使えれば使う
	}

	//float3 eyeDir = normalize(camPos.xyz - pos);		//ワールド座標とカメラの視点
	float4 diffuse = colorTex.Sample(MySamp, In.texel);	//diffuseカラー値
	color = diffuse;

	for (int i = 1; i < NUM_LIGHT; ++i)
	{
		if (LightColor[i].a > 0.0f)
		{
			float3 lightDir = LightPos[i].xyz - pos;		//ワールド座標とライトの視点
			float attenution = LightColor[i].a / length(lightDir);		//ライト強さ(ここでいい？)
			float3 L = normalize(lightDir);					//正規化

			color.rgb += attenution * LightColor[i].rgb * max(dot(L, nom), 0);
			//color.rgb += attenution * LightColor[i].rgb * diffuse.rgb * max(dot(L, nom), 0);
			color.a = diffuse.a;	//アルファはそのままいれる
		}
	}

#else
	float4 diffuse = colorTex.Sample(MySamp, In.texel);	//diffuseカラー値
	color = diffuse;
#endif
	return color;
}

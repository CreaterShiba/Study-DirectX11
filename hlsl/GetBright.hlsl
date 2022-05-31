#include "SpriteHeader.hlsli"

//�P�x���o
float4 PS(PS_INPUT input) : SV_TARGET
{
	float4 fix = MyTex.Sample(MySamp, input.Tex);	//�Ƃ肠�����o������
	float3 texel;
	float fminBright = 0.2f;
	texel.r = max(0.0f, fix.r - fminBright);
	texel.g = max(0.0f, fix.g - fminBright);
	texel.b = max(0.0f, fix.b - fminBright);
	fix = float4(texel.rgb, 1.0f);

	return fix;
}


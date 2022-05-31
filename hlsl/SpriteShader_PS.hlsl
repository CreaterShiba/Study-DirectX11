#include "SpriteHeader.hlsli"

float4 PS(PS_INPUT input) : SV_TARGET
{
	float4 fix = MyTex.Sample(MySamp, input.Tex);
	return fix;
}


#include "Mrt.hlsli"

float4 PS(PS_INPUT input) : SV_TARGET
{
	float4 output;
	output = input.WorldPos.z / input.WorldPos.w;
	output.a = 1.0f;

	return output;
}

//EOF

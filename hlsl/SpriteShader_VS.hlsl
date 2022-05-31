#include "SpriteHeader.hlsli"

PS_INPUT VS(VS_INPUT input)
{
	//‚±‚ê‚¾‚¯
	PS_INPUT output;

	float4 Pos = float4( input.Pos, 1.0f );

	output.Pos = mul(Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Proj);

	// UV
	output.Tex = input.Tex;
	return output;
}

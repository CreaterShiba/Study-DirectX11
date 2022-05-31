#include "DefferedHeader.hlsli"

VS_OUT vs_main( VS_INPUT In )
{
 	VS_OUT output = { float4( In.pos, 1.0f ), In.texel };
 	output.pos.x *= 2.0f;
 	output.pos.y *= 2.0f;

	output.texel = In.texel;

 	return output;
}

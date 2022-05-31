#include "SpriteHeader.hlsli"
#include "BlurHeader.hlsli"

//�u���[
float4 PS(PS_INPUT input) : SV_TARGET
{
	float4 fix = 0.0f;
#if 0
	if (nBlurType == 0)
	{
		//�^�e�����ւ̃u���[
		for (int i = 0; i < SAMPLE_COUNT; i++) 
		{
			//���󂱂������������������ĂȂ�����
			fix += (MyTex.Sample(MySamp, input.Tex + float2(0.0f, offsets[i].y)) * weights[i]);
		}
	}
	else 
	{
		//���R�����ւ̃u���[
		for (int i = 0; i < SAMPLE_COUNT; i++) 
		{
			fix += (MyTex.Sample(MySamp, input.Tex + float2(offsets[i].x, 0.0f)) * weights[i]);
		}
	}

#else
	if (nBlurType == 0)
	{
		//�^�e�����ւ̃u���[
		for (int i = 0; i < SAMPLE_COUNT; i++)
		{
			//���󂱂������������������ĂȂ�����
			fix += (MyTex.Sample(MySamp, input.Tex + float2(0.0f, GaussParam[i].y)) * GaussParam[i].x);
		}
	}
	else
	{
		//���R�����ւ̃u���[
		for (int i = 0; i < SAMPLE_COUNT; i++)
		{
			fix += (MyTex.Sample(MySamp, input.Tex + float2(GaussParam[i].z, 0.0f)) * GaussParam[i].x);
		}
	}

#endif

	fix.a = 1.0f;
	//fix = MyTex.Sample(MySamp, input.Tex);

	return fix;
}

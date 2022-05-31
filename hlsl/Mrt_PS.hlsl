#include "Mrt.hlsli"

PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT pOut = (PS_OUTPUT)0;

#if 1
	//DX10�ȍ~�V���h�E�̓n�[�h�E�F�A�ŃT�|�[�g����Ă�炵���BPCF�������Ăɂ���Ă����
	//@note http://www.project-asura.com/program/d3d11/d3d11_008.html �Q�l
	// �f�B�t���[�Y�}�b�v���t�F�b�`.
	float4 diffuse = AlbedoTex.Sample(MySamp, input.Tex);
	float3 shadowColor = float3(1.0f, 1.0f, 1.0f);

	// �A���t�@�e�X�g.
	//clip((diffuse.a < 0.125f) ? -1.0f : 1.0f);

	// �V���h�E�}�b�v�̐[�x�l�Ɣ�r����.
	float3 shadowCoord = input.projuv.xyz / input.projuv.w;
	//if (shadowCoord.x >= 0.0f && shadowCoord.x <= 1.0f && shadowCoord.y >= 0.0f && shadowCoord.y <= 1.0f)
	{
		float shadowBias = 0.0f;

		// ���ǂ��o�C�A�X�l�𓾂邽�߂̎�@
		//https://msdn.microsoft.com/ja-jp/library/cc308048(v=vs.85).aspx

		// �ő�[�x�X�΂����߂�.
		float  maxDepthSlope = max(abs(ddx(shadowCoord.z)), abs(ddy(shadowCoord.z)));

		float  bias = 0.01f;				// �Œ�o�C�A�X�ł�.
		float  slopeScaledBias = 0.01f;     // �[�x�X��.
		float  depthBiasClamp = 0.1f;      // �o�C�A�X�N�����v�l.

		shadowBias = bias + slopeScaledBias * maxDepthSlope;
		shadowBias = min(shadowBias, depthBiasClamp);

#if 1
		shadowColor = float3(0.0f, 0.0f, 0.0f);
		float  shadowThreshold = 1.0f;      // �V���h�E�ɂ��邩�ǂ�����臒l�ł�.
		shadowThreshold = ShadowMap.SampleCmpLevelZero(ShadowSmp, shadowCoord.xy, shadowCoord.z - shadowBias);

		shadowColor = lerp(shadowColor, float3(1.0f, 1.0f, 1.0f), shadowThreshold);
		//shadowColor = pow(shadowColor, 3);
		//pOut.vColor.rgb = shadowThreshold;
#else
		// --------------------------- ���C�g�ڐ��ɂ��Z�l�̍ĎZ�o ---------------------------
		// ���C�g�ڐ��ɂ��Z�l�̍ĎZ�o
		float ZValue = shadowCoord.z;	//���ꂪ�������ރs�N�Z���́A���C�g��������݂�Z�l�ɂȂ�B

		// --------------------------- Z�e�N�X�`�������Z�l���o�� ---------------------------
		// �ˉe��Ԃ�XY���W���e�N�X�`�����W�ɕϊ�
		float2 TransTexCoord;
		TransTexCoord.x = shadowCoord.x;
		TransTexCoord.y = shadowCoord.y;

		// ���A��Z�l���o
		float SM_Z = ShadowMap.Sample(MySamp, TransTexCoord).x;	//���O��Z�o�b�t�@�ɓ���Ă�����Z�l�B

		// --------------------------- �Q��Z�l���r���ĉ��Z����B ---------------------------
		// �Z�o�_���V���h�E�}�b�v��Z�l�����傫����Ήe�Ɣ��f
		if (ZValue > SM_Z + shadowBias)
		{
			shadowColor = 0.5f;
		}
#endif
	}

	//�K���Ƀn�[�t�����o�[�g
	float p = dot(input.Normal, LightPos.xyz);
	p = p * 0.5f + 0.5f;
	p = p * p;

	pOut.vColor.rgb = shadowColor * diffuse.rgb * p * 0.2f;	//������Ƃ��炭
#else
	// �����_�����O�s�N�Z��
	pOut.vColor = AlbedoTex.Sample(MySamp, input.Tex);
#endif

	pOut.vColor.a = 1.0f;

	//�@��
	float3 nom = input.Normal;
	nom = nom * 0.5f + 0.5f;	//-1�`1��0�`1�ɕϊ�
	pOut.vNormal = float4(nom, 1.0f);

	//���[���h���W
	pOut.vPosition = float4(input.LocalPos.xyz, 1.0f);

	return pOut;
}

//EOF

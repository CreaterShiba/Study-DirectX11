
static const int SAMPLE_COUNT = 16;

cbuffer cbGaussParamObject : register(b2)
{
	//float2 offsets[SAMPLE_COUNT];	//�I�t�Z�b�g(x�̓��R�����ւ̃{�J�V�Ɏg�p�AY�̓^�e����)
	//float weights[SAMPLE_COUNT];
	float4 GaussParam[SAMPLE_COUNT];	//X:Weight YZ:UV���炷 W:���g�p
};

cbuffer cbGaussDrawType : register(b3)
{
	int nBlurType;			// �㉺�u���[�^�C�v
	float3 BlurTypeAlign;	// �A���C��
};

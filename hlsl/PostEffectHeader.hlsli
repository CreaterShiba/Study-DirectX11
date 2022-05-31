//���ɂȂɂ����Ȃ��X�v���C�g�V�F�[�_

//�����s��̂݁B�r���[�s��͒P�ʍs��B�����Ȃ��Ă��悢�H
cbuffer cbNeverChanges : register(b0)
{
	matrix Proj;   	// �����ϊ��s��
	matrix View; 	  	// �r���[�ϊ��s��
};

//���[���h�}�g���N�X�o�b�t�@
cbuffer cbChangesEveryObject : register(b1)
{
	matrix World;			// ���[���h�ϊ��s��
	int nPostEffectType;	// �|�X�g�G�t�F�N�g�^�C�v
	int nBloomOnOff;
	float2 AlignmentVec;	// �A���C��
};

Texture2D MyTex : register(t0);
Texture2D uiTex : register(t1);
Texture2D BloomTex : register(t2);

SamplerState MySamp : register(s0);

// ���_�V�F�[�_�̓��̓f�[�^��`
struct VS_INPUT
{
	float3 Pos : POSITION;		// ���_���W
	float2 Tex : TEXCOORD;		// UV
};

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT
{
	float4 Pos : SV_POSITION;	// ���_���W
	float2 Tex : TEXCOORD0;		// UV
};
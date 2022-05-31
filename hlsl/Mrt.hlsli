
//�s��n�o�b�t�@
cbuffer cbMatrixs : register(b0)
{
	matrix world;		//���[���h�}�g���N�X
	matrix SkinMat[32];	//�X�L���p�}�g���N�X
	matrix view;		//�r���[
	matrix proj;		//�v���W�F�N�V����
	matrix Shadow;		//�e�N�X�`���ϊ��s��
};

//�p�����[�^
cbuffer cbParam : register(b1)
{
	int nSkinTrue;		//0: 1: 2: 3:
	float3 LightPos;	//���C�g���[���h���W
}

//�C���X�^���V���O�s��
cbuffer cbInstancingParam : register(b2)
{
	matrix insWorld[128];		//�C���X�^���V���O�p���[�J���}�g���N�X
}


//�e��ϐ�
Texture2D AlbedoTex : register(t0);	//�A���x�h
Texture2D NormalTex : register(t1);		//�m�[�}���}�b�v
Texture2D ShadowMap : register(t2);		//�V���h�E�p Z�e�N�X�`��

SamplerState MySamp : register(s0);					//�T���v��
SamplerComparisonState ShadowSmp : register(s1);	//�V���h�E����p�̔�r�T���v��

// ���_�V�F�[�_����
struct VS_INPUT
{
	float3 Pos : POSITION;				// ���_���W(���f�����W�n)
	float2 Tex : TEXCOORD;				// UVSV_InstanceID
	float3 Normal : NORMAL;				// �@���f�[�^
	float3 blend : BLENDWEIGHT;			// �u�����h��d
	int4 idx : BLENDINDICES;			// �u�����h�C���f�b�N�X
	uint InstanceId : SV_InstanceID;	// �C���X�^���X�h�c
};

//�s�N�Z���V�F�[�_����
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD1;
	float3 Normal : NORMAL;
	float4 LocalPos : LOCAL_POSITION;	//���f�������[���h�s�������������
	float4 WorldPos : WORLD_POSITION;		//MVP�s�������������
	float4 projuv : SHADOW_COORD;
	float4 WorldNormal : WORLD_NORMAL;	//���[���h�ϊ��ςݖ@��
};

// �s�N�Z�� �V�F�[�_�o��(MRT)
struct PS_OUTPUT
{
	float4 vColor : SV_Target0;
	float4 vNormal : SV_Target1;
	float4 vPosition : SV_Target2;
};


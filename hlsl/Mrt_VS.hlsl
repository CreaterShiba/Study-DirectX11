#include "Mrt.hlsli"

PS_INPUT VS(VS_INPUT input) 
{
	PS_INPUT output = (PS_INPUT)0;
	float4 localPos = float4(input.Pos, 1.0f);	//���[�J���l

	if (nSkinTrue == 0)
	{
		//�X�L�����胂�f���`��
		output.Pos = mul(localPos, world);
		//output.WorldNormal = mul(float4(input.Normal, 1.0f), world);
	}
	else if (nSkinTrue == 2)
	{
		//�X�L���Ȃ��C���X�^���X�`��B���_�V�F�[�_�͈�񂵂��ʂ�Ȃ��B
		output.Pos = mul(localPos, insWorld[input.InstanceId]);
		//output.WorldNormal = mul(float4(input.Normal, 1.0f), insWorld[input.InstanceId]);
	}
	else if(nSkinTrue == 1 || nSkinTrue == 3)
	{	
		//�X�L���p�̒��_�ϊ�����
		//@todo �C���X�^���X��p�����X�L���`��ł���悤��
		float w[3] = (float[3])input.blend;	//�u�����h�����
		float4x4 comb = (float4x4)0;		//�����}�g���N�X

		for (int i = 0; i < 3; i++)
		{
			//�s��u�����h�BSkinMat�̓{�[�����̍s��
			comb += SkinMat[input.idx[i]] * w[i];
		}
		comb += SkinMat[input.idx[3]] * (1.0f - w[0] - w[1] - w[2]);	//�Ō�̏d����1.0f������������̂ł�����B

		output.Pos = mul(localPos, comb);
		//output.WorldNormal = mul(float4(input.Normal, 1.0f), comb);
	}

	output.LocalPos = output.Pos;	// Defferd�p�́A�|���S�����W�Ƀ��[�J���s�������������
	output.Normal = input.Normal;	// �@���͂��̂܂ܓ���Ă�

	//output.projuv = mul(Shadow, output.LocalPos);	//�e�`��pUV.Shadow�ɂ̓��C�g��View,Proj�������Ă�
	output.projuv = mul(output.LocalPos, Shadow);	//�e�`��pUV.Shadow�ɂ̓��C�g��View,Proj�������Ă�

	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, proj);
	output.WorldPos = output.Pos;	// Z�����_�����O�p�ɁAMVP���������̕ۑ����Ă���

	// UV
	output.Tex = input.Tex;

	return output;
}

//EOF

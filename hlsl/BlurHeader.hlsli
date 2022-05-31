
static const int SAMPLE_COUNT = 16;

cbuffer cbGaussParamObject : register(b2)
{
	//float2 offsets[SAMPLE_COUNT];	//オフセット(xはヨコ方向へのボカシに使用、Yはタテ方向)
	//float weights[SAMPLE_COUNT];
	float4 GaussParam[SAMPLE_COUNT];	//X:Weight YZ:UVずらす W:未使用
};

cbuffer cbGaussDrawType : register(b3)
{
	int nBlurType;			// 上下ブラータイプ
	float3 BlurTypeAlign;	// アライン
};

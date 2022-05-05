struct VSInput
{
	float3 Position : POSITION; //���_���W
	float Color : COLOR;//���_�J���[�ł�
};


struct VSOutput {
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};


cbuffer Transform : register(b0) {

	float4x4 World : packoffset(c0);//���[���h���W
	float4x4 View : packoffset(c4); //�r���[���W
	float4x4 Proj : packoffset(c8);//�ˉe�s��

}


VSOutput main( VSInput input )
{
	VSOutput output = (VSOutput)0;

	float4 localPos = float4(input.Position, 1.0f);
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(view, worldPos);
	float4 projPos = mul(Proj, viewPos);

	output.Position = projPos;
	output.Color = input.Color;

	return output;
}
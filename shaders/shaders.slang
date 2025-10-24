struct VertexInput {
    float2 inPosition;
    float3 inColor;
    float2 inTexCoord;
};

struct VertexOutput {
    float3 color;
    float4 pos : SV_Position;
    float2 textCoord;
};

struct UniformBuffer {
    float4x4 view;
    float4x4 proj;
};

ConstantBuffer<UniformBuffer> ubo;

struct PushConstants {
    float4x4 model;
    int isTextured;
    int textureIndex;
};

[[vk::push_constant]]
PushConstants push;

[[vk::binding(1)]]
Texture2D gTextures[];

[[vk::binding(1)]]
SamplerState gSamplers[];

[shader ("vertex")]
VertexOutput vertMain(VertexInput input) {
    VertexOutput output;
    output.pos = mul(ubo.proj, mul(ubo.view, mul(push.model, float4(input.inPosition, 0.0, 1.0))));
    output.color = input.inColor;
    output.textCoord = input.inTexCoord;
    return output;
}

[shader ("fragment")]
float4 fragMain (VertexOutput inVert) : SV_Target
{
    if (push.isTextured == 0)
        return float4(inVert.color, 1.0);
    else
        return gTextures[push.textureIndex].Sample(gSamplers[push.textureIndex], inVert.textCoord);
}

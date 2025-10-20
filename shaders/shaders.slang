struct VertexInput {
    float2 inPosition;
    float3 inColor;
};

struct VertexOutput {
    float3 color;
    float4 pos : SV_Position;
};

struct UniformBuffer {
    float4x4 view;
    float4x4 proj;
};

ConstantBuffer<UniformBuffer> ubo;

struct PushConstants {
    float4x4 model;
    int isTextured;
};

[[vk::push_constant]]
PushConstants push;

[shader ("vertex")]
VertexOutput vertMain(VertexInput input) {
    VertexOutput output;
    output.pos = mul(ubo.proj, mul(ubo.view, mul(push.model, float4(input.inPosition, 0.0, 1.0))));
    output.color = input.inColor;
    return output;
}

[shader ("fragment")]
float4 fragMain (VertexOutput inVert) : SV_Target
{
    if (push.isTextured == 0)
        return float4(inVert.color, 1.0);
    else
        return float4(1.0, 1.0, 0.0, 1.0);
}

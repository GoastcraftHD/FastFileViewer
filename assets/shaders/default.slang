struct VertexIn
{
    float2 position;
    float3 color;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float3 color;
};

[shader("vertex")]
VertexOut vertexMain(VertexIn input)
{
    VertexOut output;
    output.position = float4(input.position, 0.0, 1.0);
    output.color = input.color;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VertexOut vertex) : SV_TARGET
{
    return float4(vertex.color, 1.0);
}

//struct VertexIn
//{
//    float2 position : POSITION;
//};

struct VertexOut
{
    float4 position : SV_POSITION;
};

[shader("vertex")]
VertexOut vertexMain(uint vertexId : SV_VertexID)
{
    float2 verts[3] ={
        float2(-0.7, 0.7),
        float2(0.7, 0.7),
        float2(0.0, -0.7)
    };

    VertexOut vert;
    //vert.position = float4(vertex.position, 0.0, 1.0);
    vert.position = float4(verts[vertexId], 0.0, 1.0);
    return vert;
}

[shader("fragment")]
float4 fragmentMain(VertexOut vertex)
{
    return float4(1.0, 1.0, 0.0, 1.0);
}

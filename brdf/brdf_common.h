__device__ 
inline float dot(float3 v0, float3 v1)
{
    return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z;
}

__device__ 
inline float3 scale(float3 v0, float scalar)
{
    float3 res = v0;
    res.x *= scalar;
    res.y *= scalar;
    res.z *= scalar;
    return res;
}

__device__
inline float3 add(float3 v0, float3 v1)
{
    float3 res = v0;
    res.x += v1.x;
    res.y += v1.y;
    res.z += v1.z;
    return res;
}

__device__ 
inline float3 minus(float3 v0, float3 v1)
{
    float3 res = v0;
    res.x -= v1.x;
    res.y -= v1.y;
    res.z -= v1.z;
    return res;
}

__device__
inline float3 normalize(float3 v0)
{
    float3 res = v0;
    float len = sqrt(v0.x * v0.x + v0.y * v0.y + v0.z * v0.z);
    if(len > 10e-6)
    {
        res.x /= len;
        res.y /= len;
        res.z /= len;
    }
    else
    {
        res.x = res.y = res.z = 0.57735f;
    }
    
    return res;
}

__device__
inline float sqr(float x)
{
    return x * x;
}

__device__ 
inline float3 reflect(float3 I, float3 N)
{
    return  minus(scale(N, 2.f * dot(I,N)), I);
}

__device__
inline float3 calculateL(unsigned int width, unsigned int height, unsigned int x, unsigned int y)
{
    float phi = 3.1415926f * 2.f / (float) width * x;
    float theta = 3.1415926f * 0.5f / (float) height * y;

    return make_float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));   
}



#ifndef AK_RENDERER_RANDOM_H
#define AK_RENDERER_RANDOM_H

void Get_Random_Seed(void* Data, uint32_t Size);

struct random32
{
    uint32_t Seed;
    
    uint32_t RandomU32();
    int32_t  RandomI32();
    float    Random_UNorm();
    float    Random_SNorm();
    int32_t  Random_Between(int32_t Min, int32_t Max);
    float    Random_Between(float Min, float Max);
};

struct random64
{
    uint64_t Seed;
    
    uint64_t RandomU64();
    int64_t  RandomI64();
    double   Random_UNorm();
    double   Random_SNorm();
    int64_t  Random_Between(int64_t Min, int64_t Max);
    double   Random_Between(double Min,  double Max);
};

random32 Init_Random32(uint32_t Seed);
random32 Init_Random32();

random64 Init_Random64(uint64_t Seed);
random64 Init_Random64();

#endif

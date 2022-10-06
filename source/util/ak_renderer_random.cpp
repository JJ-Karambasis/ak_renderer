#ifdef _WIN32
void Get_Random_Seed(void* Data, uint32_t Size)
{
    HCRYPTPROV Prov = 0;
    CryptAcquireContextW(&Prov, 0, 0, PROV_DSS, CRYPT_VERIFYCONTEXT);
    CryptGenRandom(Prov, Size, (BYTE*)Data);
    CryptReleaseContext(Prov, 0);
}
#endif

uint32_t random32::RandomU32()
{
    uint32_t Result = Seed;
    Result ^= Result << 13;
    Result ^= Result >> 17;
    Result ^= Result << 5;
    Seed = Result;
    return Result;
}

int32_t random32::RandomI32()
{
    return (int32_t)RandomU32();
}

float random32::Random_UNorm()
{
    float Divisor = 1.0f / (float)0xFFFFFFFF;
    float Result = Divisor*(float)RandomU32();
    return Result;
}

float random32::Random_SNorm()
{
    float Result = 2.0f*Random_UNorm() - 1.0f;
    return Result;
}

int32_t random32::Random_Between(int32_t Min, int32_t Max)
{
    int32_t Denominator = ((Max+1) - Min);
    Assert(Denominator != 0);
    int32_t Result = Min + (int32_t)(RandomU32() % Denominator);
    return Result;
}

float random32::Random_Between(float Min, float Max)
{
    float Result = Lerp(Min, Random_UNorm(), Max);
    return Result;
}

uint64_t random64::RandomU64()
{
    uint64_t Result = Seed;
    Result ^= Result << 13;
    Result ^= Result >> 7;
    Result ^= Result << 17;
    Seed = Result;
    return Result;
}

int64_t random64::RandomI64()
{
    return (int64_t)RandomU64();
}

double random64::Random_UNorm()
{
    double Divisor = 1.0 / (double)0xFFFFFFFFFFFFFFFF;
    double Result = Divisor*(double)RandomU64();
    return Result;
}

double random64::Random_SNorm()
{
    double Result = 2.0*Random_UNorm() - 1.0;
    return Result;
}

int64_t random64::Random_Between(int64_t Min, int64_t Max)
{
    int64_t Result = Min + (int64_t)(RandomU64() % ((Max+1) - Min));
    return Result;
}

double random64::Random_Between(double Min, double Max)
{
    double Result = Lerp(Min, Random_UNorm(), Max);
    return Result;
}

random32 Init_Random32(uint32_t Seed)
{
    random32 Random = {};
    Random.Seed = Seed;
    return Random;
}

random32 Init_Random32()
{
    uint32_t Seed;
    Get_Random_Seed(&Seed, sizeof(Seed));
    return Init_Random32(Seed);
}

random64 Init_Random64(uint64_t Seed)
{
    random64 Random = {};
    Random.Seed = Seed;
    return Random;
}

random64 Init_Random64()
{
    uint64_t Seed;
    Get_Random_Seed(&Seed, sizeof(Seed));
    return Init_Random64(Seed);
}
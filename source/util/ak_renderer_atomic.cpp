inline void spin_lock::Lock()
{
    int64_t PreviousTarget = Atomic_Add64(&Target, 1);
    while(PreviousTarget != Current) { _mm_pause(); }
}

inline void spin_lock::Unlock()
{
    Atomic_Increment64(&Current);
}


temp_lock::temp_lock(spin_lock* Lock)
{
    Begin(Lock);
}

void temp_lock::Begin(spin_lock* Lock)
{
    TempLock = Lock;
    TempLock->Lock();
}

void temp_lock::End()
{
    if(TempLock)
    {
        TempLock->Unlock();
        TempLock = NULL;
    }
}

temp_lock::~temp_lock()
{
    End();
}

#ifdef _MSC_VER
inline int32_t Atomic_Increment32(volatile int32_t* Addend)
{
    int32_t Result = _InterlockedIncrement((volatile long*)Addend);
    return Result;
}

inline int64_t Atomic_Increment64(volatile int64_t* Addend)
{
    int64_t Result = _InterlockedIncrement64(Addend);
    return Result;
}

inline int32_t Atomic_Decrement32(volatile int32_t* Addend)
{
    int32_t Result = _InterlockedDecrement((volatile long*)Addend);
    return Result;
}

inline int64_t Atomic_Decrement64(volatile int64_t* Addend)
{
    int64_t Result = _InterlockedDecrement64(Addend);
    return Result;
}

inline int32_t Atomic_Add32(volatile int32_t* Addend, int32_t Value)
{
    int32_t Result = _InterlockedExchangeAdd((volatile long*)Addend, Value);
    return Result;
}

inline int64_t Atomic_Add64(volatile int64_t* Addend, int64_t Value)
{
    int64_t Result = _InterlockedExchangeAdd64(Addend, Value);
    return Result;
}

inline int32_t Atomic_Exchange32(volatile int32_t* Dst, int32_t Value)
{
    int32_t Result = _InterlockedExchange((volatile long*)Dst, Value);
    return Result;
}

inline int64_t Atomic_Exchange64(volatile int64_t* Dst, int64_t Value)
{
    int64_t Result = _InterlockedExchange64(Dst, Value);
    return Result;
}

inline void* Atomic_ExchangePtr(volatile void** Dst, void* Value)
{
    void* Result = _InterlockedExchangePointer((volatile PVOID*)Dst, Value);
    return Result;
}

inline int32_t Atomic_Compare_Exchange32(volatile int32_t* Dst, int32_t Exchange, int32_t Comperand)
{
    int32_t Result = _InterlockedCompareExchange((volatile long*)Dst, Exchange, Comperand);
    return Result;
}

inline int64_t Atomic_Compare_Exchange64(volatile int64_t* Dst, int64_t Exchange, int64_t Comperand)
{
    int64_t Result = _InterlockedCompareExchange64(Dst, Exchange, Comperand);
    return Result;
}

inline void* Atomic_Compare_ExchangePtr(volatile void** Dst, void* Exchange, void* Comperand)
{
    void* Result = _InterlockedCompareExchangePointer((volatile PVOID*)Dst, Exchange, Comperand);
    return Result;
}
#endif
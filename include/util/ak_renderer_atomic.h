#ifndef AK_RENDERER_ATOMIC_H
#define AK_RENDERER_ATOMIC_H

struct spin_lock
{
    volatile int64_t Target;
    volatile int64_t Current;
    
    void Lock();
    void Unlock();
};

struct temp_lock
{
    spin_lock* TempLock;
    temp_lock(spin_lock* Lock);
    void Begin(spin_lock* Lock);
    void End();
    ~temp_lock();
};

int32_t Atomic_Increment32(volatile int32_t* Addend);
int64_t Atomic_Increment64(volatile int64_t* Addend);
int32_t Atomic_Decrement32(volatile int32_t* Addend);
int64_t Atomic_Decrement64(volatile int64_t* Addend);
int32_t Atomic_Add32(volatile int32_t* Addend, int32_t Value);
int64_t Atomic_Add64(volatile int64_t* Addend, int64_t Value);
int32_t Atomic_Exchange32(volatile int32_t* Dst, int32_t Value);
int64_t Atomic_Exchange64(volatile int64_t* Dst, int64_t Value);
void*   Atomic_ExchangePtr(volatile void** Dst, void* Value);
int32_t Atomic_Compare_Exchange32(volatile int32_t* Dst, int32_t Exchange, int32_t Comperand);
int64_t Atomic_Compare_Exchange64(volatile int64_t* Dst, int64_t Exchange, int64_t Comperand);
void*   Atomic_Compare_ExchangePtr(volatile void** Dst, void* Exchange, void* Comperand);

#endif
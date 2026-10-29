#ifndef _CAS_H_
#define _CAS_H_

// Native control words can be shared with another process. Keep their existing
// representation and use the platform's interprocess operations on aligned words.
namespace cqyh
{
inline INT32 SharedLoad32(const volatile INT32* value)
{
#if defined(WIN32)
    return InterlockedCompareExchange(reinterpret_cast<volatile LONG*>(const_cast<volatile INT32*>(value)), 0, 0);
#elif defined(__linux__)
    static_assert(__atomic_always_lock_free(sizeof(INT32), nullptr));
    return __atomic_load_n(value, __ATOMIC_SEQ_CST);
#else
#error "Unknown shared-memory platform"
#endif
}
inline void SharedStore32(volatile INT32* value, INT32 next)
{
#if defined(WIN32)
    InterlockedExchange(reinterpret_cast<volatile LONG*>(value), next);
#elif defined(__linux__)
    __atomic_store_n(value, next, __ATOMIC_SEQ_CST);
#endif
}
}

#if defined(WIN32)
	inline INT32 _EVA32(INT32 volatile* pDest, INT32 Value)
	{
		return InterlockedExchange((LONG*)pDest, Value);
	}
    inline INT32 _CAS32(INT32 volatile* pDestination, INT32 Exchange, INT32 Comparand)
    {
        return InterlockedCompareExchange((LONG*)pDestination, Exchange, Comparand);
    }
    inline INT64 _CAS(INT64 volatile* pDestination, INT64 Exchange, INT64 Comparand)
    {
        return InterlockedCompareExchange64(pDestination, Exchange, Comparand);
    }

//#elif defined(LINUX32) 
#elif defined(__linux__) 
    inline INT32 _EVA32(INT32 volatile* pDest, INT32 Value)
    {
        return __atomic_exchange_n(pDest, Value, __ATOMIC_SEQ_CST);
    }
    inline INT32 _CAS32(INT32 volatile* pDestination, INT32 Exchange, INT32 Comparand)
    {
        __atomic_compare_exchange_n(pDestination, &Comparand, Exchange, false,
                                    __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
        return Comparand;
    }
    inline INT64 _CAS(INT64 volatile* pDestination, INT64 Exchange, INT64 Comparand)
    {
		__atomic_compare_exchange_n(pDestination, &Comparand, Exchange, false,
                                    __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
        return Comparand;
    }
#else
    #error("unknown os")
#endif 

#endif  // _CAS_H_

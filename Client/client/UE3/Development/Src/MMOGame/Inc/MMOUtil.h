#ifndef __MMOUTIL_H__
#define __MMOUTIL_H__

#include "MMOPrerequisites.h"
#include <vector>
#include <map>
#include <set>
#include <limits>
#include <new>

// Fixed wire fields may fill their complete array without a terminator.
template<size_t Size>
inline FString ReadFixedUTF8(const char (&field)[Size])
{
    char terminated[Size + 1];
    appMemcpy(terminated, field, Size);
    terminated[Size] = 0;
    return FString(UTF8_TO_TCHAR(terminated));
}

class ComparFString : public std::less<FString>
{
public:

	bool operator()(const FString& _Left, const FString& _Right) const
	{	// apply operator< to operands
		return ((_Left < _Right) == TRUE);
	}
};

#if M_MALLOC_PROF_GAME
struct FScopeMemProfiler
{
	FString _TagName;
	FScopeMemProfiler(const FString& tagName)
		: _TagName(tagName)
	{
		GMalloc->Exec(
			*FString::Printf(TEXT("SNAPSHOTMEMORY [%s]Begin"), *tagName), *GLog);
	}

	~FScopeMemProfiler()
	{
		GMalloc->Exec(
			*FString::Printf(TEXT("SNAPSHOTMEMORY [%s]End"), *_TagName), *GLog);
	}
};

#	define M_SCOPE_MEM_PROF(varName, tagName) FScopeMemProfiler __memProf_##varName(tagName);
#else
#	define M_SCOPE_MEM_PROF(...)
#endif


/** \addtogroup Core
*  @{
*/
/** \addtogroup Memory
*  @{
*/
/**
Wrapper class for operating as an STL container allocator.
This class acts as the host for a configured allocation policy.

It takes the following template parameters:-
<ul>
<li>T			: the type we will be allocating (see rebind later)</li>
<li>AllocPolicy : a memory allocator policy </li>
</ul>

See StdAllocPolicy for an example of implementing a Policy. The reason for
implementing the allocation in a policy is that it makes implementing
a custom allocator easier -  you just have to provide an alternative
policy with just allocate/deallocate implementations; this class does all the
housekeeping associated with keeping the STL happy.
*/

// Base STL allocator class.
template<typename T>
struct STLAllocatorBase
{	// base class for generic allocators
	typedef T value_type;
};

// Base STL allocator class. (const T version).
template<typename T>
struct STLAllocatorBase<const T>
{	// base class for generic allocators for const T
	typedef T value_type;
};

template
	<
	typename T,
	typename AllocPolicy
	>
class STLAllocator : public STLAllocatorBase<T>
{
public :
	/// define our types, as per ISO C++
	typedef STLAllocatorBase<T>			Base;
	typedef typename Base::value_type	value_type;
	typedef value_type*					pointer;
	typedef const value_type*			const_pointer;
	typedef value_type&					reference;
	typedef const value_type&			const_reference;
	typedef std::size_t					size_type;
	typedef std::ptrdiff_t				difference_type;


	/// the standard rebind mechanism
	template<typename U>
	struct rebind
	{
		typedef STLAllocator<U, AllocPolicy> other;
	};

	/// ctor
	inline explicit STLAllocator()
	{ }

	/// dtor
	virtual ~STLAllocator()
	{ }

	/// copy ctor - done component wise
	inline STLAllocator( STLAllocator const& )
	{ }

	/// cast
	template <typename U>
	inline STLAllocator( STLAllocator<U, AllocPolicy> const& )
	{ }

	/// cast
	template <typename U, typename P>
	inline STLAllocator( STLAllocator<U, P> const& )
	{ }

	/// memory allocation (elements, used by STL)
	inline pointer allocate( size_type count,
		typename std::allocator<void>::const_pointer ptr = 0 )
	{
		(void)ptr;
		// convert request to bytes
		if (count > max_size())
			throw std::bad_alloc();
		const size_type sz = count * sizeof(T);
		pointer p  = static_cast<pointer>(AllocPolicy::allocateBytes(sz));
		if (p == NULL && count != 0)
			throw std::bad_alloc();
		return p;
	}

	/// memory deallocation (elements, used by STL)
	inline void deallocate( pointer ptr, size_type )
	{
		// convert request to bytes, but we can't use this?
		// register size_type sz = count*sizeof( T );
		AllocPolicy::deallocateBytes(ptr);
	}

	pointer address(reference x) const
	{
		return &x;
	}

	const_pointer address(const_reference x) const
	{
		return &x;
	}

	size_type max_size() const throw()
	{
		// maximum size this can handle, delegate
		return AllocPolicy::getMaxAllocationSize() / sizeof(T);
	}

	void construct(pointer p)
	{
		// call placement new
		new(static_cast<void*>(p)) T();
	}

	void construct(pointer p, const T& val)
	{
		// call placement new
		new(static_cast<void*>(p)) T(val);
	}

	void destroy(pointer p)
	{
		// do we have to protect against non-classes here?
		// some articles suggest yes, some no
		p->~T();
	}

};

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename T2, typename P>
inline bool operator==(STLAllocator<T,P> const&,
	STLAllocator<T2,P> const&)
{
	// same alloc policy (P), memory can be freed
	return true;
}

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename P, typename OtherAllocator>
inline bool operator==(STLAllocator<T,P> const&,
	OtherAllocator const&)
{
	return false;
}
/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename T2, typename P>
inline bool operator!=(STLAllocator<T,P> const&,
	STLAllocator<T2,P> const&)
{
	// same alloc policy (P), memory can be freed
	return false;
}

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename P, typename OtherAllocator>
inline bool operator!=(STLAllocator<T,P> const&,
	OtherAllocator const&)
{
	return true;
}


/** @} */
/** @} */


class UE3STLAllocatorPolicy
{
public:
	static inline void* allocateBytes(size_t count, 
		const char* file = 0, int line = 0, const char* func = 0)
	{
		if (count > getMaxAllocationSize())
			throw std::bad_alloc();
		return GMalloc->Malloc((DWORD)count);
	}
	static inline void deallocateBytes(void* ptr)
	{
		GMalloc->Free(ptr);
	}
	/// Get the maximum size of a single allocation
	static inline size_t getMaxAllocationSize()
	{
		return (std::numeric_limits<DWORD>::max)();
	}

private:
	// No instantiation
	UE3STLAllocatorPolicy()
	{ }
};

#define M_UE3_STL_ALLOC(T) STLAllocator<T, UE3STLAllocatorPolicy>
#define M_UE3_STL_ALLOC_MAP(K, V) std::less<K>, STLAllocator< std::pair<K, V>, UE3STLAllocatorPolicy >
#define M_UE3_STL_ALLOC_MAP_GRT(K, V) std::greater<K>, STLAllocator< std::pair<K, V>, UE3STLAllocatorPolicy >

typedef std::vector<INT,  M_UE3_STL_ALLOC(INT) > STLIntVector;
typedef std::map<INT, INT, M_UE3_STL_ALLOC_MAP(INT, INT) > STLInt2IntMap;

#endif
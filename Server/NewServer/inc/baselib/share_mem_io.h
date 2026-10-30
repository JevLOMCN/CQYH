#ifndef _SHARE_MEM_IO_H_
#define _SHARE_MEM_IO_H_

#include "share_mem.h"
#include <cstddef>

#define SHARED_MEM_TAG 0x6434ABCD

#pragma pack(push, 1)

struct MemHeader
{
	INT32 nMemTag;
	INT32 nBlockSize;
	INT32 nReadOffset;
	INT32 nWriteOffset;
};

#pragma pack(pop)

class CShareMemBuffer
{
public:
	CShareMemBuffer(void);
	~CShareMemBuffer(void);
    CShareMemBuffer(const CShareMemBuffer&) = delete;
    CShareMemBuffer& operator=(const CShareMemBuffer&) = delete;

	BOOL Create(const char* szName, INT32 nSize, INT32 nUserHeaderSize);

	BOOL Open(const char* szName);
    BOOL Open(const char* szName, INT32 nUserHeaderSize);

    // Single producer/single consumer, capacity nSize-1 bytes. Lifecycle operations
    // and Clear require both participants to have stopped accessing this view.
	void Clear(void);

	void Destroy(void);

	BOOL Write(const char* pBuff, INT32 nSize);

	BOOL Read(char* pBuff, INT32 nSize);

	inline void* GetUserHeader(void);

	inline const char* GetData(void) const;

private:
    BOOL Attach(INT32 userHeaderSize);
    bool ReadOffsets(std::size_t& read, std::size_t& write) const;
    MemPtr m_Ptr{};
    MemHeader* m_pHeader = nullptr;
    char* m_pUserBlock = nullptr;
    INT32 m_nBlockSize = 0;
};

inline void* CShareMemBuffer::GetUserHeader(void)
{
	return m_Ptr.pSharedMem;
}

inline const char* CShareMemBuffer::GetData(void) const
{
	return m_pUserBlock;
}

#endif // _share_mem_io_h__

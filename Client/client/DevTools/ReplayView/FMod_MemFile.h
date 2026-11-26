//----------------------------------------------------------------
// Desc: 内存文件,硬盘文件加载器
// Edit: ShowLong, http://blog.csdn.net/showlong
//
// Date Created: 2010-06-11
//
// Update Date :
//
// Copyright (c) Fuel4D (http://www.fuel4d.com), All rights reserved.
//
// 注意：暂不支持对大于2G的文件进行读写操作
//----------------------------------------------------------------
#pragma once


class FMemFile
{
    FDECLARE_CLASS(FMemFile)
public:
    // 默认空构造
    explicit FMemFile()
        :pMemBuffer(0)
        ,nMemLength(0)
        ,nPosition(0)
        ,nFileSize(0)
        ,bCheckFileSize(false)
        ,bAutoDelete(true)
    {}
    // 初始指定大小的内存
    explicit FMemFile(size_t init_size, bool auto_delete = true)
        :nPosition(0)
        ,nFileSize(0)
        ,bCheckFileSize(false)
        ,bAutoDelete(auto_delete)
    {
        pMemBuffer = (init_size > 0 ? (BYTE*)calloc(init_size, 1) : 0);
        nMemLength = pMemBuffer ? init_size : 0;
    }
    // 以其它内存地址来初始化本类
    explicit FMemFile(const BYTE* _data, size_t length, bool auto_delete = true)
        :pMemBuffer((BYTE*)_data)
        ,nMemLength(_data ? length : 0)
        ,nPosition(0)
        ,nFileSize(0)
        ,bCheckFileSize(false)
        ,bAutoDelete(auto_delete)
    {}
    // 以文件初始化本类，自动加载该文件
    explicit FMemFile(const WCHAR* filename, const WCHAR* mode, bool auto_delete = true)
        :pMemBuffer(0)
        ,nMemLength(0)
        ,nPosition(0)
        ,nFileSize(0)
        ,bCheckFileSize(false)
        ,bAutoDelete(auto_delete)
    {
        LoadFileW(filename, mode);
    }
    explicit FMemFile(const ACHAR* filename, const ACHAR* mode, bool auto_delete = true)
        :pMemBuffer(0)
        ,nMemLength(0)
        ,nPosition(0)
        ,nFileSize(0)
        ,bCheckFileSize(false)
        ,bAutoDelete(auto_delete)
    {
        LoadFileA(filename, mode);
    }
    virtual~FMemFile()
    {
        if (bAutoDelete)
            Release();
    }
	// 释放由本类创建的内存
	static FINLINE void Free(BYTE*& p)
	{
		free(p);
		p = 0;
	}
    // 获取位置
    FINLINE size_t Size() { return nPosition; }
    FINLINE size_t Size() const { return nPosition; }
    // 获取容量
    FINLINE size_t Capacity() { return nMemLength; }
    FINLINE size_t Capacity() const { return nMemLength; }
    FINLINE size_t Remaining() const
    {
        const size_t length = bCheckFileSize && nFileSize < nMemLength ? nFileSize : nMemLength;
        return nPosition <= length ? length - nPosition : 0;
    }
    // 取首地址
    FINLINE BYTE* Get() { return pMemBuffer; }
    FINLINE BYTE* Get() const { return pMemBuffer; }
    // 定位
    FINLINE void Seek(size_t pos) { nPosition = pos < nMemLength ? pos : nMemLength; }
    // 当前内存是否为空
    FINLINE bool IsLoad() { return pMemBuffer != 0; }
    FINLINE bool IsLoad() const { return pMemBuffer != 0; }
    // 仅仅是清理内存标记，不释放
    FINLINE void Clear() { nPosition = 0; }
    // 手动填充新数据
    FINLINE void Fill(BYTE* _data, size_t length, bool auto_delete)
    {
        Release();

        pMemBuffer = _data;
        nMemLength = _data ? length : 0;
        nPosition = 0;
        bAutoDelete = auto_delete;
    }
    // 释放数据
    FINLINE void Release()
    {
        nPosition = nMemLength = nFileSize = 0;
        bCheckFileSize = false;
        if (bAutoDelete && pMemBuffer)
            Free(pMemBuffer);
        pMemBuffer = 0;
    }
    // 重新分配
    FINLINE void ReAllocate(size_t newsize)
    {
        if (newsize <= nMemLength)
            return;
        BYTE* pNewBuffer = (BYTE*)calloc(newsize, 1);
        if (!pNewBuffer)
            return;
        if (pMemBuffer)
        {
            memcpy(pNewBuffer, pMemBuffer, nMemLength);
            if (bAutoDelete)
                Free(pMemBuffer);
        }
        bAutoDelete = true;
        pMemBuffer = pNewBuffer;
        nMemLength = newsize;
    }
    // 写入数据
    FINLINE void* WriteByte(const void* _str, size_t _size, size_t _count)
    {
        if (!_str || !_size || _count > (size_t)-1 / _size)
            return 0;
        size_t _data_count = _size * _count;
        if (_data_count <= 0)
            return 0;
        if (nPosition > nMemLength || _data_count > (size_t)-1 - nPosition)
            return 0;
        const size_t required = nPosition + _data_count;
        const size_t sourceOffset = (size_t)_str - (size_t)pMemBuffer;
        const bool sourceInBuffer = pMemBuffer && sourceOffset <= nMemLength;
        if (sourceInBuffer && _data_count > nMemLength - sourceOffset)
            return 0;
        if (required > nMemLength)
        {
            const size_t extra = required >> 1;
            ReAllocate(required <= (size_t)-1 - extra ? required + extra : required);
        }
        if (!pMemBuffer || required > nMemLength)
            return 0;
        BYTE* dst_pos = pMemBuffer + nPosition;
        memmove(dst_pos, sourceInBuffer ? pMemBuffer + sourceOffset : _str, _data_count);
        nPosition += _data_count;
        return dst_pos;
    }
    // 读取数据
    FINLINE void* ReadByte(void* _str, size_t _size, size_t _count)
    {
        if (!_str || !_size || _count > (size_t)-1 / _size)
            return 0;
        size_t _data_count = _size * _count;
        if (_data_count <= 0)
            return 0;
        if (!pMemBuffer)
            return 0;
        if (nPosition > nMemLength || _data_count > nMemLength - nPosition)
            return 0;
        if (bCheckFileSize && (nPosition > nFileSize || _data_count > nFileSize - nPosition))
            return 0;
        const size_t destinationOffset = (size_t)_str - (size_t)pMemBuffer;
        if (destinationOffset <= nMemLength && _data_count > nMemLength - destinationOffset)
            return 0;
        memmove(_str, pMemBuffer + nPosition, _data_count);
        nPosition += _data_count;
        if (nPosition > nMemLength)
            nPosition = nMemLength;
        return _str;
    }
    // 保存文件
    FINLINE bool SaveFile(const NCHAR* filename, const NCHAR* mode = _FT("wb+"))
    {
#ifdef UNICODE
        return SaveFileW(filename, mode);
#else
        return SaveFileA(filename, mode);
#endif
    }
    FINLINE bool SaveFileW(const WCHAR* filename, const WCHAR* mode = _FTW("wb+"))
    {
        return SaveFileA(FStringA(filename), FStringA(mode));
    }
    FINLINE bool SaveFileA(const ACHAR* filename, const ACHAR* mode = _FTA("wb+"))
    {
        if (!pMemBuffer || nPosition > nMemLength)
            return false;
        FILE* pFile = fopen(filename, mode);
        if (!pFile)
            return false;
        // 允许保存0字节的空文件
        fwrite(pMemBuffer, nPosition, 1, pFile);
        fclose(pFile);
        return true;
    }
    // 加载文件
    FINLINE bool LoadFile(const NCHAR* filename, const NCHAR* mode = _FT("rb"))
    {
#ifdef UNICODE
        return LoadFileW(filename, mode);
#else
        return LoadFileA(filename, mode);
#endif
    }
    FINLINE bool LoadFileW(const WCHAR* filename, const WCHAR* mode = _FTW("rb"))
    {
        return LoadFileA(FStringA(filename), FStringA(mode));
    }
    FINLINE bool LoadFileA(const ACHAR* filename, const ACHAR* mode = _FTA("rb"))
    {
        Release();

        FILE* pFile = fopen(filename, mode);
        if (!pFile)
            return false;
        INT64 file_size = 0;
        struct _stat64 buf;
        int ret = ::_stat64(filename, &buf);
        if (ret == 0)
            file_size = buf.st_size;
        if (file_size > 0 && (unsigned __int64)file_size <= (size_t)-1)
        {
            nFileSize = (size_t)file_size;
            bCheckFileSize = true;
            ReAllocate(nFileSize);
            if (!pMemBuffer || nMemLength < nFileSize ||
                fread(pMemBuffer, 1, nFileSize, pFile) != nFileSize)
            {
                fclose(pFile);
                Release();
                return false;
            }
        }
        fclose(pFile);
        nPosition = 0;
        return nFileSize > 0 && nMemLength > 0;
    }
    // 加载文件模式时的文件长度
    FINLINE size_t FileSize(){ return nFileSize; }
    // 写指定类型数据
    template < typename _Ty >
    FINLINE _Ty* Write(_Ty* data, size_t count = 1) { return (_Ty*)WriteByte(data, sizeof(_Ty), count); }
    // 读指定类型数据
    template < typename _Ty >
    FINLINE _Ty* Read(_Ty* data, size_t count = 1) { return (_Ty*)ReadByte(data, sizeof(_Ty), count); }
    // 写字符串
    FINLINE NCHAR* WriteStr(FString& str)
    {
#ifdef UNICODE
        return WriteStrW(str);
#else
        return WriteStrA(str);
#endif
    }
    FINLINE WCHAR* WriteStrW(FStringW& str)
    {
        WORD size = (WORD)(sizeof(WCHAR) * str.GetLength());
        if (!Write(&size))
            return 0;
        return (WCHAR*)WriteByte(str.GetString(), size, 1);
    }
    FINLINE ACHAR* WriteStrA(FStringA& str)
    {
        WORD size = (WORD)(sizeof(ACHAR) * str.GetLength());
        if (!Write(&size))
            return 0;
        return (ACHAR*)WriteByte(str.GetString(), size, 1);
    }
    // 读字符串
    FINLINE const NCHAR* ReadStr(FString& str)
    {
#ifdef UNICODE
        return ReadStrW(str);
#else
        return ReadStrA(str);
#endif
    }
    FINLINE const WCHAR* ReadStrW(FStringW& str)
    {
        str.Empty();
        WORD size;
        if (!Read(&size))
            return 0;
        if (bCheckFileSize)
        {
            if (nPosition > nFileSize || size > nFileSize - nPosition)
                return 0;
        }
        else
        {
            if (nPosition > nMemLength || size > nMemLength - nPosition)
                return 0;
        }
        if (size % sizeof(WCHAR) != 0)
            return 0;
        str.Append((WCHAR*)(pMemBuffer + nPosition), size / sizeof(WCHAR));
        nPosition += size;
        return str.GetString();
    }
    FINLINE const ACHAR* ReadStrA(FStringA& str)
    {
        str.Empty();
        WORD size;
        if (!Read(&size))
            return 0;
        if (bCheckFileSize)
        {
            if (nPosition > nFileSize || size > nFileSize - nPosition)
                return 0;
        }
        else
        {
            if (nPosition > nMemLength || size > nMemLength - nPosition)
                return 0;
        }
        str.Append((ACHAR*)(pMemBuffer + nPosition), size / sizeof(ACHAR));
        nPosition += size;
        return str.GetString();
    }

protected:
    BYTE* pMemBuffer;       // 当前缓冲区地址
    size_t nMemLength;      // 当前缓冲区内存长度
    size_t nPosition;       // 当前操作游标
    size_t nFileSize;       // 加载文件的时候文件长度
    bool bCheckFileSize;    // 在读取数据时需要检查文件长度
    bool bAutoDelete;       // 是否需要释放内存数据
};
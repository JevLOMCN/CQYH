#ifndef _PAK_FILE_H_
#define _PAK_FILE_H_

#include <array>
#include <string>
#include <vector>

#include "file_owner.h"
#include "pak_def.h"

// The on-disk structures and filename hashes retain the original 32-bit PAK format.
class CPakFile
{
public:
    CPakFile() = default;
    virtual ~CPakFile();
    CPakFile(const CPakFile&) = delete;
    CPakFile& operator=(const CPakFile&) = delete;

    BOOL Create(const char* file_name);
    BOOL Open(const char* file_name);
    BOOL Close();
    BOOL IsOpened();
    BOOL SetPackageFlag(BOOL hash_table, BOOL names, BOOL compress_names, BOOL compress_info);
    BOOL AddFile(const char* package_name, const char* disk_name);
    BOOL DelFile(const char* package_name);
    int FindFile(const char* package_name);
    BOOL AddFileFromPak(const char* package_name, CPakFile* source, int index);
    BOOL AddFileFromData(const char* package_name, time_t created, time_t modified,
        const char* data, size_t size);
    int GetFileCount();
    size_t GetFileSize(int index);
    const char* GetFileName(int index);
    BOOL GetFileHashValue(int index, unsigned long& hash_a, unsigned long& hash_b);
    BOOL IsDeletedFile(int index);
    BOOL GetFileMD5Value(int index, unsigned char digest[16]);
    BOOL ReadFileData(int index, char* data, size_t size);
    BOOL VerifyFile(int index);
    BOOL ExtractFile(int index);

private:
    struct Entry
    {
        PAK_FILE_INFO info{};
        std::string name;
        std::vector<PAK_CHUNK_INFO> chunks;
    };

    cqyh::CFilePtr m_file;
    PAK_FILE_HEADER m_header{};
    std::vector<Entry> m_entries;
    std::vector<UINT32> m_hash_table;
    std::string m_fallback_name;
    bool m_open = false;
    bool m_dirty = false;
    bool m_hash_dirty = false;
    bool m_write_failed = false;

    void Reset() noexcept;
    void UpdateHashTable();
    Entry& At(int index);
    BOOL AddRaw(const char* package_name, const Entry& source, const std::vector<char>& bytes);
};

#endif // _PAK_FILE_H_

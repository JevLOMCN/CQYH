#ifndef _TAB_FILE_H_
#define _TAB_FILE_H_

#include <string>
#include <map>
#include <functional>
#include <optional>
#include <string_view>
#include <memory>
#include <vector>

/************************************************************************/
/* The strings output from this class are always UTF-8 format!!!		*/
/************************************************************************/

class CTabFile
{
public:
    CTabFile() = default;
    ~CTabFile() = default;
    CTabFile(const CTabFile&) = delete;
    CTabFile& operator=(const CTabFile&) = delete;

	BOOL Init(const char* pcszFileName, INT32 nSkipLine);
	BOOL UnInit(void);

	INT32 GetRowCount(void);
	INT32 GetColCount(void);
    const char* GetColName(INT32 nCol);

    BOOL ReadData(INT32 nCol, INT32 nRow, INT32& rnValue);
	BOOL ReadData(const char* pcszColName, int nRow, INT32& rnValue);
    BOOL ReadData(std::string_view columnName, INT32 nRow, INT32& rnValue);
    BOOL ReadData(INT32 nCol, INT32 nRow, char* pszValue, INT32 nSize);
    BOOL ReadData(const char* pcszColName, int nRow, char* pszValue, INT32 nSize);
    BOOL ReadData(std::string_view columnName, INT32 nRow, char* pszValue, INT32 nSize);

    UINT32 GetCRC(void);

private:
    const char* GetData(INT32 nCol, INT32 nRow) const;
    const char* GetValueData(INT32 nCol, INT32 nRow) const;
    std::optional<INT32> FindColumn(std::string_view name) const;

private:
    std::unique_ptr<char[]> m_pData;
    size_t m_uSize = 0;
    INT32 m_nSkipLine = 0;
    INT32 m_nRowCount = 0;
    INT32 m_nColCount = 0;
    std::vector<INT32> m_pOffsetTable;
    UINT32 m_dwCRC32 = 0;

	using MAP_NAME_2_COL = std::map<std::string, int, std::less<>>;
	MAP_NAME_2_COL	m_mapName2Col;
};

#endif	// _TAB_FILE_H_

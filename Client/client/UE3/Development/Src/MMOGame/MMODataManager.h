#ifndef __MMODATAMANAGER_H__
#define __MMODATAMANAGER_H__

#include "MMOSingleton.h"
#include "MMOUtil.h"
#include "DataErrorCheck.h"
#include "MMOFacade.h"
#include "tabfile.h"
//#include "read_data_helper.h"

#define M_HEADER_ROW  0
#define M_DEFAULT_VALUE_ROW  2
#define M_SKIP_ROWS 3
#define M_INVALID_DATA_IDX (-1)

// Pass DataProcessor as the second template parameter when implementing the following interface declarations
/*
template<typename T>
struct TStructDataProcessor
{
public:

	static T* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		return new T();
	}

	static void DestroyDataObject(T* obj)
	{
		delete obj;
	}

	static void ParseData(T* obj, class CTabFile* dataRow, int rowIdx)
	{
		obj->SomeIntVar = TReadDataValue_Int<MMOItemManager>(TEXT("headerName"), dataRow, rowIdx);
	}

	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(T* obj)
	{
		return obj->GetFullName();
	}

	static void PostInitData()
	{

	}
};
*/

struct FindDataIndexResult
{
	bool FoundResult;
	INT Index;

	FindDataIndexResult(bool r, INT idx)
		: FoundResult(r)
		, Index(idx)
	{
	}
};

// Count all columns read from tables
#if !SHIPPING_PC_GAME

void StatTabHeader(const FString& tabFileName, const FString& headerName, const FString& headerUsage);
void _DumpUsedTabHeaders();

#define M_DumpUsedTabHeaders _DumpUsedTabHeaders
#define M_StatUsedTabHeader StatTabHeader
#else
#define M_StatUsedTabHeader(...)
#define M_DumpUsedTabHeaders(...)
#endif


//------------------------------------------------------------------------------------------------

template<typename ManagerType>
FString TReadDataValue_FString(const FString& headerName, class CTabFile* dataRow, int rowIdx)
{
	M_StatUsedTabHeader(ManagerType::GetTabfileName(),  headerName, ManagerType::getSingleton().FindHeaderUsage(headerName));

	FindDataIndexResult r = ManagerType::getSingleton().FindHeaderIndex(headerName);
	if(r.FoundResult)
	{
		const TCHAR* dat = dataRow->GetData(r.Index, rowIdx);
		if(dat)
		{
			return FString(dat);
		}
		else
		{
			const TCHAR* defaultVal = ManagerType::getSingleton().FindDefaultValue(r.Index);
			return defaultVal;
		}
	}
	else
	{
		M_AddDataError(ManagerType::GetTabfileName(), FString::Printf(TEXT("Table header: %s not found!"), *headerName));
		return headerName;
	}
}

//------------------------------------------------------------------------------------------------

template<typename ManagerType>
INT TReadDataValue_Int(const FString& headerName, class CTabFile* dataRow, int rowIdx)
{
	M_StatUsedTabHeader(ManagerType::GetTabfileName(),  headerName, ManagerType::getSingleton().FindHeaderUsage(headerName));

	FindDataIndexResult r = ManagerType::getSingleton().FindHeaderIndex(headerName);
	if(r.FoundResult)
	{
		const TCHAR* dat = dataRow->GetData(r.Index, rowIdx);
		if(dat)
		{
			return appAtoi(dat);
		}
		else
		{
			FString defaultVal = ManagerType::getSingleton().FindDefaultValue(r.Index);
			return appAtoi(*defaultVal);
		}
	}
	else
	{
		M_AddDataError(ManagerType::GetTabfileName(), FString::Printf(TEXT("Table header: %s not found!"), *headerName));
		return 0;
	}
}

//------------------------------------------------------------------------------------------------

template<typename ManagerType>
BYTE TReadDataValue_ServerEnum(const FString& headerName, class CTabFile* dataRow, int rowIdx, const TCHAR* clientEnumName)
{
	M_StatUsedTabHeader(ManagerType::GetTabfileName(),  headerName, ManagerType::getSingleton().FindHeaderUsage(headerName));

	FindDataIndexResult r = ManagerType::getSingleton().FindHeaderIndex(headerName);
	if(r.FoundResult)
	{
		UEnum* Enum = FindObject<UEnum>(UServerEnum::StaticClass(), clientEnumName, TRUE);
		if(!Enum)
		{
			M_AddDataError(ManagerType::GetTabfileName(), FString::Printf(TEXT("Server enum type not found: %s"), clientEnumName));
			return 0;
		}

		const TCHAR* dat = dataRow->GetData(r.Index, rowIdx);
		if(dat)
		{
			FString enumStr(dat);
			enumStr = MMOUI::ConvertServerEnumToClientEnum(enumStr);
			BYTE retVal = (BYTE)Enum->FindEnumIndex(FName(*enumStr));

			if(INDEX_NONE == retVal)
			{
				M_AddDataError(ManagerType::GetTabfileName(), FString::Printf(TEXT("Failed to read server enum value %s : %s"), clientEnumName, dat));
				return 0;
			}

			return retVal;

		}
		else
		{
			FString defaultVal = ManagerType::getSingleton().FindDefaultValue(r.Index);
			FString enumStr = MMOUI::ConvertServerEnumToClientEnum(defaultVal);
			BYTE retVal = (BYTE)Enum->FindEnumIndex(FName(*enumStr));

			if(INDEX_NONE == retVal)
			{
				M_AddDataError(ManagerType::GetTabfileName(), FString::Printf(TEXT("Failed to read default server enum value %s : %s"), clientEnumName, *defaultVal));
				return 0;
			}

			return retVal;
		}
	}
	else
	{
		M_AddDataError(ManagerType::GetTabfileName(), FString::Printf(TEXT("Server enum type header not found: %s"), *headerName));
		return 0;
	}
}

//------------------------------------------------------------------------------------------------

template<typename ManagerType>
FLOAT TReadDataValue_Float(const FString& headerName, class CTabFile* dataRow, int rowIdx)
{
	M_StatUsedTabHeader(ManagerType::GetTabfileName(),  headerName, ManagerType::getSingleton().FindHeaderUsage(headerName));

	FindDataIndexResult r = ManagerType::getSingleton().FindHeaderIndex(headerName);
	if(r.FoundResult)
	{
		const TCHAR* dat = dataRow->GetData(r.Index, rowIdx);
		if(dat)
		{
			return appAtof(dat);
		}
		else
		{
			const TCHAR* defaultVal = ManagerType::getSingleton().FindDefaultValue(r.Index);
			return appAtof(defaultVal);
		}
	}
	else
	{
		M_AddDataError(ManagerType::GetTabfileName(), FString::Printf(TEXT("Table header: %s not found!"), *headerName));
		return 0;
	}
}

//------------------------------------------------------------------------------------------------
template<typename T>
T* LoadUEObject(const FString& inObjPath, const FString& cfgFile)
{
	FString objPath = inObjPath;
	int idx = objPath.InStr(TEXT(" "));
	if(idx != INDEX_NONE)
	{
		objPath = objPath.RightChop(idx + 1);
	}

	T* obj = Cast<T>(UObject::StaticLoadObject(
		T::StaticClass(), NULL, *objPath, NULL, LOAD_Quiet|LOAD_NoWarn, NULL));

	if(!obj)
	{
		M_AddDataError(cfgFile, FString::Printf(TEXT("Failed to load data object: %s. The resource may have been deleted improperly or renamed"), *objPath));
		return 0;
	}

	obj->AddToRoot();
	return obj;
}

#if 0 // don't load object while reading config file
template<typename ObjectType, typename ManagerType>
ObjectType* TReadDataValue_Object(const FString& headerName, class CTabFile* dataRow, int rowIdx/*, class UClass* classType*/)
{
	FindDataIndexResult r = ManagerType::getSingleton().FindHeaderIndex(headerName);
	if(r.FoundResult)
	{

		const TCHAR* dat = dataRow->GetData(r.Index, rowIdx);
		if(dat)
		{
			FString objPath(dat);
			int idx = objPath.InStr(TEXT(" "));
			if(idx != INDEX_NONE)
			{
				objPath = objPath.RightChop(idx + 1);
			}

			UObject* objectValue = UObject::StaticLoadObject(
				ObjectType::StaticClass(), NULL, *objPath, NULL, LOAD_Quiet|LOAD_NoWarn, NULL);

			if(!objectValue)
			{
				M_AddDataError(ManagerType::GetTabfileName(), FString::Printf(TEXT("Failed to load data object: %s. The resource may have been deleted improperly or renamed"), *objPath));
				return 0;
			}

			return Cast<ObjectType>(objectValue);

		}
		else
		{
			return 0;
		}
	}
	else
	{
		M_AddDataError(ManagerType::GetTabfileName(), FString::Printf(TEXT("Table header: %s not found!"), *headerName));
		return 0;
	}
}
#endif

//------------------------------------------------------------------------------------------------
#if WITH_EDITOR
template<typename ObjectType, typename ManagerType, typename TemplateObjectFinder>
ObjectType* TReadDataValue_TemplateIDObject(const FString& headerName, class CTabFile* dataRow, int rowIdx)
{
	M_StatUsedTabHeader(ManagerType::GetTabfileName(),  headerName, ManagerType::getSingleton().FindHeaderUsage(headerName));

	FindDataIndexResult r = ManagerType::getSingleton().FindHeaderIndex(headerName);
	if(r.FoundResult)
	{	
		const TCHAR* dat = dataRow->GetData(r.Index, rowIdx);
		if(dat)
		{
			INT templateID = appAtoi(dat);
			return TemplateObjectFinder::FindTemplateObject(templateID);
		}
		return 0;
	}
	else
	{
		M_AddDataError(ManagerType::GetTabfileName(), FString::Printf(TEXT("Table header: %s not found!"), *headerName));
		return 0;
	}
}
#endif

//------------------------------------------------------------------------------------------------

template<typename DataType, typename DataProcessor, typename KeyType = INT>
class TDataManger : public SingletonMeyers< typename TDataManger<DataType, DataProcessor, KeyType> >
{
public:

	friend typename DataProcessor;
	
	typename typedef std::map<KeyType, DataType*, M_UE3_STL_ALLOC_MAP(KeyType, DataType*) > DataTypeMap;
	typedef std::map<FString, INT, ComparFString, STLAllocator< std::pair<FString, INT>, UE3STLAllocatorPolicy> > HeaderIndexMap;

protected:
	DataTypeMap _dataMap;
	class CTabFile* _tabFile;
	HeaderIndexMap _headerIndexMap;
	TArray<FString> _defaultValues;


	void ClearTabFile()
	{
		if(_tabFile)
		{
			delete _tabFile;
			_tabFile = NULL;
		}
	}
	
public:

	static const TCHAR* GetTabfileName() { return DataProcessor::GetName(); }

	TDataManger()
	: _tabFile(0)
	{
	}

	virtual ~TDataManger()
	{
		ClearTabFile();
	}
		
#if WITH_EDITOR
	void CollectReferencedResource(TSet<FString>& outResList);
#endif

	void AddData(KeyType id, DataType* obj);

	void Clear();
	void InitFromFile(const FString& fileName, bool needEmptyStr = false, bool deleQuotate = false);
	void InitFromData(class CTabFile* tabFile);
	FindDataIndexResult FindHeaderIndex(const FString& headerName) const;
	const DataType* FindData(KeyType id) const;
	DataType* FindAndModifyData(KeyType id);
	const TCHAR* FindDefaultValue(int idx) const;
	const TCHAR* FindDefaultValue(const FString& headerName) const;
	const TCHAR* FindHeaderUsage(const FString& headerName) const;
	const DataTypeMap& GetDataMap() const { return _dataMap; }
	DataTypeMap& GetDataMap() { return _dataMap; }

};
//------------------------------------------------------------------------------------------------
#if WITH_EDITOR
extern void _CollectReferencedResource(UObject* obj, TSet<FString>& outResList);

template<typename DataType, typename DataProcessor, typename KeyType >
void TDataManger<DataType, DataProcessor, KeyType>::CollectReferencedResource( TSet<FString>& outResList )
{
	DataTypeMap::const_iterator it = _dataMap.begin();
	for (; it != _dataMap.end(); ++it)
	{
		DataProcessor::CollectReferencedResourceFromDataTable(it->second, outResList);
	}
}
#endif

template<typename DataType, typename DataProcessor, typename KeyType >
void TDataManger<DataType, DataProcessor, KeyType>::InitFromFile( const FString& fileName, bool needEmptyStr, bool deleQuotate )
{
	M_BeginLoadData(fileName);

	ClearTabFile();
	
	MMOFacade::ReadConfigString(_tabFile, *fileName, needEmptyStr, deleQuotate);
	InitFromData(_tabFile);
}
//------------------------------------------------------------------------------------------------
template<typename DataType, typename DataProcessor, typename KeyType >
void TDataManger<DataType, DataProcessor, KeyType>::Clear()
{
	DataTypeMap::iterator it = _dataMap.begin();
	for (; it != _dataMap.end(); ++it)
	{
		DataProcessor::DestroyDataObject(it->second);
	}

	_dataMap.clear();
	_headerIndexMap.clear();
	_defaultValues.Empty();
}
//------------------------------------------------------------------------------------------------
template<typename DataType, typename DataProcessor, typename KeyType >
const TCHAR* TDataManger<DataType, DataProcessor, KeyType>::FindDefaultValue(int idx) const
{
	if(!GIsEditor)
	{
		if(idx < _defaultValues.Num())
		{
			return *_defaultValues(idx);
		}


// 		if(idx < _tabFile->GetColCount())
// 		{
// 			return _tabFile->GetData(idx, M_DEFAULT_VALUE_ROW);
// 		}

		M_AddDataError(GetTabfileName(), FString::Printf(TEXT("Cannot find default value column: %d max columns: %d"), idx, _defaultValues.Num()));
	}

	return TEXT("");
}
//------------------------------------------------------------------------------------------------
template<typename DataType, typename DataProcessor, typename KeyType >
const TCHAR* TDataManger<DataType, DataProcessor, KeyType>::FindDefaultValue(const FString& headerName) const
{
	FindDataIndexResult r = FindHeaderIndex(headerName);
	if(!r.FoundResult)
	{
#if !SHIPPING_PC_GAME
		M_AddDataError(GetTabfileName(), FString::Printf(TEXT("Default value not found, header: %s"), *headerName));
#endif
		return TEXT("");
	}

	return FindDefaultValue(r.Index);
}
//------------------------------------------------------------------------------------------------
template<typename DataType, typename DataProcessor, typename KeyType >
const TCHAR* TDataManger<DataType, DataProcessor, KeyType>::FindHeaderUsage(const FString& headerName) const
{
	FindDataIndexResult r = FindHeaderIndex(headerName);
	if(!r.FoundResult)
	{
#if !SHIPPING_PC_GAME
		M_AddDataError(GetTabfileName(), FString::Printf(TEXT("Header Usage not found, header: %s"), *headerName));
#endif
		return TEXT("");
	}

	if(_tabFile)
	{
		return _tabFile->GetData(r.Index, M_DEFAULT_VALUE_ROW - 1);
	}

	return TEXT("");
}
//------------------------------------------------------------------------------------------------
template<typename DataType, typename DataProcessor, typename KeyType >
void TDataManger<DataType, DataProcessor, KeyType>::InitFromData(class CTabFile* tabFile)
{
	if(_tabFile != tabFile)
	{
		ClearTabFile();
	}

	Clear();

	if(tabFile && (tabFile->GetRowCount() > M_DEFAULT_VALUE_ROW))
	{
		//const TArray<FString>& headers = tabData(M_HEADER_ROW);
		for (INT i = 0; i < tabFile->GetColCount(); ++i)
		{
			_headerIndexMap.insert(std::make_pair(tabFile->GetData(i, M_HEADER_ROW), i));
			_defaultValues.AddItem(tabFile->GetData(i, M_DEFAULT_VALUE_ROW));
		}
	}


	FindDataIndexResult id = FindHeaderIndex(DataProcessor::GetTemplateIDName());
	if(!id.FoundResult)
	{
#if !SHIPPING_PC_GAME
		appMsgf(AMT_OK, TEXT("Header not found: %s config file: %s"), *DataProcessor::GetTemplateIDName(), DataProcessor::GetName());
#endif
		return;
	}

	int idIdx = id.Index;

	for (INT i = M_SKIP_ROWS; i < tabFile->GetRowCount(); ++i)
	{
		KeyType templateID = DataProcessor::ParseID(tabFile->GetData(idIdx, i));

		bool needAddToMap = true;
		DataType* obj =	DataProcessor::CreateDataObject(templateID, needAddToMap);
		DataProcessor::ParseData(obj, tabFile, i);

		if(needAddToMap)
		{
			AddData(templateID, obj);
		}
	}

	DataProcessor::PostInitData();

	ClearTabFile();
}
//------------------------------------------------------------------------------------------------
template<typename DataType, typename DataProcessor, typename KeyType >
FindDataIndexResult TDataManger<DataType, DataProcessor, KeyType>::FindHeaderIndex(const FString& headerName) const
{
	//int idx = _tabFile->GetKeyColIdx(*headerName);
// 	if(idx != -1)
// 	{
// 		return FindDataIndexResult(true, idx);
// 	}
	HeaderIndexMap::const_iterator it = _headerIndexMap.find(headerName);
	if(it != _headerIndexMap.end())
	{
		return FindDataIndexResult(true, it->second);
	}



	return FindDataIndexResult(false, M_INVALID_DATA_IDX);
}
//------------------------------------------------------------------------------------------------
template<typename DataType, typename DataProcessor, typename KeyType >
const DataType* TDataManger<DataType, DataProcessor, KeyType>::FindData(KeyType id) const
{
	DataTypeMap::const_iterator it = _dataMap.find(id);
	if(it != _dataMap.end())
	{
		return it->second;
	}

	return NULL;
}
//------------------------------------------------------------------------------------------------
template<typename DataType, typename DataProcessor, typename KeyType >
DataType* TDataManger<DataType, DataProcessor, KeyType>::FindAndModifyData(KeyType id)
{
	DataTypeMap::iterator it = _dataMap.find(id);
	if(it != _dataMap.end())
	{
		return it->second;
	}

	return NULL;
}

//------------------------------------------------------------------------------------------------
template<typename DataType, typename DataProcessor, typename KeyType >
void TDataManger<DataType, DataProcessor, KeyType>::AddData(KeyType id, DataType* obj)
{
	DataTypeMap::iterator it = _dataMap.find(id);
	if(it != _dataMap.end())
	{
		M_AddDataError(GetTabfileName(), FString::Printf(TEXT("Duplicate data id: %d, replaced [%s] with [%s]"),
			id, *DataProcessor::FormatAsString(obj), *DataProcessor::FormatAsString(it->second)));

		DataProcessor::DestroyDataObject(it->second);
		it->second = obj;
	}
	else
	{
		_dataMap.insert(std::make_pair(id, obj));
	}
}

#endif
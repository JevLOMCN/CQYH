#include "stdafx.h"

IMPLEMENT_CLASS(UAI_Affix_Table);

UBOOL UAI_Affix_Table::IsNeedArchetype()
{
	return false;
}
//------------------------------------------------------------------------
void UAI_Affix_Table::DataToExcel(TArray<FString>& Row)
{
	//Row.AddItem(NameID.ToString());
	//Row.AddItem(DisplayName);

	//Row.AddItem(FString::Printf(TEXT("%d"), Level));
	//Row.AddItem(FString::Printf(TEXT("%d"), MaxHP));
	//Row.AddItem(FString::Printf(TEXT("%d"), MaxMP));
	//Row.AddItem(FString::Printf(TEXT("%d"), Speed));
	//Row.AddItem(FString::Printf(TEXT("%d"), Radius));
	//Row.AddItem(FString::Printf(TEXT("%d"), ServerSpawnTime));
	//Row.AddItem(FString::Printf(TEXT("%d"), ReviveTime));
	//Row.AddItem(FString::Printf(TEXT("%d"), CorpseTime));
	//Row.AddItem(FString::Printf(TEXT("%d"), RespawnTime));

}

FString UAI_Affix_Table::GetTableName()
{
	return TableName;
}

INT UAI_Affix_Table::GetRowCount()
{
	return Table.Num();
}

void UAI_Affix_Table::ExportRowData(TArray<FString>& Row, INT nIndex)
{
	if(Table.IsValidIndex(nIndex))
	{
		Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).ID));
		Row.AddItem(Table(nIndex).AffixName);
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Type));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Group));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func1));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func1Param1));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func1Param2));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func1Param3));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func1Param4));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func2));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func2Param1));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func2Param2));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func2Param3));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func2Param4));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func3));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func3Param1));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func3Param2));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func3Param3));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func3Param4));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func4));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func4Param1));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func4Param2));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func4Param3));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func4Param4));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func5));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func5Param1));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func5Param2));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func5Param3));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func5Param4));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func6));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func6Param1));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func6Param2));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func6Param3));
		//Row.AddItem(FString::Printf(TEXT("%d"), Table(nIndex).Func6Param4));
	}
}

void UAI_Affix_Table::ImportRowData(const TArray<FString>& Values)
{
	if(Values.Num() < 2)
		return;
	int nID = appAtoi(*Values(0));
	int nIndex = -1;
	for(int i = 0; i < Table.Num(); ++i)
	{
		if(Table(i).ID == nID)
		{
			nIndex = i;
			break;
		}
	}
	if(nIndex >= 0)
	{
		int n = 0;
		Table(nIndex).ID			= appAtoi(*Values(n++));
		Table(nIndex).AffixName		= Values(n++);
		//Table(nIndex).Type			= appAtoi(*Values(n++));
		//Table(nIndex).Group			= appAtoi(*Values(n++));
		//Table(nIndex).Func1			= appAtoi(*Values(n++));
		//Table(nIndex).Func1Param1	= appAtoi(*Values(n++));
		//Table(nIndex).Func1Param2	= appAtoi(*Values(n++));
		//Table(nIndex).Func1Param3	= appAtoi(*Values(n++));
		//Table(nIndex).Func1Param4	= appAtoi(*Values(n++));
		//Table(nIndex).Func2			= appAtoi(*Values(n++));
		//Table(nIndex).Func2Param1	= appAtoi(*Values(n++));
		//Table(nIndex).Func2Param2	= appAtoi(*Values(n++));
		//Table(nIndex).Func2Param3	= appAtoi(*Values(n++));
		//Table(nIndex).Func2Param4	= appAtoi(*Values(n++));
		//Table(nIndex).Func3			= appAtoi(*Values(n++));
		//Table(nIndex).Func3Param1	= appAtoi(*Values(n++));
		//Table(nIndex).Func3Param2	= appAtoi(*Values(n++));
		//Table(nIndex).Func3Param3	= appAtoi(*Values(n++));
		//Table(nIndex).Func3Param4	= appAtoi(*Values(n++));
		//Table(nIndex).Func4			= appAtoi(*Values(n++));
		//Table(nIndex).Func4Param1	= appAtoi(*Values(n++));
		//Table(nIndex).Func4Param2	= appAtoi(*Values(n++));
		//Table(nIndex).Func4Param3	= appAtoi(*Values(n++));
		//Table(nIndex).Func4Param4	= appAtoi(*Values(n++));
		//Table(nIndex).Func5			= appAtoi(*Values(n++));
		//Table(nIndex).Func5Param1	= appAtoi(*Values(n++));
		//Table(nIndex).Func5Param2	= appAtoi(*Values(n++));
		//Table(nIndex).Func5Param3	= appAtoi(*Values(n++));
		//Table(nIndex).Func5Param4	= appAtoi(*Values(n++));
		//Table(nIndex).Func6			= appAtoi(*Values(n++));
		//Table(nIndex).Func6Param1	= appAtoi(*Values(n++));
		//Table(nIndex).Func6Param2	= appAtoi(*Values(n++));
		//Table(nIndex).Func6Param3	= appAtoi(*Values(n++));
		//Table(nIndex).Func6Param4	= appAtoi(*Values(n++));
	}
}
//------------------------------------------------------------------------
void UAI_Affix_Table::ExportExcelHeaders(TArray<FString>& Header,TArray<FString>& DefaultValues)
{
	Header.AddItem(TEXT("ID"));
	Header.AddItem(TEXT("Name"));
	Header.AddItem(TEXT("Type"));
	Header.AddItem(TEXT("Group"));
	Header.AddItem(TEXT("Func1"));
	Header.AddItem(TEXT("Func1Param1"));
	Header.AddItem(TEXT("Func1Param2"));
	Header.AddItem(TEXT("Func1Param3"));
	Header.AddItem(TEXT("Func1Param4"));
	Header.AddItem(TEXT("Func2"));
	Header.AddItem(TEXT("Func2Param1"));
	Header.AddItem(TEXT("Func2Param2"));
	Header.AddItem(TEXT("Func2Param3"));
	Header.AddItem(TEXT("Func2Param4"));
	Header.AddItem(TEXT("Func3"));
	Header.AddItem(TEXT("Func3Param1"));
	Header.AddItem(TEXT("Func3Param2"));
	Header.AddItem(TEXT("Func3Param3"));
	Header.AddItem(TEXT("Func3Param4"));
	Header.AddItem(TEXT("Func4"));
	Header.AddItem(TEXT("Func4Param1"));
	Header.AddItem(TEXT("Func4Param2"));
	Header.AddItem(TEXT("Func4Param3"));
	Header.AddItem(TEXT("Func4Param4"));
	Header.AddItem(TEXT("Func5"));
	Header.AddItem(TEXT("Func5Param1"));
	Header.AddItem(TEXT("Func5Param2"));
	Header.AddItem(TEXT("Func5Param3"));
	Header.AddItem(TEXT("Func5Param4"));
	Header.AddItem(TEXT("Func6"));
	Header.AddItem(TEXT("Func6Param1"));
	Header.AddItem(TEXT("Func6Param2"));
	Header.AddItem(TEXT("Func6Param3"));
	Header.AddItem(TEXT("Func6Param4"));
}
//------------------------------------------------------------------------
void UAI_Affix_Table::ImportFromExcel(const TArray<FString>& Values)
{	
}
//------------------------------------------------------------------------
int UAI_Affix_Table::GetTemplateID() const
{
	return TemplateID;
}
void UAI_Affix_Table::InitDataToExcel()
{
}
//---------------------------------------------------------------------------
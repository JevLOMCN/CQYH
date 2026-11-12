//
// NPCAIData
//

class AI_Affix_Table extends Object implements(IExcelable,IReferenceID)
	native
	hidecategories(Object)
	dependson(ServerEnum);

//enum AI_SAY_EVENT
//{
//    aseInvalid = 0,
//
//    aseSpawn,
//    aseWander,
//    aseEnterFight,
//    aseDie,
//    aseRevive,
//    aseFlee,
//
//    aseTotal
//};

struct native Ai_Affix
{
	var() int ID					<DisplayName="ID">;
	var() string AffixName			<DisplayName="AffixName">;
//	var() EAFFIX_TYPE Type			<DisplayName="Type">;
//	var() int Group					<DisplayName="Group">;
//	var() EAFFIX_APPLY_FUNC Func1	<DisplayName="Func1">;
//	var() int Func1Param1			<DisplayName="Func1Param1">;
//	var() int Func1Param2			<DisplayName="Func1Param2">;
//	var() int Func1Param3			<DisplayName="Func1Param3">;
//	var() int Func1Param4			<DisplayName="Func1Param4">;
//	var() EAFFIX_APPLY_FUNC Func2	<DisplayName="Func2">;
//	var() int Func2Param1			<DisplayName="Func2Param1">;
//	var() int Func2Param2			<DisplayName="Func2Param2">;
//	var() int Func2Param3			<DisplayName="Func2Param3">;
//	var() int Func2Param4			<DisplayName="Func2Param4">;
//	var() EAFFIX_APPLY_FUNC Func3	<DisplayName="Func3">;
//	var() int Func3Param1			<DisplayName="Func3Param1">;
//	var() int Func3Param2			<DisplayName="Func3Param2">;
//	var() int Func3Param3			<DisplayName="Func3Param3">;
//	var() int Func3Param4			<DisplayName="Func3Param4">;
//	var() EAFFIX_APPLY_FUNC Func4	<DisplayName="Func4">;
//	var() int Func4Param1			<DisplayName="Func4Param1">;
//	var() int Func4Param2			<DisplayName="Func4Param2">;
//	var() int Func4Param3			<DisplayName="Func4Param3">;
//	var() int Func4Param4			<DisplayName="Func4Param4">;
//	var() EAFFIX_APPLY_FUNC Func5	<DisplayName="Func5">;
//	var() int Func5Param1			<DisplayName="Func5Param1">;
//	var() int Func5Param2			<DisplayName="Func5Param2">;
//	var() int Func5Param3			<DisplayName="Func5Param3">;
//	var() int Func5Param4			<DisplayName="Func5Param4">;
//	var() EAFFIX_APPLY_FUNC Func6	<DisplayName="Func6">;
//	var() int Func6Param1			<DisplayName="Func6Param1">;
//	var() int Func6Param2			<DisplayName="Func6Param2">;
//	var() int Func6Param3			<DisplayName="Func6Param3">;
//	var() int Func6Param4			<DisplayName="Func6Param4">;

};

var() int TemplateID				<DisplayName="TemplateID">;
var() string TableName				<DisplayName="TableName"|ExportExcel="1">;
var() array<Ai_Affix> Table			<DisplayName="Table">; 




native function bool IsNeedArchetype();

//-------------------------------------------------------
// export to Excel
//-------------------------------------------------------
cpptext
{
}

// export table header, call the script function first, then the native function
event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues)
{
}

// export each row of data, call the script function first, then the native function
event ScriptDataToExcel(out array<string> Row)
{

}

native function ExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);
native function DataToExcel(out array<string> Row);
native function int GetRowCount();
native function ExportRowData(out array<string> Row, int nIndex);
native function ImportRowData(array<string> values);
native function string GetTableName();
native function ImportFromExcel(array<string> values);
event ScriptImportFromExcel(array<string> Values)
{
	//local string val;

	//val = values[0];
	//TemplateID = int(val);
}
native function InitDataToExcel();

native function int GetTemplateID() const;

cpptext
{	
	virtual FString GetDetailedDescription( INT InIndex )
	{
		switch( InIndex )
		{
		case 0:
			return FString::Printf(TEXT("%d"), GetTemplateID());
			break;
		}

		return TEXT("");
	}
}

DefaultProperties
{
	
}

//
// main NPC base class, handles character animation management
//

class NPC_Pattern extends Object implements(IExcelable,IReferenceID)
	native
	hidecategories(Object);

var() int TemplateID		<ExportExcel="0"|DisplayName="TemplateID">;
var() string ScriptName		<ExportExcel="1"|DisplayName="ScriptName">;
var() string PatternName	<ExportExcel="2"|DisplayName="PatternName">;
var() string Description	<ExportExcel="3"|DisplayName="Description">;


native function ExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);
native function DataToExcel(out array<string> Row);
native function ImportFromExcel(array<string> values);
native function InitDataToExcel();
event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues){}
event ScriptDataToExcel(out array<string> Row){}
event ScriptImportFromExcel(array<string> Values){}
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

//
// IExcelable interface, objects that can output Excel can implement this interface
//
interface IExcelable
	native;

//
// Important: if the script version of a function is implemented, do not implement the native version; implementing both causes data conflicts
//

// export table header, call the script function first, then the native function
event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);


// export each row of data, call the script function first, then the native function
event ScriptDataToExcel(out array<string> Row);

native function ExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);
native function DataToExcel(out array<string> Row);
native function InitDataToExcel();

// Import data from EXCEL
native function ImportFromExcel(array<string> values);
event ScriptImportFromExcel(array<string> Values);
//// Output data to Excel
//cpptext
//{
//	bool DataToExcel(TArray<FString>& Row);
//}
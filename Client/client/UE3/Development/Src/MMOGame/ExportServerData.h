#ifndef _EXPORT_SERVER_DATA_H_
#define _EXPORT_SERVER_DATA_H_

FString GetCurrentNPCPreLoadingListFile();

#if !SHIPPING_PC_GAME

void ExportBinaryServerScene();

void ConvertToNewNPCTemplate();
void FixNPCTemplateRef();
void ExtractEquipAppearence();

//
// Server scene data exporter interface
//
class IServerSceneExporter
{
public:

	virtual void PrePorcess(INT& GlobalAutoID) = 0;
	virtual void WriteHeader(FBufferArchive& ar ) = 0;
	virtual void WriteData(FBufferArchive& ar) = 0;

	// Return how many types of data each export plugin needs to export (generally one plugin handles only one class)
	virtual	int GetBlockNumber() const = 0;
};

//
// Global data processing
//
class IGlobalDataExporter
{
public:

	virtual void ProcessData(INT& GlobalAutoID) = 0;
};

typedef TArray<IServerSceneExporter*> ServerSceneExporterList;
extern ServerSceneExporterList GServerSceneExporterList;

// Extension plugins, handle some preprocessing for global export
typedef TArray<IGlobalDataExporter*> GlobalDataExporterList;;
extern GlobalDataExporterList GGlobalDataExporterList;

#define REG_SERVER_EXPORTER(ExportClass) \
	ExportClass __##ExportClass##Instance; \
	GServerSceneExporterList.AddItem(&__##ExportClass##Instance);


#define REG_GLOBAL_EXPORTEREX(ExportClass) \
	ExportClass __##ExportClass##Instance; \
	GGlobalDataExporterList.AddItem(&__##ExportClass##Instance);

#endif // #if !SHIPPING_PC_GAME

#endif

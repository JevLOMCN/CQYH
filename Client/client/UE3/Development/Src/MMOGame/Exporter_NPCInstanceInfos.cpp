#include "stdafx.h"
#include "Exporter_NPCInstanceInfos.h"
#include <sstream>
#include <fstream>
#include <ios>
#include "SourceControl.h"

#if !SHIPPING_PC_GAME

#define SetNpcHasFunc(typeInt,FuncType) ((typeInt)=((typeInt)|(1<<(INT)(FuncType))))

Exporter_NPCInstanceInfos::Exporter_NPCInstanceInfos(void)
{
}
//------------------------------------------------------------------------
Exporter_NPCInstanceInfos::~Exporter_NPCInstanceInfos(void)
{
}
//------------------------------------------------------------------------
void Exporter_NPCInstanceInfos::ProcessData(INT& GlobalAutoID)
{
	UWorld*	World = GWorld;
	//static_cast<UWorld*>( UObject::StaticFindObject(UWorld::StaticClass(), PackageToSave, TEXT("TheWorld")) );
	FString ExportDir;
	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"), ExportDir, GGameIni);
	
	ExportDir = MMO::FormatTempleteDataLoadingDir(ExportDir);

	FString mapName = World->GetWorldInfo()->GetMapName() + TEXT(".npc");
	FString mapPath = ExportDir + mapName;

	const INT C_TMPBUF_SZ = 2048;
	wchar_t tmpBuf[C_TMPBUF_SZ];

	std::wstringstream ss;

	ss << TEXT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	ss << TEXT("<root>\n");

	for(TObjectIterator<ANPCServerInstance> It; It; ++It)
	{
		ANPCServerInstance* inst = *It;
		if(inst->TemplateNew)
		{
			memset(tmpBuf, 0, sizeof(wchar_t) * C_TMPBUF_SZ);

			//NPC function attributes
			INT npcFuncType = 0;

			if( inst->TemplateNew->IsReputationFunc ){
				SetNpcHasFunc( npcFuncType, ENPCFY_ReputationNpc );
			}
			if( inst->TemplateNew->IsPVP ){
				SetNpcHasFunc( npcFuncType, ENPCFY_PVP );
			}
			if( inst->TemplateNew->IsMerchant ){
				SetNpcHasFunc( npcFuncType, ENPCFY_Merchant );
			}
			if( inst->TemplateNew->IsWarehouse ){
				SetNpcHasFunc( npcFuncType, ENPCFY_Warehouse );
			}
			if( inst->TemplateNew->IsNormalFunction ){
				SetNpcHasFunc( npcFuncType, ENPCFY_NormalFun );
			}
			//
			swprintf_s(tmpBuf, C_TMPBUF_SZ - 1,
				TEXT("\t<NPC Name=\"%s\" TemplateID=\"%d\" DisplayName=\"%s\" NpcType=\"%d\" NpcFuncType=\"%d\" Position=\"%s\"/>\n"),
				*inst->GetName(), inst->TemplateNew->TemplateID, *inst->TemplateNew->GetDisplayName(), inst->TemplateNew->ActorType,
				npcFuncType, *inst->Location.ToString());

			ss << tmpBuf;
		}
		else
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, inst, TEXT("NPC instance has invalid reference template!"), TEXT(""));
		}

	}
	ss << TEXT("</root>\n");


	SET_WARN_COLOR(COLOR_GREEN);
	warnf(TEXT("=====[Export]===== Exported npc info file:%s"), *mapPath);
	CLEAR_WARN_COLOR();

#if HAVE_SCC
	FCheckoutResult sccResult = FSourceControl::TryCheckOut(mapPath);
#endif

	std::string str(TCHAR_TO_ANSI(*mapPath));
	std::wfstream fs(str.c_str(), std::ios_base::out);
	if(fs.good())
	{
		fs << TCHAR_TO_UTF8(ss.str().c_str());
		fs.close();
	}

#if HAVE_SCC
	sccResult.TryAddToDefaultChangelist();
#endif
}
//------------------------------------------------------------------------
#endif
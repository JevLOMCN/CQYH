#include "UnrealEd.h"
#include "EngineMaterialClasses.h"
#include "EngineSequenceClasses.h"
#include "UnPropertyTag.h"
#include "EngineUIPrivateClasses.h"
#include "EnginePhysicsClasses.h"
#include "EngineParticleClasses.h"
#include "LensFlare.h"
#include "EngineAnimClasses.h"
#include "UnTerrain.h"
#include "EngineFoliageClasses.h"
#include "SpeedTree.h"
#include "UnTerrain.h"
#include "EnginePrefabClasses.h"
#include "Database.h"
#include "EngineSoundClasses.h"

#include "SourceControl.h"

#include "PackageHelperFunctions.h"
#include "PackageUtilityWorkers.h"

#include "PerfMem.h"
#include "AnimationEncodingFormat.h"
#include "AnimationUtils.h"
#include "AnimationCompression.h"

#include <map>
#include <set>
#include "../../Engine/Inc/EngineClasses.h"
#include "../../Engine/Inc/EngineMeshClasses.h"
#include "../../Engine/Inc/EngineAnimClasses.h"
#include "../../Engine/Inc/EngineAIClasses.h"
#include "../../GameFramework/Inc/GameFrameworkClasses.h"
#include "../../MMOGame/Inc/MMOGameClasses.h"
#include "ExportArchetypeToXMLUtils.h"

#include "../../MMOGame/Inc/MMOUtil.h"
#include "EditorCommandlets.h"

//------------------------------------------------------------------------------------------------
// make resource reference list for cooking
//------------------------------------------------------------------------------------------------

IMPLEMENT_CLASS(UMakeRefListCommandlet)
IMPLEMENT_CLASS(UDumpRefListCommandlet)

#if WITH_EDITOR

extern bool g_isCommandlet;
/** delay in seconds for the spinning involved in synchronizing child jobs */
extern const FLOAT MTSpinDelay; 
/** time in seconds before we decide someone has crashed or a file is permanently locked */
extern FLOAT MTTimeout; 


// Inconvenient to write in .h, all written here as member variables
bool g_isChildProcess = false;
INT g_NumChildProcesses = 8;
INT g_currentProcessIdx = -1;
INT g_currentJobIdx = 0;
INT g_batchCnt = 1;
bool g_debugSkipUPK = false;
bool g_debugSkipUDK = false;
bool g_debugSkipCfg = false;

TArray<FString> g_Tokens;
TArray<FString> g_Switches;
TArray<UCookPackagesCommandlet::FChildProcess> g_ChildProcesses;

TSet<FString> g_resourcesList;


UClass* resourceClasses[] =
{
	UMaterialExpression::StaticClass(),
	ULightMapTexture2D::StaticClass(),
	UStaticMesh::StaticClass(),
	USkeletalMesh::StaticClass(),
	UTexture::StaticClass(),
	UMaterialInterface::StaticClass(),
	UParticleSystem::StaticClass(),
	USoundNodeWave::StaticClass(),
	UApexAsset::StaticClass(),
	UAnimSequence::StaticClass(),
	USoundCue::StaticClass(),
};

bool IsResourceClass(UObject* obj)
{
	for (INT i = 0; i < ARRAY_COUNT(resourceClasses); ++i)
	{
		if(obj->IsA(resourceClasses[i]))
		{
			return true;
		}
	}

	return false;
}


INT g_resourceClassCount = ARRAY_COUNT(resourceClasses);

void AddToRefResList( UObject* obj, TSet<FString>& outResList);
void DumpRefResSizeInfo();

void _CollectReferencedResource(UObject* obj, TSet<FString>& outResList);
extern bool SkipCookClassWhenMakeRefList(UObject* Object);
void ExportResourceProperty(BYTE*Data, UProperty* ObjPro, TSet<FString>& outResList);

bool SkipUnusedProp(UProperty* prop)
{
	bool Skip = false;
	QWORD PropertyFlags = prop->GetPropertyFlags();
	if( (PropertyFlags & (CPF_EditorOnly|CPF_Transient|CPF_DuplicateTransient|CPF_ArchetypeProperty|CPF_NonTransactional|CPF_Deprecated)) != 0 )
	{
		Skip =	((PropertyFlags & CPF_Transient) != 0)
			//||	((PropertyFlags & CPF_Const) != 0 )
			||	((PropertyFlags & CPF_EditorOnly) != 0 )
			//||	((PropertyFlags & CPF_Native) != 0 )
			||	((PropertyFlags & CPF_DuplicateTransient) != 0 )
			||	((PropertyFlags & CPF_ArchetypeProperty) != 0 )
			||	((PropertyFlags & CPF_NonTransactional) != 0 )
			||	((PropertyFlags & CPF_Deprecated) != 0 )
			;
	}
	return Skip;
}

//------------------------------------------------------------------------------------------------
// ExportResourceProperty
//------------------------------------------------------------------------------------------------
void ExportResourceProperty(BYTE*Data, UProperty* ObjPro, TSet<FString>& outResList)
{	
	// 	if(ObjPro->GetNameCPP() == TEXT("LODModels"))
	// 	{
	// 		int i = 0;
	// 		++i;
	// 	}

	if(SkipUnusedProp(ObjPro))
	{
		return;
	}

	UPropertyValue pv;
	ObjPro->GetPropertyValue(Data, pv);

	if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UInterfaceProperty )
	{		
		_CollectReferencedResource(pv.InterfaceValue->GetObject(), outResList);
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UArrayProperty )
	{
		UArrayProperty* ArrayProp = (UArrayProperty*)ObjPro;
		for (INT ArrayIndex = 0; ArrayIndex < pv.ArrayValue->Num(); ArrayIndex++)
		{
			ExportResourceProperty(((BYTE*)pv.ArrayValue->GetData()) + ArrayIndex*ArrayProp->Inner->ElementSize,
				ArrayProp->Inner, outResList);
		}
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStructProperty )
	{
		if(((UStructProperty*)ObjPro)->Struct == NULL)
		{
			return;
		}

		UProperty* NextProperty = ((UStructProperty*)ObjPro)->Struct->PropertyLink;
		for (; NextProperty != NULL; NextProperty = NextProperty->PropertyLinkNext)
		{
			for(INT i = 0; i < NextProperty->ArrayDim; ++i)
			{
				ExportResourceProperty(Data+NextProperty->Offset + NextProperty->ElementSize*i, NextProperty, outResList);
			}
		}
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UClassProperty )
	{		
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UObjectProperty )
	{
		UObject* obj = pv.ObjectValue;
		if(!obj)
		{
			return;
		}

		if(IsResourceClass(obj))
		{
			AddToRefResList(obj, outResList);
		}

		_CollectReferencedResource(obj, outResList);
	}
}

//------------------------------------------------------------------------------------------------
// _CollectReferencedResource
//------------------------------------------------------------------------------------------------
typedef std::set<UObject*> ObjectSet;
ObjectSet g_objProcessed;

void ClearProcessecObjectList()
{
	g_objProcessed.clear();
}

void _CollectReferencedResource(UObject* obj, TSet<FString>& outResList)
{
	if(!obj)
	{
		return;
	}

	if(obj == obj->GetClass()->GetDefaultObject())
	{
		return;
	}

	if(g_objProcessed.find(obj) != g_objProcessed.end())
	{
		return;
	}

	g_objProcessed.insert(obj);

	if (SkipCookClassWhenMakeRefList(obj))
	{
		return;
	}

	obj->CollectReferencedResource(outResList);

	if(IsResourceClass(obj))
	{
		AddToRefResList(obj, outResList);
	}

	for( TFieldIterator<UField> It(obj->GetClass()); It; ++It )
	{
		UField* PreviewObj = *It;

		if (PreviewObj)
		{
			UProperty* ObjPro = Cast<UProperty>(PreviewObj);
			if (ObjPro)
			{
				BYTE* Data = (BYTE*)obj + ObjPro->Offset;
				ExportResourceProperty(Data, ObjPro, outResList);
			}
		}
	}
}

#endif WITH_EDITOR


#if 1
#	define M_LOG_RES_FROM(fmtStr)  tmpRes.Empty();
#else
#	define M_LOG_RES_FROM(fmtStr) \
{ \
	TSet<FString>::TIterator it(tmpRes); \
	for (; it; ++it) \
{ \
	FString k = *it; \
	warnf(TEXT(fmtStr), *k); \
} \
	tmpRes.Empty();\
}
#endif

#if WITH_EDITOR

FString UMakeRefListCommandlet::GetChildProcessWorkDir(INT processIdx) const
{
	FString ChildString = appGameLogDir() * FString::Printf(TEXT("ResRefCollectChildProcess_%x"), processIdx);
	return ChildString;
}

FString UMakeRefListCommandlet::GetCurrentChildProcessWorkDir() const
{
	return GetChildProcessWorkDir(g_currentProcessIdx);
}

void UMakeRefListCommandlet::StartChildren(INT NumJobs)
{
	FString Args = appCmdLine();
	Args += FString(" -MTCHILD");
	TCHAR ExeName[MAX_FILEPATH_LENGTH];
	GetModuleFileName(NULL, ExeName, MAX_FILEPATH_LENGTH - 1);
	for (INT ProcessIndex = 0; ProcessIndex < NumJobs; ProcessIndex++)
	{
		UCookPackagesCommandlet::FChildProcess ProcessInfo;
		ProcessInfo.LogFilename = FFilename(appBaseDir()) * appGameLogDir() * FString::Printf(TEXT("Child%dResRef.log"), ProcessIndex);

#ifdef _DEBUG
		FString Command = FString::Printf(TEXT("MakeRefList %s -ProcIdx=%d -nopause -ABSLOG=\"%s\""), *Args, ProcessIndex, *ProcessInfo.LogFilename);
#else
		FString Command = FString::Printf(TEXT("MakeRefList %s -ProcIdx=%d -nopause -unattended -ABSLOG=\"%s\""), *Args, ProcessIndex, *ProcessInfo.LogFilename);
#endif

		debugf(TEXT("Job[%d] Launching with Cmd %s"),ProcessIndex, *Command);

		ProcessInfo.ProcessId = 0;
		ProcessInfo.ProcessHandle = appCreateProc(ExeName,*Command,FALSE,TRUE,TRUE,&ProcessInfo.ProcessId,-1);
		if (!ProcessInfo.ProcessHandle || !ProcessInfo.ProcessId)
		{
			appErrorf(TEXT("Failed to start process %s %s"),ExeName,*Command);
		}
		else
		{
			warnf(NAME_Log,TEXT("Job[%d] Launched"),ProcessIndex);
		}

		//FString ChildString = FString::Printf(TEXT("Process_%x"), ProcessInfo.ProcessId);
		FString ChildString = GetChildProcessWorkDir(ProcessIndex);
		ProcessInfo.Directory = FFilename(appGameLogDir()) * ChildString;
		debugf(TEXT("  Directory %s"), *ProcessInfo.Directory);
		ProcessInfo.CommandFile = ProcessInfo.Directory * TEXT("ResRefCmd.txt");
		ProcessInfo.StartTime = 0.0;
		g_ChildProcesses.AddItem(ProcessInfo);
	}
}

/**
 * Determine if a given child process is idle or not
 *  Only used if bIsMTMaster is TRUE. 
 *
 * @param Job String representing the filename of the file to cook on the child process
 * @return TRUE if the given child process is idle
 */
UBOOL UMakeRefListCommandlet::ChildIsIdle(INT ProcessIndex)
{
	UBOOL bIdle = !g_ChildProcesses(ProcessIndex).CommandFile.NonLockingFileExists();
	if (bIdle && g_ChildProcesses(ProcessIndex).StartTime > 0.0)
	{
		warnf(NAME_Log,TEXT("Job[%d] %s done in %5.1fs"),
			ProcessIndex,
			*g_ChildProcesses(ProcessIndex).LastCommand,
			FLOAT(appSeconds() - g_ChildProcesses(ProcessIndex).StartTime));
		g_ChildProcesses(ProcessIndex).StartTime = 0.0;
		g_ChildProcesses(ProcessIndex).JobsCompleted++;
	}
	return bIdle;
}


/**
 * Waits until a child process is available, then gives it the given job
 *  Only used if bIsMTMaster is TRUE. 
 *
 */
void UMakeRefListCommandlet::StartChildJob(const TArray<FString>& Job)
{
	for(;;)
	{
		UBOOL Sent = FALSE;
		static INT LastSent = -1;
		for (INT ProcessIndex = LastSent + 1; ProcessIndex < g_ChildProcesses.Num(); ProcessIndex++)
		{
			if (ChildIsIdle(ProcessIndex))
			{
				SendChildCommand(ProcessIndex, Job);
				LastSent = ProcessIndex;
				Sent = TRUE;
				break;
			}
		}

		if (!Sent)
		{
			for (INT ProcessIndex = 0; ProcessIndex <= LastSent && ProcessIndex < g_ChildProcesses.Num(); ProcessIndex++)
			{
				if (ChildIsIdle(ProcessIndex))
				{
					SendChildCommand(ProcessIndex,Job);
					LastSent = ProcessIndex;
					Sent = TRUE;
					break;
				}
			}
		}

		if (Sent)
		{
			break;
		}

		UBOOL Sync = FALSE;
// 		if (g_ChildProcesses.Num() > 6)
// 		{
// 			static INT LastSync = -1;
// 
// 			INT NumSync = 0;
// 			INT PrevLastSync = LastSync;
// 			const INT ToDoPerTurn = 2;
// 
// 			for (INT ProcessIndex = PrevLastSync + 1; ProcessIndex < g_ChildProcesses.Num() && NumSync < ToDoPerTurn; ProcessIndex++)
// 			{
// 				LastSync = ProcessIndex;
// 				if (CheckForTFCSync(ProcessIndex))
// 				{
// 					Sync = TRUE;
// 					NumSync++;
// 				}
// 			}
// 			if (NumSync < ToDoPerTurn)
// 			{
// 				for (INT ProcessIndex = 0; ProcessIndex <= PrevLastSync && ProcessIndex < g_ChildProcesses.Num() && NumSync < ToDoPerTurn; ProcessIndex++)
// 				{
// 					LastSync = ProcessIndex;
// 					if (CheckForTFCSync(ProcessIndex))
// 					{
// 						Sync = TRUE;
// 						NumSync++;
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
// // 			for (INT ProcessIndex = 0; ProcessIndex < g_ChildProcesses.Num(); ProcessIndex++)
// // 			{
// // 				if (CheckForTFCSync(ProcessIndex))
// // 				{
// // 					Sync = TRUE;
// // 				}
// // 			}
// 		}

		if (!Sync)
		{
			appSleep(MTSpinDelay);
		}

		CheckForCrashedChildren();
	}
}

/**
 * Called by a child process to signal is has completed a job (or the stop command)
 *  Only used if bIsMTChild is TRUE. 
 *
 */
void UMakeRefListCommandlet::JobCompleted()
{
	FFilename CommandFile(GetCurrentChildProcessWorkDir() * TEXT("ResRefCmd.txt"));

#if _DEBUG
	warnf(NAME_Warning, TEXT("---- JobCompleted:%s"), *CommandFile);
#endif

	WaitToDeleteFile(*CommandFile);
}

/**
 * On a heavily loaded machine, sometimes, files don't want to delete right away and we need to 
 * make sure they are deleted for integrity, so this routine calls GFilemanger->Delete until it 
 * succeeds or MTTimeout time is spent...if the timeout expires, we call appError
 * During this time, it calls CheckForCrashedChildren repeatedly if we are the master process
 *
 * @param	Filename file to delete
 */
void UMakeRefListCommandlet::WaitToDeleteFile(const TCHAR *Filename)
{
	if(!GFileManager->Delete(Filename))
	{
		DOUBLE StartTime = appSeconds();
		do 
		{
			if (!g_isChildProcess)
			{
				CheckForCrashedChildren();
			}

			appSleep(MTSpinDelay);
			
			if (appSeconds() - StartTime > MTTimeout)
			{
				// do this one more time because a detected child crash is a little more graceful
				if (!g_isChildProcess)
				{
					CheckForCrashedChildren();
				}
				appErrorf(TEXT("Could not delete %s"),Filename);
			}

		} while (!GFileManager->Delete(Filename));
	}
}

/**
 * Called by a child process to wait for the next command and return it
 *  Only used if bIsMTChild is TRUE. 
 *
 * @return	The next job. Either a package name to cook or the empty filename if this is the stop command
 */
TArray<FString> UMakeRefListCommandlet::GetMyNextJob()
{
	FFilename CommandFile(GetCurrentChildProcessWorkDir() * TEXT("ResRefCmd.txt"));
	FString Command;
	DOUBLE StartTime = appSeconds();
	TArray<FString> CmdList;

#if _DEBUG
	warnf(NAME_Warning, TEXT("==== GetMyNextJob:%s"), *CommandFile);
#endif

	while (!appLoadFileToString(Command, *CommandFile))
	{
		appSleep(MTSpinDelay);
		if (appSeconds() - StartTime > MTTimeout)
		{
			appErrorf(TEXT("Waited more that 60 seconds for another job, assume parent is dead, killing myself."));
		}
	}

	if (Command.StartsWith(FString(TEXT("*stop"))))
	{
		Command.Empty();
	}
 	else
 	{
 		//LoadParentPersistentCookerData();
		Command.ParseIntoArray(&CmdList, TEXT("\n"), TRUE);
		//warnf(TEXT("Get jobs:%d"), CmdList.Num());
 	}

	return CmdList;
}

/**
 * Low level routine to send a given command string to a child
 * Caller must verify that child is idle
 *  Only used if bIsMTMaster is TRUE. 
 *
 * @param ProcessIndex	Index of child to send command to
 * @param Command		String to send to child
 */
void UMakeRefListCommandlet::SendChildCommand(INT ProcessIndex, const TArray<FString>& Command)
{
// 	if (CookStats.TFCTextureSaved - g_ChildProcesses(ProcessIndex).LastTFCTextureSaved > 10 * 1024 * 1024
// 		&& g_ChildProcesses(ProcessIndex).JobsCompleted > 0)  // must have completed at least one job, otherwise it may already need a TFC sync from startup
// 	{
// 		// we saved more than 10MB of TFC so the child should load the new PCD to avoid recooking texture
// 		SavePersistentCookerDataForChild(ProcessIndex);
// 	}
	g_ChildProcesses(ProcessIndex).StartTime = appSeconds();
	if(Command.Num() > 0)
	{
		g_ChildProcesses(ProcessIndex).LastCommand =
			FString::Printf(TEXT("%s x %d"),*FFilename(Command(0)).GetCleanFilename(), Command.Num());
	}

	RobustSaveStringToFile(Command, *g_ChildProcesses(ProcessIndex).CommandFile);
}

/**
 * Sends stop commands to all child processes that are idle but have not yet been stopped
 *  Only used if bIsMTMaster is TRUE. 
 *
 * @return	TRUE if all children have been sent stop commands
 */
UBOOL UMakeRefListCommandlet::SendStopCommands()
{
	UBOOL AllDone = TRUE;
	CheckForCrashedChildren();
	for (INT ProcessIndex = 0; ProcessIndex < g_ChildProcesses.Num(); ProcessIndex++)
	{
		if (!g_ChildProcesses(ProcessIndex).bStopped)
		{
			if (ChildIsIdle(ProcessIndex))
			{
				FString cmd(TEXT("*stop"));
				TArray<FString> cmdList;
				cmdList.AddItem(cmd);
				SendChildCommand(ProcessIndex, cmdList);
				g_ChildProcesses(ProcessIndex).bStopped = TRUE;
			}
			else
			{
				//CheckForTFCSync(ProcessIndex);
				AllDone = FALSE;
			}
		}
	}
	return AllDone;
}


/**
 * Waits until all children are stopped, merges results and cleans the directories
 *  Only used if bIsMTMaster is TRUE. 
 *
 */
void UMakeRefListCommandlet::StopChildren()
{
	while (!SendStopCommands())
	{
		appSleep(MTSpinDelay);
	}

	for (INT ProcessIndex = 0; ProcessIndex < g_ChildProcesses.Num(); ProcessIndex++)
	{
		UCookPackagesCommandlet::FChildProcess& procInfo = g_ChildProcesses(ProcessIndex);
		check(procInfo.bStopped);
		check(!procInfo.bMergedResults);

		appWaitForProc(procInfo.ProcessHandle);
		//MergeChildProducts(ProcessIndex);
		//GFileManager->DeleteDirectory(*g_ChildProcesses(ProcessIndex).Directory,FALSE,TRUE);
		FString resBinFile = GetChildProcessWorkDir(ProcessIndex) * TEXT("ResList.bin");
		warnf(TEXT("Merging list from: %s ..."), *resBinFile);
		TSet<FString> tmpList = ReadResList(resBinFile);
		g_resourcesList = g_resourcesList.Union(tmpList);

		GFileManager->DeleteDirectory(*procInfo.Directory, FALSE, TRUE);

		g_ChildProcesses(ProcessIndex).bMergedResults = TRUE;
	}
	MergeLogs();
	g_ChildProcesses.Empty();
}

/**
 * A robust version of appSaveStringToFile
 * we save to a temp file, then rename. This is important for synchronization, which is what this is used for
 *
 * @param	string to save
 * @param	Filename file to save
 */
void UMakeRefListCommandlet::RobustSaveStringToFile(const TArray<FString>& String,const TCHAR *Filename)
{
	FString TempName = Filename;
	TempName += TEXT(".temp");

	FString tmpStr;
	for (INT i = 0; i < String.Num(); ++i)
	{
		tmpStr += String(i);
		tmpStr += TEXT("\n");
	}

	UBOOL Ok = appSaveStringToFile(tmpStr,*TempName);
	if (!Ok)
	{
		appErrorf(TEXT("Could not save %s"),*TempName);
	}
	if (!GFileManager->Move( Filename, *TempName ))
	{
		appErrorf(TEXT("Could not move to %s"),*Filename);
	}
}

/**
 * Verifies that all child processes that have not been stoppped are still running, appErrors otherwise
 *  Only used if bIsMTMaster is TRUE. 
 *
 */
void UMakeRefListCommandlet::CheckForCrashedChildren()
{
	UBOOL bAnyCrashed = FALSE;
	for (INT ProcessIndex = 0; ProcessIndex < g_ChildProcesses.Num(); ProcessIndex++)
	{
		if (!g_ChildProcesses(ProcessIndex).bStopped)
		{
			if (!appIsProcRunning(g_ChildProcesses(ProcessIndex).ProcessHandle))
			{
				bAnyCrashed = TRUE;
				warnf(NAME_Log, TEXT("Job[%d] Crashed."),ProcessIndex);
			}
		}
	}

	if (bAnyCrashed)
	{
		// terminate everyone
		appSleep(10.0f); // give the autoreporter a chance to work on the crashed child

		for (INT ProcessIndex = 0; ProcessIndex < g_ChildProcesses.Num(); ProcessIndex++)
		{
			if (!g_ChildProcesses(ProcessIndex).bStopped)
			{
				g_ChildProcesses(ProcessIndex).bStopped = TRUE;
				warnf(NAME_Log, TEXT("Job[%d] Killed."),ProcessIndex);
				appTerminateProc(g_ChildProcesses(ProcessIndex).ProcessHandle);
			}
		}
		FString Reason;
		MergeLogs(&Reason);
		appErrorf(TEXT("Child process crashed: %s"),*Reason);
	}
}

/**
 * Merges the logs from all of the children into my log
 *  Only used if bIsMTMaster is TRUE. 
 *
 */
void UMakeRefListCommandlet::MergeLogs(FString *CriticalReason)
{
	if (GIsBuildMachine)
	{
		if (CriticalReason)
		{
			for (INT ProcessIndex = 0; ProcessIndex < g_ChildProcesses.Num(); ProcessIndex++)
			{
				if (g_ChildProcesses(ProcessIndex).LogFilename.FileExists())
				{
					FString Log;
					if (appLoadFileToString(Log,*g_ChildProcesses(ProcessIndex).LogFilename))
					{
						if (Log.Len())
						{
							const TCHAR *At = appStristr(&Log[0],TEXT("Critical:"));
							if (At)
							{
								const TCHAR *End = appStristr(At,TEXT("\n"));
								TCHAR Message[2048] = {0};
								INT Size = 2048;
								if (End)
								{
									Size = Min<INT>(Size,End-At);
								}
								appStrncpy(Message,At,Size);
								*CriticalReason += FString::Printf(TEXT("%d>%s\r\n"),ProcessIndex,Message);
							}
						}
					}
				}
			}
		}
		return;
	}
	for (INT ProcessIndex = 0; ProcessIndex < g_ChildProcesses.Num(); ProcessIndex++)
	{
		if (g_ChildProcesses(ProcessIndex).LogFilename.FileExists())
		{
			FString Log;
			if (appLoadFileToString(Log,*g_ChildProcesses(ProcessIndex).LogFilename))
			{
				TArray<FString> LineArray;
				Log.ParseIntoArray(&LineArray, TEXT("\n"), TRUE);
				for (INT LineIndex = 0; LineIndex < LineArray.Num(); LineIndex++)
				{
					FString Line = LineArray(LineIndex);
					Line.Replace(TEXT("\r"),TEXT(""));
					debugf(TEXT("%d>%s"),ProcessIndex,*Line);
					if (CriticalReason && Line.InStr(TEXT("Critical:"),FALSE,TRUE) != INDEX_NONE)
					{
						*CriticalReason += FString::Printf(TEXT("%d>%s\r\n"),ProcessIndex,*Line);
					}
				}
			}
		}
	}
}


TSet<FString> UMakeRefListCommandlet::ReadResList(const FString& fileName)
{
	TSet<FString> readList;
	FArchive* arr = GFileManager->CreateFileReader(*fileName);
	if(arr)
	{
		(*arr) << readList;
		delete arr;
	}
	else
	{
		warnf(NAME_Error, TEXT("ReadResList failed to open file:%s"), *fileName);
	}
	//warnf(TEXT("ResRef:%d"), resources.Num());
	return readList;
}

bool UMakeRefListCommandlet::WriteResList(const FString& fileName, const TSet<FString>& resList)
{
	FArchive* ar = GFileManager->CreateFileWriter(*fileName);
	if(ar)
	{
		(*ar) << (TSet<FString>&)resList;
		ar->Flush();
		ar->Close();
		delete ar;
		return true;
	}
	else
	{
		warnf(NAME_Error, TEXT("WriteResList failed to open file:%s"), *fileName);
		return false;
	}
}

#endif


INT UMakeRefListCommandlet::Main(const FString& Params)
{
#if WITH_EDITOR

#ifdef _WIN64
	FUntypedBulkData::g_SkipLoadBulkData = TRUE; // don't load bulkdata that we really don't need
#endif

	g_isCommandlet = true;


// #if 0
// 	FArchive* arr = GFileManager->CreateFileReader(TEXT("ResRef.bin"));
// 	(*arr) << resources;
// 	delete arr;
// 	warnf(TEXT("ResRef:%d"), resources.Num());
// 	return 0;
// #endif

	GUnrealEd = Cast<UUnrealEdEngine>(GEditor);
		
	ParseCommandLine( *Params, g_Tokens, g_Switches );

	g_isChildProcess = (g_Switches.FindItemIndex(TEXT("MTCHILD")) != INDEX_NONE);
	g_debugSkipUPK = (g_Switches.FindItemIndex(TEXT("SkipUPK")) != INDEX_NONE);
	g_debugSkipUDK = (g_Switches.FindItemIndex(TEXT("SkipUDK")) != INDEX_NONE);
	g_debugSkipCfg = (g_Switches.FindItemIndex(TEXT("SkipCfg")) != INDEX_NONE);

	Parse(appCmdLine(), TEXT("Processes="), g_NumChildProcesses);
	Parse(appCmdLine(), TEXT("BatchJobCnt="), g_batchCnt);
	

	if(g_isChildProcess)
	{
		Parse(appCmdLine(), TEXT("ProcIdx="), g_currentProcessIdx);

		warnf(NAME_Log, TEXT("====[Child Process]==== Idx:%d"), g_currentProcessIdx);
	}

	bool autoMaps = false;
	if (ParseParam(appCmdLine(), TEXT("AUTOMAPLIST")))
	{
		autoMaps = true;
	}

	if(!g_isChildProcess)
	{
		TArray<FString> udkList, upkList;

		// add all config maps
		if(autoMaps)
		{
			extern void AddAutoMapList(TArray<FString>& cookPackages);
			AddAutoMapList(udkList);
		}

		// add login map
		FString cfgMap;
		GConfig->GetString(TEXT("URL"), TEXT("LocalMap"), cfgMap, GEngineIni);
		udkList.AddUniqueItem(cfgMap);

		GConfig->GetString(TEXT("URL"), TEXT("TransitionMap"), cfgMap, GEngineIni);
		udkList.AddUniqueItem(cfgMap);


		// add config maps
		TArray<FString> cfgDirs;		
		GConfig->GetArray(TEXT("Cook.DataPath"), TEXT("Path"), cfgDirs, GEditorIni);
		for(INT k = 0; k < cfgDirs.Num(); ++k)
		{
			TArray<FString> pkgs;
			appFindFilesInDirectory(pkgs, *cfgDirs(k), TRUE, FALSE);
			for (INT n = 0; n < pkgs.Num(); ++n)
			{
				FFilename fnm(pkgs(n));
				if(fnm.GetExtension() == FURL::DefaultMapExt)
				{
					//warnf(NAME_Log, TEXT("==== Adding found map file:%s"), *pkgs(n));
					udkList.AddUniqueItem(pkgs(n));
				}
				else
				{
					// No need to add UPK here, the config table will clean it up
					//upkList.AddUniqueItem(pkgs(n));
				}
			}
		}

		// add config upk to list
		TArray<FString> customPkgs;
		GConfig->GetArray(TEXT("Cook.RefDataPath"), TEXT("Pkg"), customPkgs, GEditorIni);
		upkList += customPkgs;

		//TArray<FString> cfgDirs;		
		cfgDirs.Empty();
		GConfig->GetArray(TEXT("Cook.RefDataPath"), TEXT("Path"), cfgDirs, GEditorIni);
		for(INT k = 0; k < cfgDirs.Num(); ++k)
		{
			TArray<FString> pkgs;
			appFindFilesInDirectory(pkgs, *cfgDirs(k), TRUE, FALSE);
			for (INT n = 0; n < pkgs.Num(); ++n)
			{
				FFilename fnm(pkgs(n));
				if(fnm.GetExtension() == FURL::DefaultMapExt)
				{
					//warnf(NAME_Log, TEXT("==== Adding found map file:%s"), *pkgs(n));
					udkList.AddUniqueItem(pkgs(n));
				}
				else
				{
					upkList.AddUniqueItem(pkgs(n));
				}
			}
		}

		if (!ParseParam(appCmdLine(), TEXT("NoLoadStartupPackages")))
		{
			// look for any packages that we want to force preload at startup
			FConfigSection* PackagesToPreload = GConfig->GetSectionPrivate( TEXT("Engine.StartupPackages"), 0, 1, GEngineIni );
			if (PackagesToPreload)
			{
				// go through list and add to the array
				for( FConfigSectionMap::TIterator It(*PackagesToPreload); It; ++It )
				{
					if (It.Key() == TEXT("Package"))
					{
						// add this package to the list to be fully loaded later
						upkList.AddItem(*(It.Value()));
					}
				}
			}

			PackagesToPreload = GConfig->GetSectionPrivate( TEXT("Engine.StartupPackagesMMO"), 0, 1, GEngineIni );
			if (PackagesToPreload)
			{
				// go through list and add to the array
				for( FConfigSectionMap::TIterator It(*PackagesToPreload); It; ++It )
				{
					if (It.Key() == TEXT("Package"))
					{
						// add this package to the list to be fully loaded later
						upkList.AddItem(*(It.Value()));
					}
				}
			}
		}

		//
		// start children jobs
		//

		if(g_NumChildProcesses > 0)
		{
			warnf(NAME_Log, TEXT("------- Multiprocess collect, starting %d processes for %d upks   %d udks -------"),
				g_NumChildProcesses, upkList.Num(), udkList.Num());

			StartChildren(g_NumChildProcesses);

			
			//
			// send proc command to child processes
			//
			
			DOUBLE startTime = appSeconds();

			INT i = 0;
			INT curBatchCnt = 0;
			TArray<FString> cmdList;

			if(!g_debugSkipUPK)
			{
				while (i < upkList.Num())
				{
					if(curBatchCnt < g_batchCnt)
					{
						cmdList.AddItem(upkList(i));
						++curBatchCnt;
						++i;
					}
					else
					{
						SET_WARN_COLOR(COLOR_GREEN);
						warnf(NAME_Log, TEXT("Processing UPK: [%d/%d]"), i, upkList.Num());
						CLEAR_WARN_COLOR();

						StartChildJob(cmdList);
						curBatchCnt = 0;
						cmdList.Empty();
						//--i;
					}
				}

				if(cmdList.Num())
				{
					SET_WARN_COLOR(COLOR_GREEN);
					warnf(NAME_Log, TEXT("Processing UPK: [%d/%d]"), i, upkList.Num());
					CLEAR_WARN_COLOR();

					StartChildJob(cmdList);
					curBatchCnt = 0;
					cmdList.Empty();
				}
			}

			DOUBLE endTime = appSeconds();
			startTime = endTime;
			SET_WARN_COLOR(COLOR_PURPLE);
			warnf(NAME_Log, TEXT("===== UPK All Done:%.2f"), (FLOAT)(endTime - startTime));
			CLEAR_WARN_COLOR();

			if(!g_debugSkipUDK)
			{
				i = 0;

				while (i < udkList.Num())
				{
					if(curBatchCnt < g_batchCnt)
					{
						cmdList.AddItem(udkList(i));
						++curBatchCnt;
						++i;
					}
					else
					{
						SET_WARN_COLOR(COLOR_GREEN);
						warnf(NAME_Log, TEXT("Processing UDK: [%d/%d]"), i, udkList.Num());
						CLEAR_WARN_COLOR();

						StartChildJob(cmdList);
						curBatchCnt = 0;
						cmdList.Empty();
						//--i;
					}
				}

				if(cmdList.Num())
				{
					SET_WARN_COLOR(COLOR_GREEN);
					warnf(NAME_Log, TEXT("Processing UDK: [%d/%d]"), i, udkList.Num());
					CLEAR_WARN_COLOR();

					StartChildJob(cmdList);
					curBatchCnt = 0;
					cmdList.Empty();
				}
			}

			StopChildren();

			endTime = appSeconds();
			SET_WARN_COLOR(COLOR_PURPLE);
			warnf(NAME_Log, TEXT("===== UDK All Done:%.2f"), (FLOAT)(endTime - startTime));
			CLEAR_WARN_COLOR();
// 			for (INT i = 0; i < udkList.Num(); ++i)
// 			{
// 				SET_WARN_COLOR(COLOR_GREEN);
// 				warnf(NAME_Log, TEXT("Processing UDK: [%d/%d]"), i, udkList.Num());
// 				CLEAR_WARN_COLOR();
// 
// 				TArray<FString> cmdList;
// 				cmdList.AddItem(udkList(i));
// 				StartChildJob(cmdList);
// 			}

		}
		else
		{
			warnf(NAME_Log, TEXT("------- Singlethreaded collect, starting %d upks   %d udks -------"),
				upkList.Num(), udkList.Num());

			ProcessPkgs(upkList, udkList, g_resourcesList);
		}
	}


	if(g_isChildProcess)
	{
#ifdef _DEBUG
		appMsgf(AMT_OK, TEXT("Break on child:%d"), g_currentProcessIdx);
#endif
		TSet<FString> childResList;
		for(;;)
		{
			TArray<FString> nextJobName = GetMyNextJob();
			if(nextJobName.Num())
			{
				TArray<FString> udkList, upkList;
				for (INT jobCnt = 0; jobCnt < nextJobName.Num(); ++jobCnt)
				{
					FFilename jobName = nextJobName(jobCnt);
					if(jobName.GetExtension() == TEXT("UDK"))
					{
						udkList.AddItem(jobName);
					}
					else
					{
						upkList.AddItem(jobName);
					}
				}

				ProcessPkgs(upkList, udkList, childResList);
				++g_currentJobIdx;
				JobCompleted();
			}
			else
			{
				break;
			}
		}

		FString saveFile = GetCurrentChildProcessWorkDir() * TEXT("ResList.bin");
		WriteResList(saveFile, childResList);
	}

	//
	// Collect ref res list from config tables
	//
	if(!g_isChildProcess)
	{
		CollectResRefFromConfig();


		// save to file

		WriteResList(TEXT("ResRef.bin"), g_resourcesList);
	}
#endif WITH_EDITOR

	return 0;
}

#if WITH_EDITOR

void UMakeRefListCommandlet::ProcessPkgs(const TArray<FString>& upkList, const TArray<FString>& udkList, TSet<FString>& resources)
{
	TSet<FString> tmpRes;

	// proc upks
	for (INT j = 0; j < upkList.Num(); ++j)
	{
		const FString& pkgName = upkList(j);

		SET_WARN_COLOR(COLOR_GREEN);
		warnf(TEXT("[%d/%d] Loading package:%s..."), j, upkList.Num(), *pkgName);
		CLEAR_WARN_COLOR();

		UPackage* Package = UObject::LoadPackage( NULL, *pkgName, LOAD_Quiet|LOAD_NoWarn );
		if( !Package )
		{
			warnf(NAME_Error, TEXT("Failed to load upk:%s"), *pkgName);
			continue;
		}

		if(!Package->IsFullyLoaded())
		{
			Package->FullyLoad();
		}

		for (INT k = 0; k < Package->GetNetObjectCount(0); ++k)
		{
			UObject* obj = Package->GetNetObjectAtIndex(k);
			if(obj)
			{
				_CollectReferencedResource(obj, tmpRes);
			}
			else
			{
				warnf(NAME_Warning, TEXT("Failed to load object:%s Idx:%d"), *pkgName, k);
			}
		}

		for( FObjectIterator It; It; ++It )
		{
			UObject* Object = *It;
			if(IsResourceClass(Object))
			{				
				//warnf(TEXT("======== found object:%s"), *Object->GetFullName());
				_CollectReferencedResource(Object, tmpRes);
			}
		}

		resources = resources.Union(tmpRes);
		M_LOG_RES_FROM("Found from upk:%s");
	}

	// load all maps
	for (INT j = 0; j < udkList.Num(); ++j)
	{
		FFilename fnm(udkList(j));
		if(fnm.GetExtension() != FURL::DefaultMapExt)
		{
			continue;
		}

		const FString& mapName = udkList(j);
		SET_WARN_COLOR(COLOR_GREEN);
		warnf(TEXT("[%d/%d] Loading map:%s..."), j, udkList.Num(), *mapName);
		CLEAR_WARN_COLOR();

		g_objProcessed.clear();
				
 		GWorld->ClearComponents();
		UObject::CollectGarbage( RF_Native );

// 		// At this point the only world still around should be GWorld, which is the one created in CreateNew.
// 		for( TObjectIterator<UWorld> It; It; ++It )
// 		{
// 			UWorld* World = *It;
// 			if( World != GWorld )
// 			{
// 				UObject::StaticExec(*FString::Printf(TEXT("OBJ REFS CLASS=WORLD NAME=%s.TheWorld"), *World->GetOutermost()->GetName()));
// 
// 				TMap<UObject*,UProperty*>	Route		= FArchiveTraceRoute::FindShortestRootPath( World, TRUE, GARBAGE_COLLECTION_KEEPFLAGS );
// 				FString						ErrorString	= FArchiveTraceRoute::PrintRootPath( Route, World );
// 				debugf(TEXT("%s"),*ErrorString);		
// 
// 				// We cannot safely recover from this.
// 				appErrorf( TEXT("%s not cleaned up by garbage collection!") LINE_TERMINATOR TEXT("%s"), *World->GetFullName(), *ErrorString );
// 			}
// 		}

		FString fullMapName;

		BOOL bFoundF = GPackageFileCache->FindPackageFile( *mapName, NULL, fullMapName );
		if ( !bFoundF )
		{
			continue;
		}

		fullMapName = GFileManager->ConvertToAbsolutePath( *fullMapName );

		GEditor->Exec(*FString::Printf(TEXT("MAP LOAD FILE=%s"), *fullMapName));

		UPackage* WorldPackage = UObject::LoadPackage( NULL, *fullMapName, LOAD_None );

		if ( !WorldPackage )
		{
			continue;
		}
		WorldPackage->FullyLoad();

// 		UWorld* World = FindObject<UWorld>( WorldPackage, TEXT("TheWorld") );
// 		if (World != NULL)
// 		{
// 			AWorldInfo* WorldInfo = World->GetWorldInfo();
// 			if (World->PersistentLevel != NULL)
// 			{
// 				World->PersistentLevel->UpdateComponents();
// 			}
// 		}

 		GWorld->UpdateLevelStreaming();
 		GWorld->UpdateComponents( FALSE );

#ifdef _DEBUG
		for( TObjectIterator<UWorld> It; It; ++It )
		{
			UWorld* RemainingWorld = *It;
			warnf(NAME_Log, TEXT("---- World:%x"), RemainingWorld);
		}
#endif
		for( FObjectIterator It; It; ++It )
		{
			UObject* Object = *It;
			if(IsResourceClass(Object))
			{				
				//warnf(TEXT("======== found object:%s"), *Object->GetFullName());
				_CollectReferencedResource(Object, tmpRes);
			}
		}

		_CollectReferencedResource(GWorld->GetGameSequence(), tmpRes);
		resources = resources.Union(tmpRes);
		M_LOG_RES_FROM("Found from kismet:%s");

		tmpRes.Empty();
#if 1
		for (INT i = 0; i < GWorld->Levels.Num(); ++i)
		{
			ULevel* level = GWorld->Levels(i);
			for (INT j = 0; j < level->Actors.Num(); ++j)
			{
				AActor* actor = level->Actors(j);
				if ( actor )
				{			
#if 0
					if(actor->IsA(AStaticMeshActor::StaticClass())
						|| actor->IsA(ASkeletalMeshActor::StaticClass()))
					{
						warnf(TEXT("Searching:%s..."), *actor->GetFullName());
					}
#endif

					_CollectReferencedResource(actor, tmpRes);
				}
			}
		}

		resources = resources.Union(tmpRes);
		M_LOG_RES_FROM("Found from map:%s");
#endif
	}
}

struct FResInfo
{
	UINT SizeBytes;
	FString FolderKey;

	//std::map<FString, UINT, ComparFString> FolderResSizeMap;
};

typedef std::map<FString, FResInfo, ComparFString> ObjectResInfoMap;
typedef std::map<UClass*, ObjectResInfoMap> ClassResInfoMap;
ClassResInfoMap resRefInfoMap;

extern FString GetFolderKey(const FString& filename);

void AddToRefResList( UObject* obj, TSet<FString>& outResList)
{
	//warnf(TEXT("===			adding:%s"), *obj->GetFullName());
	outResList.Add(obj->GetFullName());

// 	if(obj->GetFullName().InStr(TEXT("WA_F_Chest_01")) != INDEX_NONE)
// 	{
// 		appDebugBreak();
// 	}

#if 0
	UClass* cls = obj->GetClass();

	FString resPath;
	if(obj->GetOutermost())
	{
		FString pkgFolder = obj->GetOutermost()->GetFolderName().ToString();
		if(!GPackageFileCache->FindPackageFile(*obj->GetOutermost()->GetName(), NULL, resPath))
		{
			//resPath = Filename;
		}
	}

	FString folderKey = GetFolderKey(resPath);

	ClassResInfoMap::iterator i = resRefInfoMap.find(cls);
	if(i == resRefInfoMap.end())
	{
		i = resRefInfoMap.insert(std::make_pair(cls, ObjectResInfoMap())).first;
	}

	ObjectResInfoMap::iterator j = i->second.find(obj->GetFullName());
	if(j == i->second.end())
	{
		j = i->second.insert(std::make_pair(obj->GetFullName(), FResInfo())).first;
		j->second.SizeBytes = obj->GetResourceSize();
		j->second.FolderKey = folderKey;
		// 		std::map<FString, UINT, ComparFString>::iterator foldSzIt = j->second.FolderResSizeMap.find(folderKey);
		// 		if(foldSzIt == j->second.FolderResSizeMap.end())
		// 		{
		// 			j->second.FolderResSizeMap[folderKey] = obj->GetResourceSize()
		// 		}
	}
#endif
}

void DumpRefResSizeInfo()
{
	SET_WARN_COLOR(COLOR_GREEN);
	warnf(TEXT("===========Resource Size==========="));
	CLEAR_WARN_COLOR();

	UINT64 totalSize = 0;

	ClassResInfoMap::iterator i = resRefInfoMap.begin();
	for (; i != resRefInfoMap.end(); ++i)
	{
		UINT64 clsSize = 0;
		ObjectResInfoMap::iterator j = i->second.begin();

		struct FolderResInfo
		{
			UINT ResSize;	
			UINT ResCount;

			FolderResInfo(UINT sz, UINT cnt) : ResSize(sz), ResCount(cnt){}
		};

		typedef std::map<FString, FolderResInfo, ComparFString> FolderResInfoMap;
		FolderResInfoMap folderSize;

		for (; j != i->second.end(); ++j)
		{
			clsSize += j->second.SizeBytes;

			FolderResInfoMap::iterator folderIt = folderSize.find(j->second.FolderKey);
			if(folderIt == folderSize.end())
			{
				folderSize.insert(std::make_pair(j->second.FolderKey, FolderResInfo(j->second.SizeBytes,  1)));
			}
			else
			{
				folderIt->second.ResSize += j->second.SizeBytes;
				folderIt->second.ResCount++;
			}
		}

		if(clsSize > 0)
		{
			totalSize += clsSize;
			SET_WARN_COLOR(COLOR_GREEN);
			warnf(TEXT("%30s		%7.2f MB %6d"), *i->first->GetName(), FLOAT(clsSize / 1024.0f / 1024.0f), i->second.size());
			CLEAR_WARN_COLOR();

			SET_WARN_COLOR(COLOR_DARK_GREEN);
			FolderResInfoMap::const_iterator itFolderSz = folderSize.begin();


			for (; itFolderSz != folderSize.end(); ++itFolderSz)
			{
				warnf(TEXT("\t%50s		%7.2f MB %2.1f%% %6d"),
					*itFolderSz->first, FLOAT(itFolderSz->second.ResSize / 1024.0f / 1024.0f),
					FLOAT(itFolderSz->second.ResSize) / FLOAT(clsSize) * 100.0f, itFolderSz->second.ResCount);
			}
			CLEAR_WARN_COLOR();
		}
		else
		{
			//SET_WARN_COLOR(COLOR_DARK_GREEN);
		}

		//CLEAR_WARN_COLOR();
	}

	SET_WARN_COLOR(COLOR_GREEN);
	warnf(TEXT("Total resource size:%.2f MB"), FLOAT(totalSize / 1024.0f / 1024.0f));
	CLEAR_WARN_COLOR();
}

void UMakeRefListCommandlet::CollectForDiffuseTexture(const FString& diffuseTexPath, TSet<FString>& tmpRes)
{
	if(diffuseTexPath.StartsWith(TEXT("Texture2D")))
	{
		static const int prefixLen = FString("Texture2D ").Len();

		FString texName = diffuseTexPath.RightChop(prefixLen);
		//warnf(TEXT("=== try find N  S for:%s"), *texName);
// 		if(texName.InStr(TEXT("WA_F_Chest_01")) != INDEX_NONE)
// 		{
// 			appDebugBreak();
// 		}

		UObject* DiffuseTexture = StaticLoadObject(UTexture2D::StaticClass(), NULL, *texName, NULL, LOAD_NoWarn|LOAD_Quiet, NULL);
		if (DiffuseTexture)
		{
			FString OldName = DiffuseTexture->GetName();
			FString PathName = DiffuseTexture->GetPathName();

			FString NameN = PathName.LeftChop(1) + TEXT("N");
			FString NameS = PathName.LeftChop(1) + TEXT("S");

			if (DiffuseTexture->IsInExportedPackage())
			{
				FString NewName = OldName.LeftChop(1) + TEXT("N");
				NameN = DiffuseTexture->GetOuter()->GetName();
				NameN = NameN.Replace(*OldName, *NewName, TRUE) + TEXT(".") + NewName;

				NewName = OldName.LeftChop(1) + TEXT("S");
				NameS = DiffuseTexture->GetOuter()->GetName();
				NameS = NameS.Replace(*OldName, *NewName, TRUE) + TEXT(".") + NewName;
			}

			UObject* texObj = StaticLoadObject(UTexture2D::StaticClass(), NULL, *NameN, NULL, LOAD_NoWarn|LOAD_Quiet, NULL);
			if(!texObj)
			{
				warnf(NAME_Error, TEXT("=== failed to load normalmap texture: %s"), *NameN);
			}
			else
			{
				_CollectReferencedResource(texObj, tmpRes);
			}

			texObj = StaticLoadObject(UTexture2D::StaticClass(), NULL, *NameS, NULL, LOAD_NoWarn|LOAD_Quiet, NULL);
			if(!texObj)
			{
				warnf(NAME_Error, TEXT("=== failed to load specular texture: %s"), *NameS);
			}
			else
			{
				_CollectReferencedResource(texObj, tmpRes);
			}
		}
		else
		{
			warnf(NAME_Error, TEXT("XXXXXXXX Diffuse load failed:%s"), *texName);
		}
	}	
}

void UMakeRefListCommandlet::CollectResRefFromConfig()
{
	TSet<FString> tmpRes;

	// item\item_armor\doodad\ability\quest\ui\sound matrix
	g_objProcessed.clear();

#if 1
	extern void CollectReferencedResourceFromDataTable(TSet<FString>& outResList);
	CollectReferencedResourceFromDataTable(tmpRes);

	for( FObjectIterator It; It; ++It )
	{
		UObject* Object = *It;
		if(IsResourceClass(Object))
		{				
			//warnf(TEXT("======== found object:%s"), *Object->GetFullName());
			_CollectReferencedResource(Object, tmpRes);
		}
	}

	g_resourcesList = g_resourcesList.Union(tmpRes);

	// add normal/specular
	{
		TSet<FString> diffuseNSList = tmpRes;
		TSet<FString>::TIterator itTmp(tmpRes);
		for (; itTmp; ++itTmp)
		{
			const FString& k = *itTmp;
			CollectForDiffuseTexture(k, diffuseNSList);		
		}

		tmpRes = diffuseNSList;
	}

	M_LOG_RES_FROM("Found from config table:%s");
#endif

	g_objProcessed.clear();
	_CollectReferencedResource(UGamePlayDataManager::GetSingleton(), tmpRes);
	for( FObjectIterator It; It; ++It )
	{
		UObject* Object = *It;
		if(IsResourceClass(Object))
		{				
			//warnf(TEXT("======== found object:%s"), *Object->GetFullName());
			_CollectReferencedResource(Object, tmpRes);
		}
	}

	// add normal/specular for avatar
	{
		TSet<FString> diffuseNSList = tmpRes;
		TSet<FString>::TIterator itTmp(tmpRes);
		for (; itTmp; ++itTmp)
		{
			const FString& k = *itTmp;
			CollectForDiffuseTexture(k, diffuseNSList);		
		}

		tmpRes = diffuseNSList;
	}

	g_resourcesList = g_resourcesList.Union(tmpRes);
	M_LOG_RES_FROM("Found from GamePlayDataManager:%s");

	DumpRefResSizeInfo();

	SET_WARN_COLOR(COLOR_CYAN);
	warnf(TEXT("Total found resources:%d"), g_resourcesList.Num());
	CLEAR_WARN_COLOR();
}


//------------------------------------------------------------------------------------------------
// UMakeRefListCommandlet
//------------------------------------------------------------------------------------------------
INT UDumpRefListCommandlet::Main(const FString& Params)
{
#if WITH_EDITOR

	//FString resBinFile = GetChildProcessWorkDir(ProcessIndex) * TEXT("ResList.bin");

	ParseCommandLine( *Params, g_Tokens, g_Switches );

	warnf(TEXT("DumpRefList file1.bin				dump ResRef.bin to txt"));
	warnf(TEXT("DumpRefList file1.bin				dump file1.bin to txt"));
	warnf(TEXT("DumpRefList file1.bin file2.bin		compare difference"));

	warnf(TEXT("Tokens:%d Switches:%d"), g_Tokens.Num(), g_Switches.Num());

// 	for (INT i = 0; i < g_Tokens.Num(); ++i)
// 	{
// 		warnf(TEXT("%s"), *g_Tokens(i));
// 	}
// 	return 0;

	if(g_Tokens.Num() == 2)
	{
		TSet<FString> tmpListOld = UMakeRefListCommandlet::ReadResList(g_Tokens(0));
		TSet<FString> tmpListNew = UMakeRefListCommandlet::ReadResList(g_Tokens(1));

		FString newAddList, deletList;
		INT newCnt = 0, delCnt = 0;

		TSet<FString>::TIterator itOld(tmpListOld);
		for (; itOld; ++itOld)
		{		
			// Old ones not in the new are deleted
			const FString& oldStr = *itOld;
			if(!tmpListNew.Find(oldStr))
			{
				deletList += oldStr;
				deletList += TEXT("\r\n");
				++delCnt;
			}
		}

		TSet<FString>::TIterator itNew(tmpListNew);
		for (; itNew; ++itNew)
		{		
			// New ones not in the old are added
			const FString& newStr = *itNew;
			if(!tmpListOld.Find(newStr))
			{
				newAddList += newStr;
				newAddList += TEXT("\r\n");
				++newCnt;
			}
		}

		appSaveStringToFile(newAddList, TEXT("_new.txt"));
		appSaveStringToFile(deletList, TEXT("_delete.txt"));
		warnf(TEXT("Write difference in _new.txt:%d		_delete.txt:%d"), newCnt, delCnt);

		return 0;
	}

	FString resFile(TEXT("ResRef.bin"));

	if(g_Tokens.Num() == 1)
	{
		resFile = g_Tokens(0);
	}

	TSet<FString> tmpList = UMakeRefListCommandlet::ReadResList(resFile);
	
	FString dumpList;
	TSet<FString>::TIterator it(tmpList);
	for (; it; ++it)
	{		
		const FString& k = *it;
		dumpList += k;
		dumpList += TEXT("\r\n");
		//warnf(*k);
	}
	FString fileName = resFile + TEXT(".txt");
	appSaveStringToFile(dumpList, *fileName);
	warnf(TEXT("==== Total ref res count:%d		%s"), tmpList.Num(), *fileName);

#endif

	return 0;
}
#endif WITH_EDITOR
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


IMPLEMENT_CLASS(UExportAllObjectMTCommandlet)

#if WITH_EDITOR

extern bool g_isCommandlet;
/** delay in seconds for the spinning involved in synchronizing child jobs */
extern const FLOAT MTSpinDelay; 
/** time in seconds before we decide someone has crashed or a file is permanently locked */
extern FLOAT MTTimeout; 


// Inconvenient to write in .h, all written here as member variables
bool gexp_isChildProcess = false;
INT gexp_NumChildProcesses = 8;
INT gexp_currentProcessIdx = -1;
INT gexp_currentJobIdx = 0;
INT gexp_batchCnt = 1;

TArray<FString> gexp_Tokens;
TArray<FString> gexp_Switches;
TArray<UCookPackagesCommandlet::FChildProcess> gexp_ChildProcesses;

TArray<FString> NativePackageName;

extern UBOOL GIsExportingResources;
TArray<FString> CanNotExportPackageName;
// FString ExportString(TEXT("_SBExp"));


FString UExportAllObjectMTCommandlet::GetChildProcessWorkDir(INT processIdx) const
{
	FString ChildString = appGameLogDir() * FString::Printf(TEXT("ExportAllObjChildProcess_%x"), processIdx);
	return ChildString;
}

FString UExportAllObjectMTCommandlet::GetCurrentChildProcessWorkDir() const
{
	return GetChildProcessWorkDir(gexp_currentProcessIdx);
}

void UExportAllObjectMTCommandlet::StartChildren(INT NumJobs)
{
	FString Args = appCmdLine();
	Args += FString(" -MTCHILD");
	TCHAR ExeName[MAX_FILEPATH_LENGTH];
	GetModuleFileName(NULL, ExeName, MAX_FILEPATH_LENGTH - 1);
	for (INT ProcessIndex = 0; ProcessIndex < NumJobs; ProcessIndex++)
	{
		UCookPackagesCommandlet::FChildProcess ProcessInfo;
		ProcessInfo.LogFilename = FFilename(appBaseDir()) * appGameLogDir() * FString::Printf(TEXT("Child%dExportAllObj.log"), ProcessIndex);

#ifdef _DEBUG
		FString Command = FString::Printf(TEXT("ExportAllObjectMT %s -ProcIdx=%d -nopause -ABSLOG=\"%s\""), *Args, ProcessIndex, *ProcessInfo.LogFilename);
#else
		FString Command = FString::Printf(TEXT("ExportAllObjectMT %s -ProcIdx=%d -nopause -unattended -ABSLOG=\"%s\""), *Args, ProcessIndex, *ProcessInfo.LogFilename);
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
		ProcessInfo.CommandFile = ProcessInfo.Directory * TEXT("ExportAllObjCmd.txt");
		ProcessInfo.StartTime = 0.0;
		gexp_ChildProcesses.AddItem(ProcessInfo);
	}
}

/**
 * Determine if a given child process is idle or not
 *  Only used if bIsMTMaster is TRUE. 
 *
 * @param Job String representing the filename of the file to cook on the child process
 * @return TRUE if the given child process is idle
 */
UBOOL UExportAllObjectMTCommandlet::ChildIsIdle(INT ProcessIndex)
{
	UBOOL bIdle = !gexp_ChildProcesses(ProcessIndex).CommandFile.NonLockingFileExists();
	if (bIdle && gexp_ChildProcesses(ProcessIndex).StartTime > 0.0)
	{
		warnf(NAME_Log,TEXT("Job[%d] %s done in %5.1fs"),
			ProcessIndex,
			*gexp_ChildProcesses(ProcessIndex).LastCommand,
			FLOAT(appSeconds() - gexp_ChildProcesses(ProcessIndex).StartTime));
		gexp_ChildProcesses(ProcessIndex).StartTime = 0.0;
		gexp_ChildProcesses(ProcessIndex).JobsCompleted++;
	}
	return bIdle;
}


/**
 * Waits until a child process is available, then gives it the given job
 *  Only used if bIsMTMaster is TRUE. 
 *
 */
void UExportAllObjectMTCommandlet::StartChildJob(const TArray<FString>& Job)
{
	for(;;)
	{
		UBOOL Sent = FALSE;
		static INT LastSent = -1;
		for (INT ProcessIndex = LastSent + 1; ProcessIndex < gexp_ChildProcesses.Num(); ProcessIndex++)
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
			for (INT ProcessIndex = 0; ProcessIndex <= LastSent && ProcessIndex < gexp_ChildProcesses.Num(); ProcessIndex++)
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
// 		if (gexp_ChildProcesses.Num() > 6)
// 		{
// 			static INT LastSync = -1;
// 
// 			INT NumSync = 0;
// 			INT PrevLastSync = LastSync;
// 			const INT ToDoPerTurn = 2;
// 
// 			for (INT ProcessIndex = PrevLastSync + 1; ProcessIndex < gexp_ChildProcesses.Num() && NumSync < ToDoPerTurn; ProcessIndex++)
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
// 				for (INT ProcessIndex = 0; ProcessIndex <= PrevLastSync && ProcessIndex < gexp_ChildProcesses.Num() && NumSync < ToDoPerTurn; ProcessIndex++)
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
// // 			for (INT ProcessIndex = 0; ProcessIndex < gexp_ChildProcesses.Num(); ProcessIndex++)
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
void UExportAllObjectMTCommandlet::JobCompleted()
{
	FFilename CommandFile(GetCurrentChildProcessWorkDir() * TEXT("ExportAllObjCmd.txt"));

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
void UExportAllObjectMTCommandlet::WaitToDeleteFile(const TCHAR *Filename)
{
	if(!GFileManager->Delete(Filename))
	{
		DOUBLE StartTime = appSeconds();
		do 
		{
			if (!gexp_isChildProcess)
			{
				CheckForCrashedChildren();
			}

			appSleep(MTSpinDelay);
			
			if (appSeconds() - StartTime > MTTimeout)
			{
				// do this one more time because a detected child crash is a little more graceful
				if (!gexp_isChildProcess)
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
TArray<FString> UExportAllObjectMTCommandlet::GetMyNextJob()
{
	FFilename CommandFile(GetCurrentChildProcessWorkDir() * TEXT("ExportAllObjCmd.txt"));
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
void UExportAllObjectMTCommandlet::SendChildCommand(INT ProcessIndex, const TArray<FString>& Command)
{
// 	if (CookStats.TFCTextureSaved - gexp_ChildProcesses(ProcessIndex).LastTFCTextureSaved > 10 * 1024 * 1024
// 		&& gexp_ChildProcesses(ProcessIndex).JobsCompleted > 0)  // must have completed at least one job, otherwise it may already need a TFC sync from startup
// 	{
// 		// we saved more than 10MB of TFC so the child should load the new PCD to avoid recooking texture
// 		SavePersistentCookerDataForChild(ProcessIndex);
// 	}
	gexp_ChildProcesses(ProcessIndex).StartTime = appSeconds();
	if(Command.Num() > 0)
	{
		gexp_ChildProcesses(ProcessIndex).LastCommand =
			FString::Printf(TEXT("%s x %d"),*FFilename(Command(0)).GetCleanFilename(), Command.Num());
	}

	RobustSaveStringToFile(Command, *gexp_ChildProcesses(ProcessIndex).CommandFile);
}

/**
 * Sends stop commands to all child processes that are idle but have not yet been stopped
 *  Only used if bIsMTMaster is TRUE. 
 *
 * @return	TRUE if all children have been sent stop commands
 */
UBOOL UExportAllObjectMTCommandlet::SendStopCommands()
{
	UBOOL AllDone = TRUE;
	CheckForCrashedChildren();
	for (INT ProcessIndex = 0; ProcessIndex < gexp_ChildProcesses.Num(); ProcessIndex++)
	{
		if (!gexp_ChildProcesses(ProcessIndex).bStopped)
		{
			if (ChildIsIdle(ProcessIndex))
			{
				FString cmd(TEXT("*stop"));
				TArray<FString> cmdList;
				cmdList.AddItem(cmd);
				SendChildCommand(ProcessIndex, cmdList);
				gexp_ChildProcesses(ProcessIndex).bStopped = TRUE;
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
void UExportAllObjectMTCommandlet::StopChildren()
{
	while (!SendStopCommands())
	{
		appSleep(MTSpinDelay);
	}

	for (INT ProcessIndex = 0; ProcessIndex < gexp_ChildProcesses.Num(); ProcessIndex++)
	{
		UCookPackagesCommandlet::FChildProcess& procInfo = gexp_ChildProcesses(ProcessIndex);
		check(procInfo.bStopped);
		check(!procInfo.bMergedResults);

		appWaitForProc(procInfo.ProcessHandle);
		//MergeChildProducts(ProcessIndex);
		//GFileManager->DeleteDirectory(*gexp_ChildProcesses(ProcessIndex).Directory,FALSE,TRUE);
// 		FString resBinFile = GetChildProcessWorkDir(ProcessIndex) * TEXT("ResList.bin");
// 		warnf(TEXT("Merging list from: %s ..."), *resBinFile);
// 		TSet<FString> tmpList = ReadResList(resBinFile);
// 		gexp_resourcesList = gexp_resourcesList.Union(tmpList);

		GFileManager->DeleteDirectory(*procInfo.Directory, FALSE, TRUE);

		gexp_ChildProcesses(ProcessIndex).bMergedResults = TRUE;
	}
	MergeLogs();
	gexp_ChildProcesses.Empty();
}

/**
 * A robust version of appSaveStringToFile
 * we save to a temp file, then rename. This is important for synchronization, which is what this is used for
 *
 * @param	string to save
 * @param	Filename file to save
 */
void UExportAllObjectMTCommandlet::RobustSaveStringToFile(const TArray<FString>& String,const TCHAR *Filename)
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
void UExportAllObjectMTCommandlet::CheckForCrashedChildren()
{
	UBOOL bAnyCrashed = FALSE;
	for (INT ProcessIndex = 0; ProcessIndex < gexp_ChildProcesses.Num(); ProcessIndex++)
	{
		if (!gexp_ChildProcesses(ProcessIndex).bStopped)
		{
			if (!appIsProcRunning(gexp_ChildProcesses(ProcessIndex).ProcessHandle))
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

		for (INT ProcessIndex = 0; ProcessIndex < gexp_ChildProcesses.Num(); ProcessIndex++)
		{
			if (!gexp_ChildProcesses(ProcessIndex).bStopped)
			{
				gexp_ChildProcesses(ProcessIndex).bStopped = TRUE;
				warnf(NAME_Log, TEXT("Job[%d] Killed."),ProcessIndex);
				appTerminateProc(gexp_ChildProcesses(ProcessIndex).ProcessHandle);
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
void UExportAllObjectMTCommandlet::MergeLogs(FString *CriticalReason)
{
	if (GIsBuildMachine)
	{
		if (CriticalReason)
		{
			for (INT ProcessIndex = 0; ProcessIndex < gexp_ChildProcesses.Num(); ProcessIndex++)
			{
				if (gexp_ChildProcesses(ProcessIndex).LogFilename.FileExists())
				{
					FString Log;
					if (appLoadFileToString(Log,*gexp_ChildProcesses(ProcessIndex).LogFilename))
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
	for (INT ProcessIndex = 0; ProcessIndex < gexp_ChildProcesses.Num(); ProcessIndex++)
	{
		if (gexp_ChildProcesses(ProcessIndex).LogFilename.FileExists())
		{
			FString Log;
			if (appLoadFileToString(Log,*gexp_ChildProcesses(ProcessIndex).LogFilename))
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


#endif

UClass* exportTypeClasses[] =
{
	UTexture::StaticClass(),
	UStaticMesh::StaticClass(),
	USkeletalMesh::StaticClass(),
	UParticleSystem::StaticClass(),
	UAnimSet::StaticClass(),
	USoundNodeWave::StaticClass(),
	UFont::StaticClass()
};

bool IsCanExportTypeClass(UObject* obj)
{
	for (INT i = 0; i < ARRAY_COUNT(exportTypeClasses); ++i)
	{
		if(obj->IsA(exportTypeClasses[i]))
		{
			return true;
		}
	}

	return false;
}

INT UExportAllObjectMTCommandlet::Main(const FString& Params)
{
#if WITH_EDITOR

// #ifdef _WIN64
// 	FUntypedBulkData::gexp_SkipLoadBulkData = TRUE; // don't load bulkdata that we really don't need
// #endif
	
	GIsExportingResources = TRUE;
	g_isCommandlet = true;

// #if 0
// 	FArchive* arr = GFileManager->CreateFileReader(TEXT("ExportAllObj.bin"));
// 	(*arr) << resources;
// 	delete arr;
// 	warnf(TEXT("ExportAllObj:%d"), resources.Num());
// 	return 0;
// #endif

	GUnrealEd = Cast<UUnrealEdEngine>(GEditor);
	
	SetFlags(RF_Marked);

	ParseCommandLine( *Params, gexp_Tokens, gexp_Switches );

	for (INT i = 0; i < gexp_Tokens.Num(); ++i)
	{
		ConvertFileRootFolders.AddUniqueItem(gexp_Tokens(i));
	}
	
	NativePackageName.AddItem(TEXT("Core"));
	NativePackageName.AddItem(TEXT("Engine"));
	NativePackageName.AddItem(TEXT("GameFramework"));
	NativePackageName.AddItem(TEXT("GFxUI"));
	NativePackageName.AddItem(TEXT("GFxUIEditor"));
	NativePackageName.AddItem(TEXT("IpDrv"));
	NativePackageName.AddItem(TEXT("MMOGame"));
	NativePackageName.AddItem(TEXT("OnlineSubsystemGameSpy"));
	NativePackageName.AddItem(TEXT("OnlineSubsystemLive"));
	NativePackageName.AddItem(TEXT("OnlineSubsystemPC"));
	NativePackageName.AddItem(TEXT("OnlineSubsystemSteamworks"));
	NativePackageName.AddItem(TEXT("UnrealEd"));		
	NativePackageName.AddItem(TEXT("WinDrv"));

	CanNotExportPackageName = NativePackageName;
	for (INT i = 0; i < NativePackageName.Num(); ++i)
	{
		UPackage* ToSearchPackage = UObject::FindPackage(NULL, *NativePackageName(i));
		if (!ToSearchPackage)
		{
			continue;
		}

		ULinkerLoad* linker = ULinkerLoad::FindExistingLinkerForPackage(ToSearchPackage);
		check(linker);
		TArray<FObjectImport>& ImportMap = linker->GetImportMap();
		for (INT j = 0; j < ImportMap.Num(); ++j)
		{
			if (ImportMap(j).OuterIndex == 0 && ImportMap(j).ClassName == FName(TEXT("Package")))
			{
				CanNotExportPackageName.AddUniqueItem(ImportMap(j).ObjectName.ToString());
			}
		}
	}

	gexp_isChildProcess = (gexp_Switches.FindItemIndex(TEXT("MTCHILD")) != INDEX_NONE);

	Parse(appCmdLine(), TEXT("Processes="), gexp_NumChildProcesses);
	Parse(appCmdLine(), TEXT("BatchJobCnt="), gexp_batchCnt);
	

	if(gexp_isChildProcess)
	{
		Parse(appCmdLine(), TEXT("ProcIdx="), gexp_currentProcessIdx);

		warnf(NAME_Log, TEXT("====[Child Process]==== Idx:%d"), gexp_currentProcessIdx);
	}

	bool autoMaps = false;
	if (ParseParam(appCmdLine(), TEXT("AUTOMAPLIST")))
	{
		autoMaps = true;
	}

	if(!gexp_isChildProcess)
	{
		TArray<FString> udkList, upkList;

		for (INT i = 0; i < ConvertFileRootFolders.Num(); ++i)
		{
			TArray<FString> pkgs;
			appFindFilesInDirectory(pkgs, *ConvertFileRootFolders(i), TRUE, FALSE);
			for (INT n = 0; n < pkgs.Num(); ++n)
			{
				FString PackageName = FPackageFileCache::PackageFromPath(*pkgs(n));
				INT Index = 0;
				if (CanNotExportPackageName.FindItem(PackageName, Index))
				{
					continue;
				}

				if(NativePackageName.ContainsItem(PackageName))
				{
					continue;
				}

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


		//
		// start children jobs
		//

		if(gexp_NumChildProcesses > 0)
		{
			warnf(NAME_Log, TEXT("------- Multiprocess collect, starting %d processes for %d upks   %d udks -------"),
				gexp_NumChildProcesses, upkList.Num(), udkList.Num());

			StartChildren(gexp_NumChildProcesses);

			
			//
			// send proc command to child processes
			//
			
			DOUBLE startTime = appSeconds();

			INT i = 0;
			INT curBatchCnt = 0;
			TArray<FString> cmdList;

			//if(!gexp_debugSkipUPK)
			{
				while (i < upkList.Num())
				{
					if(curBatchCnt < gexp_batchCnt)
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

// 			if(!gexp_debugSkipUDK)
// 			{
// 				i = 0;
// 
// 				while (i < udkList.Num())
// 				{
// 					if(curBatchCnt < gexp_batchCnt)
// 					{
// 						cmdList.AddItem(udkList(i));
// 						++curBatchCnt;
// 						++i;
// 					}
// 					else
// 					{
// 						SET_WARN_COLOR(COLOR_GREEN);
// 						warnf(NAME_Log, TEXT("Processing UDK: [%d/%d]"), i, udkList.Num());
// 						CLEAR_WARN_COLOR();
// 
// 						StartChildJob(cmdList);
// 						curBatchCnt = 0;
// 						cmdList.Empty();
// 						//--i;
// 					}
// 				}
// 
// 				if(cmdList.Num())
// 				{
// 					SET_WARN_COLOR(COLOR_GREEN);
// 					warnf(NAME_Log, TEXT("Processing UDK: [%d/%d]"), i, udkList.Num());
// 					CLEAR_WARN_COLOR();
// 
// 					StartChildJob(cmdList);
// 					curBatchCnt = 0;
// 					cmdList.Empty();
// 				}
// 			}

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

			TSet<FString> gexp_resourcesList;
			ProcessPkgs(upkList, udkList, gexp_resourcesList);
		}
	}


	if(gexp_isChildProcess)
	{
#ifdef _DEBUG
		appMsgf(AMT_OK, TEXT("Break on child:%d"), gexp_currentProcessIdx);
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
				++gexp_currentJobIdx;
				JobCompleted();
			}
			else
			{
				break;
			}
		}

// 		FString saveFile = GetCurrentChildProcessWorkDir() * TEXT("ResList.bin");
// 		WriteResList(saveFile, childResList);
	}

	//
	// Collect ref res list from config tables
	//
	if(!gexp_isChildProcess)
	{
// 		CollectExportAllObjFromConfig();
// 
// 
// 		// save to file
// 
// 		WriteResList(TEXT("ExportAllObj.bin"), gexp_resourcesList);
	}

	ClearFlags(RF_Marked);


	GIsExportingResources = FALSE;
#endif WITH_EDITOR

	return 0;
}

#if WITH_EDITOR

void UExportAllObjectMTCommandlet::ProcessPkgs(const TArray<FString>& upkList, const TArray<FString>& udkList, TSet<FString>& resources)
{
	TSet<FString> tmpRes;

	// proc upks
	for (INT j = 0; j < upkList.Num(); ++j)
	{
		const FString& PackageName = upkList(j);
				
		UPackage* ToProcessPackage = UObject::LoadPackage(NULL, *PackageName, LOAD_None);
		if (ToProcessPackage == NULL)
		{
			continue;
		}

		ToProcessPackage->FullyLoad();

		if (ToProcessPackage->HasAnyFlags(PKG_ExportedResourcePackage))
		{
			continue;
		}

// 		SET_WARN_COLOR(COLOR_GREEN);
// 		warnf(NAME_Log, TEXT("Processing package: [%d/%d] %s"), j, Results.Num(), *PackageName);
// 		CLEAR_WARN_COLOR();

		INT ProcessedCount = 0;

		for( FObjectIterator It; It; ++It )
		{
			UObject* ToProcessObject = *It;
			if (ToProcessObject == NULL)
			{
				continue;
			}

			if (ToProcessObject->IsIn(ToProcessPackage) == FALSE)
			{
				continue;
			}

			if (ToProcessObject->HasAnyFlags(RF_Public) == FALSE)
			{
				continue;
			}

			if (ToProcessObject->HasAnyFlags(RF_Transient))
			{
				continue;
			}

			if (ToProcessObject->IsA(UPackage::StaticClass()))
			{
				continue;
			}

			if (ToProcessObject->IsA(UObjectRedirector::StaticClass()))
			{
				continue;
			}

			UBOOL CanExport = IsCanExportTypeClass(ToProcessObject);
// 			for (INT l = 0; l < CanExportType.Num(); ++l)
// 			{
// 				if (ToProcessObject->IsA(CanExportType(l)))
// 				{
// 					CanExport = TRUE;
// 					break;
// 				}
// 			}
			if (!CanExport)
			{
				continue;
			}

			FString NewPackageName = FString::Printf(TEXT("%s_%s"), *ToProcessObject->GetPathName(), *ToProcessObject->GetClass()->GetName());
			NewPackageName = NewPackageName.Replace(TEXT("."), TEXT("__"));

			UPackage* NewPackage = UObject::CreatePackage( NULL, *NewPackageName );

			if (NewPackage == NULL)
			{
				warnf(TEXT("Create New Package(%s) Failed"), *NewPackageName);
				continue;
			}

			debugf(TEXT("Object:%s To Export"), *ToProcessObject->GetFullName());
			if (ToProcessObject->Rename(*(ToProcessObject->GetName()), NewPackage, REN_Test))
			{
				ToProcessObject->Rename(*(ToProcessObject->GetName()), NewPackage, REN_None);
			}
			else
			{
				warnf(TEXT("Rename error:%s -> %s"), *ToProcessPackage->GetPathName(), *NewPackage->GetName());
				continue;
			}


			ToProcessObject->SetFlags(RF_Public | RF_Standalone);
			ToProcessObject->MarkPackageDirty();

			NewPackage->FullyLoad();

			FString OutPath, OutFileName, OutExtension;
			FPackageFileCache::SplitPath(*upkList(j), OutPath, OutFileName, OutExtension);

			FString ToSaveFileName = FString::Printf(TEXT("%s\\%s.%s"), *OutPath, *NewPackageName, *OutExtension);

			UObject::SavePackage( NewPackage, NULL, RF_Standalone, *ToSaveFileName, GWarn );

			ProcessedCount++;
		}

		if (ProcessedCount > 0)
		{
#if HAVE_SCC
// 			FCheckoutResult sccResult = FSourceControl::TryCheckOut(upkList(j));
// 			if (sccResult.AddToSCC == false)
// 			{
// 				continue;
// 			}
#endif
			ToProcessPackage->MarkPackageDirty(TRUE);
			//ToProcessPackage->SetFlags(PKG_Cooked);
			UObject::SavePackage(ToProcessPackage, NULL, RF_Standalone, *upkList(j), GWarn);

			debugf(TEXT("Package:%s Exported OK"), *upkList(j));
		}


		CollectGarbage(RF_Native | RF_Marked, TRUE);
	}
}


#endif WITH_EDITOR
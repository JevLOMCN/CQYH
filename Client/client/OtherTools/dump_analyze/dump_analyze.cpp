
// dump_analyze.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "dump_analyze.h"
#include "dump_analyzeDlg.h"
#include "zconf.h"
#include "zlib.h"
#ifdef _DEBUG
#pragma comment(lib, "zlib_d.lib")
#else
#pragma comment(lib, "zlib.lib")
#endif

#include <fstream>
#include "boost/regex.hpp"

using namespace std;
using namespace boost;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDumpAnalyzeApp
BEGIN_MESSAGE_MAP(CDumpAnalyzeApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	ON_COMMAND(ID_FILE_OPEN, &CDumpAnalyzeApp::OnFileOpen)
	ON_COMMAND(ID_ANALYZE, &CDumpAnalyzeApp::OnAnalyze)
	ON_COMMAND(ID_REPORT, &CDumpAnalyzeApp::OnReport)
END_MESSAGE_MAP()

void SGZ2DMP(const TCHAR* path);

void CDumpAnalyzeApp::OnReport()
{
	TCHAR dir[MAX_PATH] = {0};
	BROWSEINFO BI;
	memset(&BI, 0, sizeof(BI));
	BI.hwndOwner = theManWnd()->GetSafeHwnd();
	BI.pszDisplayName = dir;
	BI.lpszTitle = "请选择DMP目录";
	BI.ulFlags = BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST IDLST = SHBrowseForFolder(&BI);
	if (!IDLST)
		return;
	if (!SHGetPathFromIDList(IDLST, dir))
		return;
	_tcscat_s(dir, _T("\\"));

	if(IDYES == MessageBox(NULL, "处理哪种类型文件\n\nSGZ(yes)\n\nDMP(no)", "提示", MB_YESNO))
	{
		SGZ2DMP(dir);
	}

	// 找该目录下所有DUMP
	std::vector< std::string > vFileName;
	if (!GetFullPathFileList(vFileName, dir, ".dmp", true, false))
	{
		return;
	}

	map<string, map<DWORD, DWORD>> mDumpReport;
	for(size_t i = 0 ; i < vFileName.size(); ++i)
	{
		vFileName.at(i);
		FMiniDump dump;
		if(!dump.Open(vFileName.at(i).c_str(), dir))
		{
			continue;
		}
		dump.ReadException();
		dump.ReadModule(true);
		CString strTmp;
		mDumpReport[dump.GetVerString(strTmp)][dump.nDump.m_uExceptionCode]++;

		dump.Close();
	}

	_tcscat_s(dir, _T("report.txt"));
	ofstream stream(dir);
	for(map<string, map<DWORD, DWORD>>::iterator iter1 = mDumpReport.begin(); iter1 != mDumpReport.end(); ++iter1)
	{
		stream << endl << iter1->first.c_str();
		size_t nTotal = 0;
		for(map<DWORD, DWORD>::iterator iter2 = iter1->second.begin(); iter2 != iter1->second.end(); ++iter2)
		{
			nTotal += iter2->second;
		}
		stream << " total " << nTotal << endl;
		if(nTotal == 0)
		{
			continue;
		}

		for(map<DWORD, DWORD>::iterator iter2 = iter1->second.begin(); iter2 != iter1->second.end(); ++iter2)
		{
			switch(iter2->first)
			{
			case EXCEPTION_ACCESS_VIOLATION:
				stream << "EXCEPTION_ACCESS_VIOLATION";
				break;
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
				stream << "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
				break;
			case EXCEPTION_BREAKPOINT:
				stream << "EXCEPTION_BREAKPOINT";
				break;
			case EXCEPTION_DATATYPE_MISALIGNMENT:
				stream << "EXCEPTION_DATATYPE_MISALIGNMENT";
				break;
			case EXCEPTION_FLT_DENORMAL_OPERAND:
				stream << "EXCEPTION_FLT_DENORMAL_OPERAND";
				break;
			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
				stream << "EXCEPTION_FLT_DIVIDE_BY_ZERO";
				break;
			case EXCEPTION_FLT_INEXACT_RESULT:
				stream << "EXCEPTION_FLT_INEXACT_RESULT";
				break;
			case EXCEPTION_FLT_INVALID_OPERATION:
				stream << "EXCEPTION_FLT_INVALID_OPERATION";
				break;
			case EXCEPTION_FLT_OVERFLOW:
				stream << "EXCEPTION_FLT_OVERFLOW";
				break;
			case EXCEPTION_FLT_STACK_CHECK:
				stream << "EXCEPTION_FLT_STACK_CHECK";
				break;
			case EXCEPTION_FLT_UNDERFLOW:
				stream << "EXCEPTION_FLT_UNDERFLOW";
				break;
			case EXCEPTION_ILLEGAL_INSTRUCTION:
				stream << "EXCEPTION_ILLEGAL_INSTRUCTION";
				break;
			case EXCEPTION_IN_PAGE_ERROR:
				stream << "EXCEPTION_IN_PAGE_ERROR";
				break;
			case EXCEPTION_INT_DIVIDE_BY_ZERO:
				stream << "EXCEPTION_INT_DIVIDE_BY_ZERO";
				break;
			case EXCEPTION_INT_OVERFLOW:
				stream << "EXCEPTION_INT_OVERFLOW";
				break;
			case EXCEPTION_INVALID_DISPOSITION:
				stream << "EXCEPTION_INVALID_DISPOSITION";
				break;
			case EXCEPTION_NONCONTINUABLE_EXCEPTION:
				stream << "EXCEPTION_NONCONTINUABLE_EXCEPTION";
				break;
			case EXCEPTION_PRIV_INSTRUCTION:
				stream << "EXCEPTION_PRIV_INSTRUCTION";
				break;
			case EXCEPTION_SINGLE_STEP:
				stream << "EXCEPTION_SINGLE_STEP";
				break;
			case EXCEPTION_STACK_OVERFLOW:
				stream << "EXCEPTION_STACK_OVERFLOW";
				break;
			case 0xE06D7363:
				stream << "Microsoft C++ Excetption";
				break;
			default:
				stream << "OTHER_EXCEPTION";
				break;
			}
			stream << "(" << hex << iter2->first << ") " << dec << iter2->second << " " << iter2->second / (float)nTotal * 100 << "%" <<  endl;
		}
	}
	stream.close();

}

#pragma comment(lib, "Rpcrt4.lib")

string GenerateUUID()
{
	UUID uuid = {0};
	const RPC_STATUS status = UuidCreate(&uuid);
	if (status != RPC_S_OK && status != RPC_S_UUID_LOCAL_ONLY)
		return std::string();

	RPC_CSTR rpcSTR = NULL;
	if (UuidToString(&uuid, &rpcSTR) != RPC_S_OK || !rpcSTR)
		return std::string();

	string strRet = string((char*)rpcSTR);
	RpcStringFree(&rpcSTR);
	return strRet;
}


void SGZ2DMP(const TCHAR* path)
{
	std::vector< std::string > vfils;
	if (!GetFullPathFileList(vfils, path, ".sgz", true, false))
	{
		return;
	}

	for (size_t i = 0; i < vfils.size(); ++i)
	{
		std::string outputName = vfils.at(i);
		const size_t extension = outputName.find_last_of('.');
		if (extension != std::string::npos)
			outputName.erase(extension);
		outputName += ".dmp";
		FILE* fInput = fopen(vfils.at(i).c_str(),"rb");
		FILE* fOutput = fopen(outputName.c_str(),"wb+");
		if (!fInput || !fOutput)
		{
			if (fInput) fclose(fInput);
			if (fOutput) fclose(fOutput);
			continue;
		}

		int iBatchSize = 128*1024;		// 每次处理最大字节数 128k

		DWORD dwDestSize = compressBound(iBatchSize);
		Byte * pSrcMem  = (Byte *)malloc(dwDestSize);
		Byte * pDestMem = (Byte *)malloc(dwDestSize);
		if (!pSrcMem || !pDestMem)
		{
			free(pSrcMem);
			free(pDestMem);
			fclose(fInput);
			fclose(fOutput);
			continue;
		}


		DWORD dwSrcSize;
		// 写输出文件的文件头，格式是“SGZ”+一个字节的版本号
		char strSGZFileHeader[4] = "SGZ";	// 文件头标识
		unsigned char cSGZFileVersion = 1;	// 文件版号
		dwSrcSize = fread(strSGZFileHeader,1,4,fInput);
		dwSrcSize = fread(&cSGZFileVersion,1,1,fInput);

		do
		{
			// 读取源文件
			DWORD dw = dwDestSize;
			dwSrcSize = 0;
			if (fread(&dwSrcSize, 1, sizeof(dwSrcSize), fInput) != sizeof(dwSrcSize) ||
				dwSrcSize > dwDestSize)
				break;
			const DWORD requestedSize = dwSrcSize;
			dwSrcSize = (DWORD)fread(pSrcMem, 1, requestedSize, fInput);
			if (dwSrcSize != requestedSize)
				break;


			if( dwSrcSize > 0 )
			{
				if( uncompress(pDestMem, &dw, pSrcMem, dwSrcSize)==Z_OK && dw>0 )
				{
					fwrite(pDestMem,1,dw,fOutput);
				}
			}
		}
		while( dwSrcSize > 0 );

		if(fOutput)
			fclose(fOutput);

		if(fInput)
			fclose(fInput);

		if(pDestMem) free(pDestMem);
		if(pSrcMem) free(pSrcMem);
	}
}

struct stSln
{
	struct stProj
	{
		string strName;
		string strUUID;
		string strProjUUID;
	};

	~stSln()
	{
		for (int i = 0; i < vProjs.size(); ++i)
		{
			delete vProjs.at(i);
		}
	}

	vector<stProj*> vProjs;
	vector<pair<stProj*, stProj*>> vRelations;

	stProj* GetProj(const string& strUUID)
	{
		for (int i = 0; i < vProjs.size(); ++i)
		{
			if (vProjs.at(i)->strUUID == strUUID)
			{
				return vProjs.at(i);
			}
		}

		return NULL;
	}

	stProj* GetProj(const char* szName)
	{
		for (int i = 0; i < vProjs.size(); ++i)
		{
			if (vProjs.at(i)->strName == szName)
			{
				return vProjs.at(i);
			}
		}

		return NULL;
	}

	void AddProj(const char* szName)
	{
		if (GetProj(szName))
		{
			return;
		}

		stProj* pProj = new stProj();
		pProj->strName = szName;
		pProj->strProjUUID = "8BC9CEB9-9B4A-11D0-8D11-00A0C91BC942";
		pProj->strUUID = GenerateUUID();

		vProjs.push_back(pProj);
	}

	void Save_vs2005(const char* szFileName)
	{
		ofstream stream(szFileName);

		stream << "Microsoft Visual Studio Solution File, Format Version 9.00" << endl;
		stream << "# Visual Studio 2005" << endl;
		for (int i = 0; i < vProjs.size(); ++i)
		{
			char szTmp[1024];
			sprintf_s(szTmp, 1024, "Project(\"{%s}\") = \"%s\", \"%s\", \"{%s}\"", 
				vProjs.at(i)->strProjUUID.c_str(), vProjs.at(i)->strName.c_str(), vProjs.at(i)->strName.c_str(), vProjs.at(i)->strUUID.c_str());
			stream << string(szTmp).c_str() << endl;
			stream << "EndProject" << endl;
		}

		stream << "Global" << endl;
		stream << "\tGlobalSection(NestedProjects) = preSolution" << endl;
		for (int i = 0; i < vRelations.size(); ++i)
		{
			char szTmp[1024];
			sprintf_s(szTmp, 1024, "\t\t{%s} = {%s}", vRelations.at(i).first->strUUID.c_str(), vRelations.at(i).second->strUUID.c_str());
			stream << string(szTmp).c_str() << endl;
		}
		stream << "\tEndGlobalSection" << endl;
		stream << "EndGlobal" << endl;
	}

    void Save_vs2010(const char* szFileName, const TCHAR* ver)
    {
        ofstream stream(szFileName);

        stream << "Microsoft Visual Studio Solution File, Format Version 11.00" << endl;
        stream << "# Visual Studio 2010" << endl;

        stream << "Project(\"{2150E333-8FDC-42A3-9474-1A3956D46DE8}\") = \"" << ver << "\", \"" << ver << "\", \"{C18035AA-14CE-4361-95CA-724E74EF8DCB}\"" << endl;
        stream << "\tProjectSection(SolutionItems) = preProject" << endl;

        char szTmp[1024];
        for (int i = 0; i < vProjs.size(); ++i)
        {
            sprintf_s(szTmp, 1023, "%s = %s", vProjs.at(i)->strName.c_str(), vProjs.at(i)->strName.c_str());
            stream << string(szTmp).c_str() << endl;
        }

        stream << "\tEndProjectSection" << endl;
        stream << "EndProject" << endl;

        stream << "Global" << endl;
        stream << "\tGlobalSection(SolutionProperties) = preSolution" << endl;
        stream << "\t\tHideSolutionNode = FALSE" << endl;
        stream << "\tEndGlobalSection" << endl;
        stream << "EndGlobal" << endl;
    }

	void Load(const char* szFileName)
	{
		static string strExpress1 = "^Project\\(\"\\{([^\\}]*)\\}\"\\) = \"([^\"]*)\", \"([^\"]*)\", \"\\{([^\\}]*)\\}\"$";
		static regex expression1(strExpress1.c_str());

		ifstream stream(szFileName);
		if (stream.bad())
		{
			return;
		}

		string strLine;
		while (getline(stream, strLine))
		{
			if (strLine.empty())
			{
				continue;
			}

			cmatch match;
			if(!regex_match(strLine.c_str(), match, expression1))
			{
				continue;
			}

			if (match.size() != 5)
			{
				continue;
			}

			stProj* pProj = new stProj();
			pProj->strProjUUID = match[1].str();
			pProj->strName = match[2].str();
			pProj->strUUID = match[4].str();

			for_each(pProj->strName.begin(), pProj->strName.end(), tolower);

			vProjs.push_back(pProj);
		}
		stream.close();

		ifstream stream2(szFileName);
		static string strExpress2 = "\\s*\\{([^\\}]*)\\} = \\{([^\\}]*)\\}";
		static regex expression2(strExpress2.c_str());
		while (getline(stream2, strLine))
		{
			if (strLine.empty())
			{
				continue;
			}

			cmatch match;
			if(!regex_match(strLine.c_str(), match, expression2))
			{
				continue;
			}

			if (match.size() != 3)
			{
				continue;
			}

			stProj* pParent = GetProj(match[2].str());
			stProj* pSon = GetProj(match[1].str());

			if (!pParent || !pSon)
			{
				continue;
			}

			vRelations.push_back(make_pair(pSon, pParent));
		}

		stream2.close();
	}
};

void MakeSln(const TCHAR* path, const TCHAR* ver)
{
	std::vector< std::string > vfils;
	if (!GetFullPathFileList(vfils, path, ".dmp", false, false))
	{
		return;
	}

	stSln sln;
	for (size_t i = 0; i < vfils.size(); ++i)
	{
		size_t offset = vfils.at(i).rfind('\\');
		if(offset == string::npos)
		{
			offset = 0;
		}
		else
		{
			offset++;
		}
		sln.AddProj(vfils.at(i).substr(offset).c_str());
	}
	sln.Save_vs2005((string(path) + "\\dump.sln").c_str());
    sln.Save_vs2010((string(path) + "\\dump.sln").c_str(), ver);

	//ShellExecute(NULL, "open", (string(path) + "\\dump.sln").c_str(), NULL, NULL, SW_MAXIMIZE);

}

// CDumpAnalyzeApp 构造
CDumpAnalyzeApp::CDumpAnalyzeApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CDumpAnalyzeApp 对象
CDumpAnalyzeApp theApp;
FMiniDump theDump;
CDumpAnalyzeDlg* theManWnd();


// CDumpAnalyzeApp 初始化
BOOL CDumpAnalyzeApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("SNDA"));

	CDumpAnalyzeDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

extern CDumpAnalyzeDlg* theManWnd()
{
	return (CDumpAnalyzeDlg*)theApp.m_pMainWnd;
}

void CDumpAnalyzeApp::OnFileOpen()
{
	CFileDialog file(TRUE, ".dmp", "", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Dump Files(*.dmp)|*.dmp||");
	if (file.DoModal() != IDOK)
		return;

	theManWnd()->OpenDump(file.GetPathName());
}
extern const TCHAR* DAFormat(const TCHAR* fmt, ...);
void CDumpAnalyzeApp::OnAnalyze()
{
	TCHAR dir[MAX_PATH] = {0};

	BROWSEINFO BI;
	memset(&BI, 0, sizeof(BI));
	BI.hwndOwner = theManWnd()->GetSafeHwnd();
	BI.pszDisplayName = dir;
	BI.lpszTitle = "请选择DMP目录";
	BI.ulFlags = BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST IDLST = SHBrowseForFolder(&BI);
	if (!IDLST)
		return;
	if (!SHGetPathFromIDList(IDLST, dir))
		return;
	_tcscat_s(dir, _T("\\"));

	if(IDYES == MessageBox(NULL, "处理哪种类型文件\n\nSGZ(yes)\n\nDMP(no)", "提示", MB_YESNO))
		SGZ2DMP(dir);

	_AddVector vadd;
    _StrVector verinfo;
	__ClearRepeat(dir, vadd, verinfo);

	size_t n = 0;
	for (size_t i = 0; i < vadd.size(); ++i)
	{
		n += vadd[i].second.size();

		MakeSln(vadd.at(i).first.c_str(), verinfo[i].c_str());
	}
	AfxMessageBox(DAFormat(_T("分析结束：\n\n找到%d个不同版本号的DUMP文件\n\n共计%d个新的、且不同的DUMP文件"), vadd.size(), n));
}

struct DUMP_CACHE
{
	int		dwHitTimes;
	string	strFileName;
	ULONG32 m_uExceptionCode;           // Structured exception's code
	ULONG64 m_uExceptionAddress;        // Exception address
	string	m_strExceptionModule;	// Exception module;
	ULONG64 m_uExceptionOffsetFromModuleBase; // Exception offset address from module base
};

bool CompareDump(DUMP_CACHE* pCache, DUMP_INFO* pDump)
{
	if(pDump->m_uExceptionCode != pCache->m_uExceptionCode)
	{
		return false;
	}

	if(pDump->m_pExceptionModule && !pCache->m_strExceptionModule.empty())
	{
		if(pDump->m_pExceptionModule->m_strModuleName == pCache->m_strExceptionModule &&
			pDump->m_uExceptionOffsetFromModuleBase == pCache->m_uExceptionOffsetFromModuleBase)
		{
			return true;
		}
		return false;
	}

	if(pDump->m_uExceptionAddress == pCache->m_uExceptionAddress)
	{
		return true;
	}

	return false;
}

void CDumpAnalyzeApp::__ClearRepeat(const TCHAR* path, _AddVector& vadd, _StrVector& verinfo)
{
	// 找该目录下所有DUMP
	std::vector< std::string > vfils;
	if (!GetFullPathFileList(vfils, path, ".dmp", true, false))
		return;

	//char date[16] = {0};
	//char time[16] = {0};
	//_strdate_s(date);
	//_strtime_s(time);
	//date[2] = '-';
	//date[5] = '-';
	//time[2] = '.';
	//time[5] = '.';

	vadd.reserve(2048);

	// 转到上级目录
	std::string root = path;
	//root += "..\\";
	root = NormalizePath(root.c_str());

	std::vector< DUMP_CACHE > vDmpCache;
	vDmpCache.reserve(64);
	CString ver;
	theManWnd()->ClearUI();
	for (size_t i = 0; i < vfils.size(); ++i)
	{
		theDump.Close();
		if (!theDump.Open(vfils[i].c_str(), path, 0, false))
			continue;

		// 加载所需dump信息
		DUMP_INFO& DI = theDump.nDump;
		theDump.ReadException();
		theDump.bIsReadException = TRUE;
		//theDump.ReadThread(); // 如果要分析寄存器的话，需要加载线程信息
		theDump.ReadModule();
		theDump.ReadModule(true);
		theDump.bIsReadModule = TRUE;
		theDump.GetExceptionModuleInfo();
		//theDump.StackWalk(DI.m_uExceptionThreadId);

		std::string dst_dir = root;	// 目标存放dump的目录
		std::string del_dir;		// 目标存放重复dump的目录
		std::string ver_dir;		// 版本目录
		dst_dir += theDump.GetVerString(ver);
		dst_dir += "\\";
		ver_dir = dst_dir;
		del_dir = dst_dir + "__del\\";
		//dst_dir += date;
		//dst_dir += ".";
		//dst_dir += time;
		//dst_dir += "\\";
		//ConfirmDir(dst_dir);
		ConfirmDir(del_dir);

		bool find_it = false;
		for (size_t j = 0; j < vDmpCache.size(); ++j)
		{
			DUMP_CACHE& DC = vDmpCache[j];
			if(CompareDump(&DC, &DI))
			{
				// 移动到废弃库，不考虑该目录是否已经存在该dump文件与否
				find_it = true;
				theDump.Close();
				std::string dst = del_dir + ExtractFileName(vfils[i].c_str());
				CopyFile(vfils[i].c_str(), dst.c_str(), FALSE);
				vDmpCache[j].dwHitTimes++;
			}
		}
		if (!find_it)
		{
			std::string dst = dst_dir + ExtractFileName(vfils[i].c_str());

			//// 需要测试是否存在该文件
			//std::vector< std::string > dst_files;
			//GetFullPathFileList(dst_files, ver_dir.c_str(), ".dmp", true, false);
			//if (!dst_files.empty())
			//{
			//	for (size_t i = 0; i < dst_files.size(); ++i)
			//		dst_files[i] = ExtractFileName(dst_files[i]);
			//	if (std::find(dst_files.begin(), dst_files.end(), ExtractFileName(dst)) != dst_files.end())
			//		continue;
			//}

			// 保存dump
			DUMP_CACHE dc;
			dc.strFileName = dst;
			dc.dwHitTimes = 1;
			dc.m_uExceptionCode = DI.m_uExceptionCode;
			dc.m_uExceptionAddress = DI.m_uExceptionAddress;
			if(DI.m_pExceptionModule)
			{
				dc.m_strExceptionModule = DI.m_pExceptionModule->m_strModuleName;
			}
			dc.m_uExceptionOffsetFromModuleBase = DI.m_uExceptionOffsetFromModuleBase;
			vDmpCache.push_back(dc);
			theDump.Close();

			// 复制到DMP库
			CopyFile(vfils[i].c_str(), dst.c_str(), FALSE);

			// 保存成功结果
			_StrVector* vfile = 0;
			for (size_t k = 0; !vfile && k < vadd.size(); ++k)
			{
				if (vadd[k].first == dst_dir)
					vfile = &vadd[k].second;
			}
			if (!vfile)
			{
				vadd.push_back(std::make_pair(dst_dir, _StrVector()));
                verinfo.push_back(ver.GetString());
				vfile = &vadd[vadd.size()-1].second;
				vfile->reserve(512);
			}
			if (vfile)
				vfile->push_back(dst);
		}
	}

	for (size_t j = 0; j < vDmpCache.size(); ++j)
	{
		char szTmp[2048];
		sprintf_s(szTmp, 2048, "%s(%d).dmp",
			vDmpCache.at(j).strFileName.substr(0, vDmpCache.at(j).strFileName.rfind('.')).c_str(), 
			vDmpCache.at(j).dwHitTimes);
		MoveFile(vDmpCache.at(j).strFileName.c_str(), szTmp);
	}

	theDump.Close();
}
#ifndef __REPLAYMANAGER_H__
#define __REPLAYMANAGER_H__

#include "MMOSingleton.h"
#include "MMOUtil.h"
#include "net/message_handler.h"

#define M_ENABLE_RPMGR 0

#if M_ENABLE_RPMGR

#define LOG_KEY_MESSAGE 0

extern bool g_replayMode;

enum EReplayDataType
{
	ERDT_Network,
	ERDT_Input,
	ERDT_ScreenRes
};

enum ENetworkDataType
{
	ENDT_Recv,
	ENDT_Send
};

enum EInputDataType
{
	EIDT_Key,
	EIDT_Axis,
	EIDT_Char,
	EIDT_MousePosition
};

static TCHAR* g_inputDataTypeStr[] =
{
	TEXT("Key"),
	TEXT("Axis"),
	TEXT("Char"),
	TEXT("MousePosition")
};

struct FReplayData
{
	FReplayData() : Index(0), GameTime(0) {}
	UINT64 Index;
	DOUBLE GameTime;
	virtual EReplayDataType GetType() const = 0;
	
	virtual UBOOL Serialize(FArchive& archive)
	{
		archive << Index;
		archive << GameTime;
		return !archive.IsError();
	}

	virtual void Destroy() = 0;
	virtual void DispatchReplayData() = 0;

	virtual FString ToString() const = 0;

	virtual bool IsHiFrequencyData() const { return false; }
};

struct FReplayDataNetwork : public FReplayData
{
	FReplayDataNetwork() : DataType(ENDT_Recv), DataSize(0), DataBuffer(NULL) {}
	ENetworkDataType DataType;
	INT DataSize;
	char* DataBuffer;

	virtual EReplayDataType GetType() const
	{
		return ERDT_Network;
	}

	virtual UBOOL Serialize(FArchive& archive);

	virtual void Destroy()
	{
		if(DataBuffer)
		{
			delete[] DataBuffer;
			DataBuffer = 0;
		}

		delete this;
	}

	virtual void DispatchReplayData();

	virtual FString ToString() const;

	virtual bool IsHiFrequencyData() const
	{
		if (!DataBuffer || DataSize < (INT)sizeof(MESSAGE_HEADER))
			return false;
		MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)DataBuffer;

		if(pHeader->wMsg != g2c_frame_signal
			&& pHeader->wMsg != g2c_focus_hifreq_sync
			&& pHeader->wMsg != g2c_self_hifreq_sync
			&& pHeader->wMsg != g2c_region_hifreq_sync
			&& pHeader->wMsg != t2c_ping_ack
			&& pHeader->wMsg != g2c_ping_ack
			&& pHeader->wMsg != g2c_char_move)
		{
			return false;
		}

		return true;
	}
};

struct FReplayDataInput : public FReplayData
{
	FReplayDataInput()
		: DataType(EIDT_Key), MousePoint(0, 0), ControllerId(0), Key(NAME_None),
		  EventType(IE_Pressed), AmountDepressed(0), GamePad(FALSE), Delta(0), DeltaTime(0), Character(0) {}
	EInputDataType DataType;

	FIntPoint MousePoint;
	
	INT ControllerId;
	FName Key;
	EInputEvent EventType;
	FLOAT AmountDepressed;
	UBOOL GamePad;

	FLOAT Delta;
	FLOAT DeltaTime;

	TCHAR Character;

	virtual EReplayDataType GetType() const
	{
		return ERDT_Input;
	}

	virtual UBOOL Serialize(FArchive& archive);


	virtual void Destroy()
	{
		delete this;
	}

	virtual void DispatchReplayData();

	virtual FString ToString() const;

	virtual bool IsHiFrequencyData() const { return DataType == EIDT_Axis || DataType == EIDT_MousePosition; }
};


struct FReplayDataScreenRes : public FReplayData
{
	FReplayDataScreenRes() : ResX(0), ResY(0), IsFullScreen(FALSE) {}
	UINT ResX;
	UINT ResY;
	UBOOL IsFullScreen;

	virtual EReplayDataType GetType() const
	{
		return ERDT_ScreenRes;
	}

	virtual UBOOL Serialize(FArchive& archive);


	virtual void Destroy()
	{
		delete this;
	}

	virtual void DispatchReplayData();

	virtual FString ToString() const;
};


class ReplayManager : public SingletonMeyers<ReplayManager>
{
public:

	ReplayManager();
	virtual ~ReplayManager();

	void Init();
	void ShutDown();

	void Recode(FReplayData* newData);
	void Tick();

protected:

	typedef std::list<FReplayData*, M_UE3_STL_ALLOC(FReplayData*) > ReplayDataList;
	ReplayDataList _replayDataList;

	FArchive* _archive;

	//FIntPoint _initMousePt;
	//FIntPoint _initWndPos;

	void _ClearReplayData();
};

#endif

#endif

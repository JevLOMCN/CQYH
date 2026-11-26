#ifndef _MMOJIGSAWMANAGER_H_
#define _MMOJIGSAWMANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMOGameClasses.h"
#include "MMODataManager.h"

class MMOJigsawManager: public SingletonMeyers<MMOJigsawManager>
{
public:
	MMOJigsawManager();
	void Init();
	void Shutdown();

	void Update(float deltaTime);

	UTexture2D* CreateJigsawTexture( INT sizeX, INT sizeY );

	void SetOpenJigsawPanelDelegate(FScriptDelegate fun);

	BOOL IsBuildingTextrue();

	void CopyTargetTextureData(char *src,INT copySize = JIGSAW_DATA_SIZE);
	void CopyAnswerTextureData(char *src,INT Index , INT copySize = JIGSAW_ANSWER_DATA_SIZE);

	void OnRecieveJigsawMsg();

	void BuilJigsawTexture();

	void OnSelectAnswer(INT answer);

	UTexture2D* GetJigsawTexture();
	UTexture2D* GetAnswerTexture(INT nIndex);

	BOOL IsMessageButtonBarShowing() {return _HintJigsawMsgButtonIdx >= 0;}
	void RemoveMessageButtionBar();

	UINT32 GetDeadLineTime() {return _DeadLineTime;}
	void SetDeadLineTime(UINT32 fTime) {_DeadLineTime = fTime;}

	void SetBlockOffset(UINT8 blockX,UINT8 blocky) {_BlockX = blockX;_BlockY = blocky;}
	void GetBlockOffset(UINT8& oBlockX,UINT8& oBlockY) {oBlockX = _BlockX,oBlockY = _BlockY;}

	void OnRetCode(	INT32 nErrCode,INT32 nParam1);

private:

	UTexture2D*	pTargetTexture;
	UTexture2D*	pAnsTexture[JIGSAW_ANSWER_COUNT];

	char				_TargetTextureData[JIGSAW_DATA_SIZE];
	char				_AnswerTextureData[JIGSAW_ANSWER_COUNT][JIGSAW_ANSWER_DATA_SIZE];

	BOOL				isBuildingTexture;

	BOOL				isBuildDone;

	FScriptDelegate		_OpenJigsawPanelDelegate;
	INT					_HintJigsawMsgButtonIdx;

	UINT32				_DeadLineTime;

	UINT8				_BlockX;
	UINT8				_BlockY;
};

#endif
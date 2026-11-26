#include "stdafx.h"
#include "MMOHintButtonManager.h"
#include "MMOFacade.h"
#include "MMOHintButtonCommand.h"

static MMOHintButtonCommand_ArenaQueue Command_ArenaQueue;

MMOHintButtonManager::MMOHintButtonManager()
{
	const FString IconPath = GConfig->GetStr( TEXT("MMOGame.HintButtonIcon") , TEXT("ArenaQueueHintIcon") , GGameIni );
	_HintIconMap.insert( std::make_pair( EHintButtonType_ArenaQueueHint , IconPath )  );

	_HintCommandMap.insert( std::make_pair( EHintButtonType_ArenaQueueHint , &Command_ArenaQueue ) );
}

MMOHintButtonManager::~MMOHintButtonManager()
{

}

void MMOHintButtonManager::ClearAllButton()
{
	for ( INT i = 0 ; i < (INT)_HintPosVec.size() ; i++ )
	{
		MMOUI::GetUIManager()->delegateRemoveHintButton( i );
	}
	_HintPosVec.clear();
}

void MMOHintButtonManager::ClickButton( INT pos )
{
	if( pos >= 0 && (size_t)pos < _HintPosVec.size() )
	{
		HintButtonCommandMap::iterator itor = _HintCommandMap.find(_HintPosVec[pos]);
		if( itor != _HintCommandMap.end() && itor->second != NULL )
		{
			itor->second->Click();
		}
	}
}

void MMOHintButtonManager::AddNewButton( EHintButtonType type , INT time , UBOOL add  )
{
	HintIconMap::iterator itor = _HintIconMap.find(type);
	if( itor != _HintIconMap.end() )
	{
		INT pos = MMOUI::GetUIManager()->delegateAddNewHintButton( itor->second , time , add );
		if( _HintPosVec.size() == pos  )
		{
			_HintPosVec.push_back(type);
			//_HintCommandVec.push_back(command);
		}
		else
		{
			assert(0);
		}
		
	}
}

void MMOHintButtonManager::RemoveButton( EHintButtonType type )
{
	for ( INT i = 0 ; i < (INT)_HintPosVec.size() ; i++ )
	{
		if( _HintPosVec[i] == type )
		{
			
			MMOUI::GetUIManager()->delegateRemoveHintButton( i );
			_HintPosVec.erase( _HintPosVec.begin() + i );
			//_HintCommandVec.erase(i);
			break;
		}
	}


}
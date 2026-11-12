/********************************************************************
Created:	2013/06
Filename: 	UUIKeyMapManager
file ext:	cpp
Author:		hemu@snda
Purpose:	control keyboard input
physical key->Slot->actual Cmd
*********************************************************************/

#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FSMState.h"

IMPLEMENT_CLASS(UUIKeyMapManager)
IMPLEMENT_CLASS(UUICommandFactory)
//IMPLEMENT_CLASS(UIUICommand)
IMPLEMENT_CLASS(UUICmd_HotKey)
//UUIKeyMapManager

void UUIKeyMapManager::IniServerSlotBind()
{
    //Default initial Locked
    for( int j = ECK_NormalAtk ; j <= ECK_Slot12 ; ++j )
    {
        UnBindingSlot( j, true );
    }	
}

void UUIKeyMapManager::OnSlotKey( int slotId , UBOOL bRelease)
{
    if( !UICmdList(slotId).IsLocked && NULL != UICmdList( slotId ).Cmd )
    {
		FSM::Ctrl.KeyType = KT_ELSE;       
        if( !bRelease )
        {
            UICmdList( slotId ).Cmd->eventUse();
        }
        else
        {
            UICmdList( slotId ).Cmd->eventOnRelease();
        }
    }
}

void UUIKeyMapManager::BindingSlot( int slotId , BYTE slottype ,int abilityid )
{
    if( !PC )
    {
       return;
    }

    if( slotId >= UICmdList.Num() || slotId < 0 )
    {
        return;
    }

    if( slottype == UICT_None )
    {
        UICmdList(slotId).Cmd = NULL;
        return;
    }

    UIUICommand* UICmd;

    UICmd = CmdFactory->eventCreateCommand( slottype );
    UICmd->eventSetData( abilityid, slotId,  PC );	

    UICmdList(slotId).Cmd = UICmd;
    UICmdList(slotId).IsLocked = false;
}

void UUIKeyMapManager::UnBindingSlot( int slotId, UBOOL isLocked )
{
    if( slotId >= UICmdList.Num() || slotId < 0 )
    {
        return;
    }

    UICmdList(slotId).Cmd = NULL;
    UICmdList(slotId).IsLocked = isLocked;
}

UBOOL UUIKeyMapManager::IsSlotLocked( int slotId )
{
    if( UICmdList.Num() < slotId )
    {
        return true;
    }
    return UICmdList(slotId).IsLocked ;
}

int UUIKeyMapManager::GetCmdIDBySlotID( int slotId )
{
    UIUICommand* cmd;
    if( UICmdList.Num() < slotId )
    {
        return -1;
    }

    cmd = UICmdList(slotId).Cmd;

    if( cmd != NULL )
    {
        if( cmd->GetID() >=0 )
            return cmd->GetID();
    }
    return -1;
}
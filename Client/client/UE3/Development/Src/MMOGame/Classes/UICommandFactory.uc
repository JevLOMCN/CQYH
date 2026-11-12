//

class UICommandFactory extends Object
	native(UI);

//conflicts with CT in Engine
enum UICommandType
{
	UICT_UseAbility,
	UICT_UseItem,
	UICT_UseMount,
	UICT_HotKey,
	UICT_None,
};

event IUICommand CreateCommand( UICommandType type)
{
	switch( type )
	{
	case UICT_UseAbility:
		return new class'UICmd_Ability';
		break;
	case UICT_UseItem:
		return new class'UICmd_UseItem';
		break;
	case UICT_UseMount:
		return new class'UICmd_UseMount';
		break;
	case UICT_HotKey:
		return new class'UICmd_HotKey';
		break;
	default:
		return none;
		break;
	}

}

DefaultProperties
{
	
}

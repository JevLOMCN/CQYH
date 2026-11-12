class MMOGFXPlayer_EquipEnhance extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIEquipEnhance _EquipEnhance;

function RegisterEquipEnhance( MMOUIEquipEnhance mc )
{
	_EquipEnhance = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_EquipEnhance);
	
	//self.OnPostAdvance = _EquipEnhance.TickForTestInteractNPCDistance;
}

// event UnRegister()
// {
// 	super.UnRegister();
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_EquipEnhance);
// 	MMOUI::RegisterMoviePlayer(class'MMOUIManager'.default.MoviePlayer_EquipEnhance);
// }

////Equipment enhancement
//function ToggleEquipEnhance()
//{
//	if( !_EquipEnhance.IsVisible ){
//		_EquipEnhance.NativeShow( true );
//	}
//	else{
//		_EquipEnhance.NativeShow( false );
//	}
//}

//
function ASHideEquipEnhance()
{
	_EquipEnhance.NativeShow( false );
}

//
function ASMakeEnhanceItem()
{
	_EquipEnhance.MakeEnhanceItem();
}

//
function ASTransferEnhanceItem()
{
	_EquipEnhance.TransferEnhanceItem();
}

//
function ASUpdateEquipEnhancePanel( int iTab )
{
	_EquipEnhance.ShowDefaultContent( iTab );
}
//end
//
DefaultProperties
{
	MovieInfoStr="UI_EquipEnhance.EquipEnhance"
	Priority=5
}

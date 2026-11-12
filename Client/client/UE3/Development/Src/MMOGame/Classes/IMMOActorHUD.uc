//
// HUD interface, Actors that need to show a HUD can implement this interface
//
interface IMMOActorHUD
	native;

native function bool DrawMyHUD( MMOHud H );
native function DrawHUDPostGFx( HUD H);

/** whether to only draw the HUD if the Pawn whose viewport we're drawing is the Owner of this Actor */
function bool OnlyDrawHUDForOwner();
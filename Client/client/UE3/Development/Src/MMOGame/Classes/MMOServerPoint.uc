//MMOServerPoint
//song@snda
//Used for server coordinate marker object extension

class MMOServerPoint extends PathNode implements(IUniqueID)
	placeable
	native
	hidecategories(NavigationPoint,VehicleUsage,Display,Attachment,Collision,Physics,Advanced,Debug,MMO,Object,Mobile)
	ClassGroup(__Server__);


var() string		DisplayName;	//limited to 8 characters
var (MMOServer)editinline int ServerUniqueID;
var (MMOServer)editinline int Range;

native function InitUniqueID();
native function INT GetUniqueID();
native function SetUniqueID(int id);

cpptext
{
	virtual void PostDuplicate() { Super::PostDuplicate(); SetUniqueID(-1); InitUniqueID(); }
#if WITH_EDITOR
	virtual void CheckForErrors(){}
#endif
}

DefaultProperties
{
	Begin Object Name=Arrow
		ArrowSize=5		
	End Object
}

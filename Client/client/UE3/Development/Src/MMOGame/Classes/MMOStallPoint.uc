
class MMOStallPoint extends PathNode
	placeable
	native
	hidecategories(NavigationPoint,VehicleUsage,Display,Attachment,Collision,Physics,Advanced,Debug,MMO,Object,Mobile)
	ClassGroup(__Nav__);

cpptext
{
	#if WITH_EDITOR
	virtual void CheckForErrors()
	{
		// No need to check
	}
	#endif
}

DefaultProperties
{
}

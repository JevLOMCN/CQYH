//MMOServerToggleDoorActor
//hemu@snda
//Used for doors in scene interaction
//Involves collision; the server needs to fill in Grid, handled separately here

class MMOServerToggleDoorActor extends MMOServerToggleActor 
	native
	ClassGroup(__Server_Old__);

//DoorId marker
var(MMOServerToggleActor)  int                   DoorId           <DisplayName="ID">;

cpptext
{
	virtual INT GetServerID(){ return DoorId; }
	virtual INT GetObjectServerType() const { return (INT)kotDoor; }
}

DefaultProperties
{
	DoorId=1
}

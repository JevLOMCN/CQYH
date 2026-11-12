//
// This is a global info storage class
//
class MMOGameReplicationInfo extends GameReplicationInfo
	config(MMOGameReplicationInfo)
	dependson(MMOGameState)
	native;

var int testCounter;


simulated function PostBeginPlay()
{	
	Super.PostBeginPlay();
	//`log("__________ MMOGameReplicationInfo::PostBeginPlay");
}

event Tick( float DeltaTime)
{
	
	//testCounter++;
	//`log("__________ MMOGameReplicationInfo::PostBeginPlay"@testCounter);
}

DefaultProperties
{
	//bTravel = true
	testCounter=0
}
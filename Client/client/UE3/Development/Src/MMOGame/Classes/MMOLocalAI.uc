

/** Automated local AI handling, simulates a player doing routine operations */

class MMOLocalAI extends Object
    config(MMOLocalAI)
	native
	dependson(TypeDefine);

// Single quest
struct native LOCAL_AI_TASK
{
    var ELocalAICheckType check_type;
    var int task_id;
};

// Login name and password (if set, this feature is enabled, otherwise disabled)
var config string UserName;
var config string Password;
var config string ConnectIP;
// Force creating a new character for testing
var config bool NewPlayer;
// Force starting from the specified main quest
var config int TaskStartID;
// How many quests to do
var config int TaskDoCount;
// Whether in detailed quest mode
var config bool TaskDetailMode;

// Whether this application is running
var bool AIWorking;
// Quest list
var array< LOCAL_AI_TASK > WorkTask;

native function Run();                      // Transaction handling logic
native function OnNetClose();               // When network closes
native function OnLogoEnd();                // Startup LOGO playback finished
native function OnLoginResult(bool ret);    // Login result
native function OnCreateRole();             // Create character
native function OnMapLoaded();              // Map loading finished
native function OnTaskState();              // Quest state changed

cpptext
{
	static class UMMOLocalAI* Instance();
}

DefaultProperties
{
    AIWorking = false;
}

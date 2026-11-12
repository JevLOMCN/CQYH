//
// Linked object interface
//
interface IMMOLinkageObject;

enum ELinkageMovementType
{
	LMT_Move <DisplayName="Move">,
	LMT_Rotate <DisplayName="Rotate">
};

// Completion ratio of the parent object
function UpdateFromSource(float parentPercent);

function float GetCurrentMovedPercent();

function ELinkageMovementType GetCurrentMovementType();

function SetCurrrentMovementType(ELinkageMovementType tp);

function PawnEnteredVolume(Pawn P);

function PawnLeavingVolume(Pawn P);

function SkeletalMeshComponent GetMesh();

function name GetCurrentState();

function bool IsAutoReset();

function bool CanInteractionNow();



function PlayStartSound();

function PlayLoopSound();

function StopLoopSound();

function PlayEndSound();
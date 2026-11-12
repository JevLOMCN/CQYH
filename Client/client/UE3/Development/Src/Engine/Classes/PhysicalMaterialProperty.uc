//
// Material sound effect properties
//
class PhysicalMaterialProperty extends PhysicalMaterialPropertyBase
	native(Physics)
	collapsecategories
	hidecategories(Object)
	editinlinenew;

enum EPhyMatType
{
	EPMAT_Default <DisplayName="Default">,
	EPMAT_Stone <DisplayName="Stone">,
	EPMAT_Dirt <DisplayName="Dirt">,
	EPMAT_Gravel <DisplayName="Gravel">,
	EPMAT_Sand <DisplayName="Sand">,
	EPMAT_Deck <DisplayName="Deck">,
	EPMAT_Wood <DisplayName="Wood">,
	EPMAT_Grass <DisplayName="Grass">,
	EPMAT_Leavs <DisplayName="Leavs">,
	EPMAT_Bush <DisplayName="Bush">,
	EPMAT_Mud <DisplayName="Mud">,
	EPMAT_Swamp <DisplayName="Swamp">,
	EPMAT_Water <DisplayName="Water">,
	EPMAT_Snow <DisplayName="Snow">,
	EPMAT_Ice <DisplayName="Ice">,
	EPMAT_Carpet <DisplayName="Carpet">,
};

var() EPhyMatType MaterialType;

DefaultProperties
{

}
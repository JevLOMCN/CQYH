class SeqAct_SwitchPlayerClassGender extends SequenceAction
	native;

// this value is used in the editor to test which output to use
var() ECHAR_CLASS_TYPE PIETestClassType;
var() EGenderType PIETestGender;

cpptext
{
	virtual void Activated();
}

DefaultProperties
{
	ObjName = "Switch Player Class & Gender"
	ObjCategory = "MMO Client"

	InputLinks.Empty
	InputLinks(0) = ( LinkDesc = "In" )

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Warrior_M")
	OutputLinks(1)=(LinkDesc="Warrior_F")
	OutputLinks(2)=(LinkDesc="Mage_M")
	OutputLinks(3)=(LinkDesc="Mage_F")
	OutputLinks(4)=(LinkDesc="Rogue_M")
	OutputLinks(5)=(LinkDesc="Rogue_F")
	OutputLinks(6)=(LinkDesc="Archer_M")
	OutputLinks(7)=(LinkDesc="Archer_F")
	OutputLinks(8)=(LinkDesc="Error")
	OutputLinks(9)=(LinkDesc="Taoist_M")
	OutputLinks(10)=(LinkDesc="Taoist_F")

	VariableLinks.Empty

	PIETestClassType=E_cls_Warrior
	PIETestGender=EGT_Male
}


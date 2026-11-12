//
// Use ability
//
class MMOStatusProc_SetMaterialAphla extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	dependson(MaterialInterface)
	hidecategories(Object);

var()	array<MaterialInterface>	Materials;
var MaterialInstanceConstant TheHealthHUDMatTemplate;
var native float Value;
var bool Init;

event OnExecute(MMOActor actor)
{
//	local int i;
//	local int nNum;
//	local MaterialInterface mi;
//	local MaterialInstanceTimeVarying mitv;
//	local MaterialInstanceConstant mic;
////	local Object Obj;
////	local String matTransName;
////	local MaterialInstance matOrig;
//	
//	//value = actor.ActorAttributes.GetCurHp()/actor.ActorAttributes.GetMaxHp();
//	
//	value = value + 0.1;
//	if(value > 10)
//		value = value - 20;
//	//local FMaterialRenderContext Context;
//
//	nNum = actor.Mesh.GetNumElements();
//	//if(!Init)
//	//{
//	//	for (i = 0; i < nNum; i++)
//	//	{
//	//		mi = actor.Mesh.GetMaterial(i);
//	//		matOrig = MaterialInstance( mi );
//	//		if( matOrig == none )
//	//		{
//	//			`Log( "[MMO Debug] Can't create opacity material for mat"@mi.Name );
//	//			continue;
//	//		}
//
//	//		Obj = actor.Mesh.GetMaterial(i).Outer;
//	//		matTransName = String( Obj.Name );
//	//		while( Obj.Outer != none )
//	//		{
//	//			matTransName = String( Obj.Outer.Name ) $ "." $ matTransName;
//	//			Obj = Obj.Outer;
//	//		}
//	//		matTransName = String( matOrig.GetMaterial().Name );
//	//		
//	//		`log( "MaterialName: "@matTransName );
//	//		`log( "Material: "@actor.Mesh.GetMaterial(i) );
//	//		`log( "Overwrite material: "@actor.Mesh.Materials[i] );
//	//		mitv = MaterialInstanceTimeVarying(mi);
//	//		mic = MaterialInstanceConstant(mi);
//	//		if(mic != none)
//	//		{
//	//			mic = new class'MaterialInstanceConstant';
//	//			mic.SetParent(actor.Mesh.GetMaterial(i));
//	//			actor.Mesh.SetMaterial(i, mic);
//
//	//		}
//	//		else if(mitv != none)
//	//		{
//	//			mitv = new class'MaterialInstanceTimeVarying';
//	//			mitv.SetParent(actor.Mesh.GetMaterial(i));
//	//			actor.Mesh.SetMaterial(i, mitv);
//
//	//		}
//	//	}
//	//	Init= true;
//	//}
//	for (i = 0; i < nNum; i++)
//	{
//		mi = actor.Mesh.GetMaterial(i);
//		mitv = MaterialInstanceTimeVarying(mi);
//		mic = MaterialInstanceConstant(mi);
//		//actor.Mesh.SetScalarParameterValue('HPPrecent', value);
//		if(mic != none)
//		{
//			mic.SetScalarParameterValue('HPPercent', value);
//			//`log("________________ SetColor MaterialInstanceConstant" @mi @clrl.A @clrl.R @clrl.G @clrl.B);
//
//		}
//		else if(mitv != none)
//		{
//			//`log("________________ SetColor MaterialInstanceTimeVarying" @mi @mitv.VectorParameterValues.Length);
//			mitv.SetScalarParameterValue('HPPercent', value);
//		}
//		else
//		{
//			//`log("________________ SetColor " @mi);
//		}
//
//		//`Log( "SetScalarParameterValue HPPrecent" @value);
//		//TheHealthHUDMatTemplate = actor.Mesh.GetMaterial(i);
//		//QuadMIC.SetScalarParameterValue("HPPrecent", value);
//	}
}
native function Update(MMOActor actor);



defaultproperties
{
	value = 0;
	Init = false;
}
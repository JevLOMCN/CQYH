
/**
 * Character material controller
 * Mainly implements control of dynamic effect parameters, such as glow and translucency
 * */

class MMOActorMaterialController extends Object 
	native;

var private MeshComponent Mesh;

struct native MeshIdMaterialMap
{
	var int		Id;
	var MaterialInterface	OriginalMaterial;		// Original material of the model
	var MaterialInstanceConstant	OpacityMaterial;	// Translucent material
	var MaterialInstance   NewMaterial;            // Replacement material
	var MaterialInstanceTimeVarying   TimeVaringMaterial;     // Material used for material parameters
};

// Current material effect
var EMaterialEffectType EffectType;
// Material effect ID, to uniquely distinguish effects
var int EffectId;

var array<MeshIdMaterialMap> MeshMaterials;

var bool  IsTransparent;
var bool  IsFading;
var float CurrentOpacity;
var float DestOpacity;
var float FadeSpeed;	// fade rate per second. +: fade in -:fade out

var array<Material> MeshTranslucencyMaterials;

event SetMaterials( array<Material> materials )
{
	MeshTranslucencyMaterials = materials;
}

event Init( MeshComponent MeshComp, optional bool initOpacityMat=false )
{
	local int i,idx;
	local MaterialInstanceTimeVarying mitv;
	//local MaterialInstanceConstant mic;
	//local MeshIdMaterialMap idMatItem;

	// check if the mesh has already been set
	if( Mesh != none )
	{
		//`Log( "[MMO Debug]Set Mesh duplicated - MaterialController."@meshComp.Name );
		//return;
		// actors can change their meshes, why can't the material controllers?
		for( i = 0; i < Mesh.GetNumElements(); ++i )
		{
			Mesh.SetMaterial( i, none );
		}
		Mesh = none;
		MeshMaterials.Remove( 0, MeshMaterials.Length );
	}
	else if(meshComp == none)
	{
		`log("[MMO] Material controller init failed, input mesh is NULL!" @ self);
		return;
	}

	Mesh = meshComp;

	// Initialize and save materials
	for(i = 0; i < Mesh.GetNumElements(); ++i)
	{
		// initial fade in/out material instances
		idx = MeshMaterials.Add(1);
		MeshMaterials[idx].Id = i;
		MeshMaterials[idx].OriginalMaterial = Mesh.GetMaterial(i);
		`log("idx"$idx$" owner="$MeshComp.Owner$"  OriginalMaterial="$MeshMaterials[idx].OriginalMaterial);

		if(initOpacityMat)
		{
			MeshMaterials[idx].OpacityMaterial = CreateOpacityMaterial( Mesh.GetMaterial(i) );			
		}

		//`log("OSCAR:_X_X_X__X_X_X_X_X_X_X" @i @Mesh.GetMaterial(i));

		mitv = new class'MaterialInstanceTimeVarying';
		mitv.SetParent(Mesh.GetMaterial(i));
		Mesh.SetMaterial(i, mitv);
	}

	IsTransparent = false;
	IsFading = false;
	CurrentOpacity = 1.f;
	DestOpacity = 1.f;
	FadeSpeed = 0.f;
}

// Create the corresponding translucent blend version for the given material
function MaterialInstanceConstant CreateOpacityMaterial( MaterialInterface mat )
{
	local String matTransName;
	local String matShortTransName;
	local Material matTrans;
	local MaterialInstance matOrig;
	local MaterialInstanceConstant mic;
	local Object Obj;
	local int i;

	if( mat == none )
	{
		`Log( "[MMO Debug] Can't create opacity material for None" );
		return none;
	}

	// the mesh components must use material instances, not materials
	// or it won't be possible to create so many fade-in/out materials for them
	matOrig = MaterialInstance( mat );
	if( matOrig == none )
	{
		`Log( "[MMO Debug] Can't create opacity material for mat"@mat.Name );
		return none;
	}

	// load fade in/out material resource
	Obj = matOrig.GetMaterial().Outer;
	matTransName = String( Obj.Name );
	while( Obj.Outer != none )
	{
		matTransName = String( Obj.Outer.Name ) $ "." $ matTransName;
		Obj = Obj.Outer;
	}
	
	matShortTransName = String(matOrig.GetMaterial().Name) $ "_FadeInOut";
	matTransName = matTransName $ "." $ String(matOrig.GetMaterial().Name) $ "_FadeInOut";

	//`Log( "[MMO Info] fade in-out material name:"@matTransName );
	for ( i = 0 ; i < MeshTranslucencyMaterials.Length ; ++i )
	{
		//MeshTranslucencyMaterials[i]->GetName( tempName )
	    if ( string( MeshTranslucencyMaterials[i].Name ) == matShortTransName )
			matTrans = MeshTranslucencyMaterials[i];
	}

	if ( matTrans == none )
		matTrans = Material( DynamicLoadObject( matTransName, class'Material' ) );

	if( matTrans == none )
	{
		`Log( "[MMO Debug]Load material failed."@matTransName );
		return none;
	}

	// create material instance
	mic = new class'MaterialInstanceConstant';
	mic.SetParent( matTrans );
	
	if(MaterialInstanceConstant(matOrig) != none)
	{
		mic.CopyAllParameters(MaterialInstanceConstant(matOrig));
	}

	// copy parameters
	//CopyMaterialInstanceParameters( matTrans, mic, matOrig );

	return mic;
}

native final function CopyMaterialInstanceParameters( Material DestMat, MaterialInstance DestMatInst, MaterialInstance SrcMatInst );

// Set color parameter value
function SetColor(name parameterName, LinearColor clrl)
{
	local int elemCnt;
	local int i;
	local MaterialInterface mi;
	local MaterialInstanceTimeVarying mitv;
	local MaterialInstanceConstant mic;
	//local LinearColor clrl;
	//local VectorParameterValueOverTime pvo;
	//local Material mat;

	//`log("_______ Set Mesh" @Mesh);

	elemCnt = Mesh.GetNumElements();
	//clrl = ColorToLinearColor(clr);
	
	
	for(i = 0; i <elemCnt; ++i)
	{
		mi = Mesh.GetMaterial(i);
		mitv = MaterialInstanceTimeVarying(mi);
		mic = MaterialInstanceConstant(mi);
		
		//`log("_______ XXXX root:" @GetRootParentMaterial(mi));

		if(mic != none)
		{
			mic.SetVectorParameterValue(parameterName, clrl );
			//`log("________________ SetColor MaterialInstanceConstant" @mi @clrl.A @clrl.R @clrl.G @clrl.B);

		}
		else if(mitv != none)
		{
			//`log("________________ SetColor MaterialInstanceTimeVarying" @mi @mitv.VectorParameterValues.Length);
			mitv.SetVectorParameterValue(parameterName, clrl );
		}
		else
		{
			//`log("________________ SetColor " @mi);
		}
	}
}

// Set glow color, produces effects like poison, charging
function SetImpactEmissiveColor(LinearColor clrl)
{
	SetColor('ImpactEmissive', clrl);
}

// Play dynamic glow
function PlayAnimImpactEmissiveColor(name schemeName, optional bool loop = false)
{
	local int elemCnt;
	local MaterialInterface mi;
	local MaterialInstanceTimeVarying mitv;
	local int i;
	local VectorParameterValueOverTime pvo;
	//local Material mat;

	elemCnt = Mesh.GetNumElements();
	
	
	for(i = 0; i <elemCnt; ++i)
	{
		mi = Mesh.GetMaterial(i);
		mitv = MaterialInstanceTimeVarying(mi);
		
		//`log("_______XXXX root:" @GetRootParentMaterial(mi));
		if(mitv != none)
		{
			//`log("________________set impact color MaterialInstanceTimeVarying" @mi @mitv.VectorParameterValues.Length);
			//mitv.SetVectorParameterValue(ImpactEmissiveColorName, clrl );
			//mitv.SetDuration(20);
			// TODO:TODO: need to extend source code, add SchemeName, support multiple sets of animation parameters
			mitv.SetVectorStartTime(schemeName, 0.0f);
			mitv.SetDuration(mitv.GetMaxDurationFromAllParameters());
			foreach mitv.VectorParameterValues(pvo)
			{
				//if(pvo.SchemeName == xxx")
				if(pvo.ParameterName == schemeName)
				{
					pvo.bLoop = loop;
					pvo.bAutoActivate = false;
					pvo.bNormalizeTime = false;
				//pvo.CycleTime = 10;
				//mitv.GetMaxDurationFromAllParameters();
					//`log("_______" @pvo.CycleTime);
					break;
				}
			}
			//mitv.SetVectorStartTime(ImpactEmissiveColorName, 0);
		}
		else
		{
			//`log("________________PlayAnimImpactEmissiveColor " @mi);
		}
	}
}

// Set model to translucent material
native final function SetOpacity(float percent);
native final function SetTransparentMaterial( bool bTransparent );
native final function SetOpacityPercent( float percent );

// Play translucent fade-in
native final function PlayOpacityFadeIn( optional float fadeInTime = 0.2 );

// Play translucent fade-out
native final function PlayOpacityFadeOut( optional float fadeOutTime = 0.2 );

function ClearOpacityEffect()
{
	if(IsFading)
	{
		IsFading = false;
		SetTransparentMaterial( false );
	}
}

// Set material skin color, only for materials with skin channel enabled (UseSkinClothColor is 1)
function SetSkinColor(LinearColor clrl)
{
	//`log("________________ SetSkinColor ");
	SetColor('SkinColor', clrl);
}

// Set material clothes color, only for materials with skin channel enabled (UseSkinClothColor is 1)
function SetClothColor(LinearColor clrl)
{
	SetColor('ClothColor', clrl);
}

native final function NativeTick(float deltaTime);

simulated event Tick( float DeltaTime )
{
	NativeTick(deltaTime);
}

DefaultProperties
{
	EffectId=-1
}

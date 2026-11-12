//
// activate parameter effects of TimeVarying material
//
class AnimNotify_ActiveMaterialParameter extends AnimNotify_Scripted
	;

var(MMO) array<name> MatParaNameList;

event Notify( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	local Pawn pawn;
	local StaticMeshActor sma;
	local SkeletalMeshActor ska;
	local MeshComponent mesh;
	local int i;
	local MaterialInterface mat;
	local MaterialInstanceTimeVarying mitv;

	`log("OSCAR:________ AnimNotify_ActiveMaterialParameter" @Owner);

	pawn = Pawn(Owner);
	if(pawn != none)
	{
		mesh = pawn.Mesh;
	}

	sma = StaticMeshActor(Owner);
	if(sma != none)
	{
		mesh = sma.StaticMeshComponent;
	}

	ska = SkeletalMeshActor(Owner);
	if(ska != none)
	{
		mesh = ska.SkeletalMeshComponent;
	}

	if(mesh != none)
	{
		for(i = 0; i < mesh.GetNumElements(); ++i)
		{
			mat = mesh.GetMaterial(i);
			if(mat.IsA('MaterialInstanceTimeVarying'))
			{
				mitv = new class'MaterialInstanceTimeVarying';
				mitv.SetParent(mat);
				mitv.SetDuration(mitv.GetMaxDurationFromAllParameters());
				mesh.SetMaterial(i, mitv);
			}
		}
	}
}

event NotifyEnd( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
}


DefaultProperties
{
	
}

/*=============================================================================
	NvApexCommands.cpp : Manages serializing, deserializing, and management of APEX assets.
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

// This code contains NVIDIA Confidential Information and is disclosed
// under the Mutual Non-Disclosure Agreement.
//
// Notice
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright 2009-2010 NVIDIA Corporation. All rights reserved.
// Copyright 2002-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright 2001-2006 NovodeX. All rights reserved.

#include "EnginePrivate.h"
#include "EngineMeshClasses.h"
#include "UnNovodexSupport.h"

#if WITH_NOVODEX

#include "NvApexManager.h"

#if WITH_APEX

#include "NxParamUtils.h"
#pragma warning(disable:4996)

#include "NvApexCommands.h"
#include "NvApexScene.h"
#include "NvApexResourceCallback.h"

#include <NxApexSDK.h>
#include <NxUserRenderResourceManager.h>
#include <NxResourceCallback.h>
#include <NxModuleDestructible.h>
#include <NxModuleClothing.h>
#include <NxRendermeshAsset.h>
#include <NxDestructibleAsset.h>
#include <NxClothingAsset.h>
// NVCHANGE_BEGIN: DJS - APEX particle support
#if WITH_APEX_PARTICLES
#include <NxApexEmitterAsset.h>
#include <NxGroundEmitterAsset.h>
#include <NxImpactEmitterAsset.h>
// NVCHANGE_BEGIN: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
#if WITH_APEX_FLUID_IOS
#include <NxFluidIosAsset.h>
#include <NxModuleFluidIos.h>
#endif // WITH_APEX_FLUID_IOS
// NVCHANGE_END: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
#include <NxBasicIosAsset.h>
#include <NxBasicIosActor.h>
#include <NxIofxAsset.h>
#endif
// NVCHANGE_END: DJS - APEX particle support
#include <NxActorDesc.h>
#include <NxStream.h>
#include <NxModuleDestructible.h>
#include <NxDestructibleAsset.h>
#include <NxDestructiblePreview.h>
#include <NxDestructibleActor.h>
#include <NxModuleClothing.h>
#include <NxClothingAsset.h>
#include <NxClothingPreview.h>
#include <NxClothingActor.h>
#include <NxClothingAssetAuthoring.h>
#include <NxParameterized.h>
#include <NxDebugRenderable.h>
#include <NxScene.h>
#include <NxUserOpaqueMesh.h>

#include "NvApexScene.h"
#include "NvApexRender.h"
#include "NvApexScene.h"
#include "NxSerializer.h"


using namespace physx::apex;


FIApexCommands *GApexCommands=0;




namespace NI_APEX_SUPPORT
{

NxResourceProvider *GNRP=0;


//*********** end of temporary named resource provider.

static const char *lastDot(const char *str)
{
  const char *ret = NULL;
  while  ( str && *str )
  {
    str = strchr(str,'.');
    if ( str )
    {
      ret = str;
      str++;
    }
  }
  return ret;
}

class ApexAsset;

typedef NxArray< FIApexActor * > TApexActorVector;
typedef NxArray< physx::PxMat44 > TPxMat44Vector;
typedef NxArray< ApexAsset * > TApexAssetVector;
typedef NxArray< FIApexScene *> TApexSceneVector;
typedef NxArray< UApexAsset * > TUApexAssetVector;


physx::apex::NxApexAssetAuthoring* ApexAuthoringFromAsset(physx::apex::NxApexAsset* Asset, physx::apex::NxApexSDK* ApexSDK);

class ApexAsset : public FIApexAsset, 
					public NxParameterized::NamedReferenceInterface
#if WITH_APEX_PARTICLES
					,public ApexEditNotify
#endif
{
public:
#if WITH_APEX_PARTICLES
	ApexAsset(const char *fname,ApexAssetType type,UApexAsset *ue3Object,UApexGenericAsset *child);
// NVCHANGE_BEGIN: HL - Apex Emitter Asset Copy
	ApexAsset(const char *fname,ApexAssetType type,UApexAsset *ue3Object,UApexGenericAsset *child,NxParameterized::Interface* iface);
// NVCHANGE_END: HL - Apex Emitter Asset Copy
#endif
	ApexAsset(const char *fname,const void *mem,physx::PxU32 dlen,const void *params,const char *originalName,UApexAsset *ue3Object);
	virtual ~ApexAsset(void);

#if WITH_APEX_PARTICLES
	virtual UBOOL  OnPostPropertyEdit(NxParameterized::Interface *iface,NxParameterized::Handle &handle,const char *Tweakable)
	{
		if ( ApexAssetObject )
		{
			Object->Modify();
			Object->MarkPackageDirty();
			bTweaked = true;
			for (TApexActorVector::const_iterator i=Actors.begin(); i!=Actors.end(); ++i)
			{
				FIApexActor *Act = static_cast<FIApexActor *>(*i);
				if ( Act )
				{
					Act->ProcessTweakable(iface,Tweakable);
				}
			}
			if ( Tweakable == NULL )
			{ 
				if ( !bEdited )
				{
					NewInterface = iface; // preserve the 'new' interface we are copying from the editor DLL
				}
				bEditPending = TRUE;
				TweakInterface = NULL;
			}
			else
			{
				TweakInterface = iface; // save the interface pointer for tweaked changes...
			}
#if !WITH_APEX_SHIPPING
			GApexCommands->NotifyAssetModified(this);
#endif
		}
		return TRUE;
	}

	void ClearEdit(void)
	{
		bEdited = FALSE;
		bEditPending = FALSE;
		NewInterface = NULL;
		TweakInterface = NULL;
	}

	virtual UBOOL  NotifyEditorClosed(NxParameterized::Interface *iface) 
	{
		UBOOL ret = !bEdited;
		ApexEditInterface = NULL;
		if ( (bTweaked || bEditPending ) && ApexAssetObject )
		{
			ret = FALSE;
			NotifyAssetGoingAway();
			
			// sometimes iface == ApexAsset's parameterized here causing problems...
			const NxParameterized::Interface *assetInterface = ApexAssetObject->getAssetNxParameterized();
			
			if ( bEdited || iface == assetInterface )
			{
				NxParameterized::Interface *ifc = ApexAssetObject->releaseAndReturnNxParameterizedInterface();
				check( ifc == iface ); // must be the same NxParameterized!
			}
			else
			{
				GApexManager->GetApexSDK()->releaseAsset(*ApexAssetObject);
			}
			ApexAssetObject = GApexManager->GetApexSDK()->createAsset(iface,Name);
			NotifyAssetImport();
			bTweaked = FALSE;
			bEditPending = FALSE;
			NewInterface = NULL;
			TweakInterface = NULL;
			bEdited = FALSE;
		}
		return ret;
	}

	virtual void NotifyApexEditMode(class ApexEditInterface *iface) 
	{
		ApexEditInterface = iface;
		iface->SetApexEditNotify(Object,this);
	}

	virtual FLOAT GetBoundingBoxExpansion()
	{
		if (GetType() == AAT_APEX_EMITTER)
		{
			// get the iofx
			const NxParameterized::Interface* EmitterAssetInterface = GetNxParameterized();

			NxParameterized::Interface *iofxAssetNamedRef=NULL; 
			NxParameterized::getParamRef(*EmitterAssetInterface, "iofxAssetName", iofxAssetNamedRef); 
			if ( iofxAssetNamedRef )
			{
				FIApexAsset* IofxAsset = GApexCommands->GetApexAsset(iofxAssetNamedRef->name(),FALSE);
				if (IofxAsset)
				{
					return IofxAsset->GetBoundingBoxExpansion();
				}
			}
		}
		return BoundingBoxExpansion;
	}

	virtual void UpdateBoundingBoxExpansion()
	{
		BoundingBoxExpansion = 0.0f;

		// arbitrarily chosen
		FLOAT maxExpectedVelocity = 20.0f;

		if (GetType() == AAT_IOFX_MESH)
		{

			for (UINT i = 0; i < GetMeshCount(); ++i)
			{
				UStaticMesh* Mesh = GetMesh(i);
				if (Mesh)
				{
					BoundingBoxExpansion = Max(BoundingBoxExpansion, Mesh->Bounds.BoxExtent.X);
					BoundingBoxExpansion = Max(BoundingBoxExpansion, Mesh->Bounds.BoxExtent.Y);
					BoundingBoxExpansion = Max(BoundingBoxExpansion, Mesh->Bounds.BoxExtent.Z);
				}
			}

			NxIofxAsset* nxIofxAsset = (NxIofxAsset*)GetNxApexAsset();
			if (nxIofxAsset)
			{
				FLOAT maxScale = nxIofxAsset->getScaleUpperBound(maxExpectedVelocity);
				BoundingBoxExpansion *= maxScale;
			}
		}
		else if (GetType() == AAT_IOFX_SPRITE)
		{
			NxIofxAsset* nxIofxAsset = (NxIofxAsset*)GetNxApexAsset();
			if (nxIofxAsset)
				BoundingBoxExpansion = 0.5f * nxIofxAsset->getScaleUpperBound(maxExpectedVelocity); // Sprites have a radius of size 0.5f
		}
	}

// NVCHANGE_BEGIN: HL - Apex Emitter Asset Copy
	const char* GetAuthoringTypeName(ApexAssetType type);
	void CreateDefaultApexAsset(ApexAssetType type,UApexAsset *obj,UApexGenericAsset *dependency);
	void CreateApexAsset(ApexAssetType type,UApexAsset *obj,UApexGenericAsset *dependency,NxParameterized::Interface* iface);
// NVCHANGE_END: HL - Apex Emitter Asset Copy
#endif

	UBOOL ReleaseNxApexAsset(NxApexAsset *apexAsset);

	virtual const char * namedReferenceCallback(const char *className,const char *namedReference,NxParameterized::Handle & /*handle*/) 
	{
		const char *ret = NULL;
		ret = GApexCommands->namedReferenceCallback(className,namedReference);
		return ret;
	}

	class ObjectDependencies : public NxParameterized::NamedReferenceInterface
	{
	public:
		ObjectDependencies(UApexAsset *object)
		{
			Object = object;
			bFirst = TRUE;
		}

		virtual const char * namedReferenceCallback(const char *className,const char *namedReference,NxParameterized::Handle & /*handle*/) 
		{
			const char *ret = NULL;

			if ( bFirst )
			{
				bFirst = FALSE;
				Object->ResetNamedReferences();
			}
#if 1
			FIApexAsset *asset = GApexCommands->GetApexAsset(namedReference,FALSE);
			UApexAsset *obj = asset ? asset->GetUE3Object() : NULL;
#else
			UApexAsset *obj = FindObject<UApexAsset>(0, ANSI_TO_TCHAR(namedReference), 0);
#endif
			if ( obj )
			{
				Object->AddNamedReference(obj);
			}

			return ret;
		}
	private:
		UApexAsset	*Object;
		UBOOL		bFirst;
	};

	UApexAsset * GetUE3Object(void) const 
	{
		return Object;
	}

	void SetUE3Object(UApexAsset *obj)
	{
		Object = obj;
	}

	UBOOL LoadFromMemory(const void *mem,physx::PxU32 dlen,const char *AssetName,const char *originalName,UApexAsset *obj);
	UBOOL LoadFromParams(const void *Params,const char *AssetName,const char *originalName,UApexAsset *obj);

	virtual UBOOL IsReady(void)   // returns true if the asset is ready to be used.
	{
		UBOOL ret = FALSE;
		if ( GetNxApexAsset() && bForceLoad )
		{
			ret = TRUE;
		}
		return ret;
	}

	/**
	 * @return	The ApexAssetType.
	**/
  	virtual ApexAssetType GetType(void) const
  	{
    	return Type;
  	}

  	UBOOL SameName(const char *fname) const
  	{
		return strcmp(Name,fname) == 0;
  	}

  	UBOOL IsValid(void) const
  	{
    	return Type != AAT_LAST;
  	}

	void NotifyAssetGoingAway(void);

	/**
  	 * @return	null if it fails, else a pointer to the NxApexAsset. 
  	**/
  	virtual NxApexAsset * GetNxApexAsset(UBOOL checkForEditingChange=TRUE)
	{ 
#if WITH_APEX_PARTICLES
		if ( bEditPending && checkForEditingChange ) // if we have a hard edit pending, we need to apply it now.
		{
			NotifyAssetGoingAway();	// notify any actors that we are going away
			NxParameterized::Interface *iface = NewInterface; // see if we have a 'new' interface pointer.
			NewInterface = NULL;

			// sometimes iface == ApexAsset's parameterized here causing problems...
			const NxParameterized::Interface *assetInterface = ApexAssetObject->getAssetNxParameterized();
			if ( iface == NULL || iface == assetInterface )
			{
				iface = ApexAssetObject->releaseAndReturnNxParameterizedInterface();
			}
			else
			{
				GApexManager->GetApexSDK()->releaseAsset(*ApexAssetObject);
			}
			ApexAssetObject = GApexManager->GetApexSDK()->createAsset(iface,Name);
			bEdited = TRUE;
			bEditPending = FALSE;
			TweakInterface = NULL;
			NotifyAssetImport();
		}
#endif
		return ApexAssetObject; 
	};
  	
	/**
	 * Causes any actors using this asset to be created due to property changes.
  	**/
	virtual void RefreshActors(void)
	{
		NotifyAssetGoingAway();
		NotifyAssetImport();
	}

	/***
	* Causes the named resource provider to resolve any asset dependencies.
	***/
	void ForceLoad(void)
	{
		if ( GetNxApexAsset() && !bForceLoad )
		{
			// Note: By not checking the return value of forceLoadAssets we are assuming it 
			// was able to load everything in the first try. According to the APEX documentation
			// forceLoadAssets should be called periodically until it returns 0. This is probably
			// fine in the editor, but shouldn't be expected to work if called in the game during
			// async loading.
			if ( GIsEditor && !GIsGame )
			{
				GetNxApexAsset()->forceLoadAssets();
			}
			bForceLoad = TRUE;
		}
	}

  	/**
	 * Remaps the material names to the submeshes
  	 * @param	SubMeshCount -	Number of sub meshes. 
  	 * @param	Names -			The sub mesh names. 
  	**/
  	virtual	void				  ApplyMaterialRemap(physx::PxU32 subMeshCount,const char **names)
  	{
  		if ( GetNxApexAsset() )
  		{
  			NotifyAssetGoingAway();
			physx::PxU32 index = 0;
			NxParameterized::Interface *iface = GetNxApexAsset()->releaseAndReturnNxParameterizedInterface();
			SetNxApexAsset(NULL);
			check(iface);
			if ( iface )
			{
				physx::PxU32 count;
				const NxParameterized::ParamResult *results = NxParameterized::getParamList(*iface,NULL,"materialNames",count,true,false,GApexManager->GetApexSDK()->getParameterizedTraits());
				if ( results )
				{
					for (physx::PxU32 i=0; i<count; i++)
					{
						const NxParameterized::ParamResult &r = results[i];
						if ( r.mDataType == NxParameterized::TYPE_ARRAY )
						{
							for (physx::PxI32 j=0; j<r.mArraySize; j++)
							{
								if ( index < subMeshCount )
								{
									NxParameterized::Handle handle(r.mHandle);
									handle.set(j);
									handle.setParamString(names[index]);
									handle.popIndex();
								}
								index++;
							}
						}
					}
					NxParameterized::releaseParamList(count,results,GApexManager->GetApexSDK()->getParameterizedTraits());
				}
				SetNxApexAsset( GApexManager->GetApexSDK()->createAsset(iface,Name) );
			}
			NotifyAssetImport();
		}
	}

	/**
   * Notifies the actor that a fresh asset has been imported and the actor can be re-created.
	**/
    void NotifyAssetImport(void);

  	const char * GetName(void) const { return Name; };

	virtual const char * GetOriginalApexName(void) 
	{
		return OriginalApexName;
	}

	void setOriginalName(const char *name)
	{
		check(name);
		if ( OriginalApexName )
		{
			appFree(OriginalApexName);
		}
		physx::PxU32 slen = (physx::PxU32)strlen(name)+1;
		OriginalApexName = (char *)appMalloc(slen);
		memcpy(OriginalApexName,name,slen);
	}

	/**
	 * Gets the default parameterized interface
	 * @return	null if it fails, else a pointer to the ::NxParameterized::Interface.
	**/
	virtual ::NxParameterized::Interface * GetDefaultApexActorDesc(void)
  	{
		::NxParameterized::Interface *ret = NULL;

		if ( GetNxApexAsset() )
		{
			ret = GetNxApexAsset()->getDefaultActorDesc();
		}

	  	return ret;
  	}

	virtual const NxParameterized::Interface * GetNxParameterized(void) 
	{
		const NxParameterized::Interface *ret = NULL;
		NxApexAsset *asset = GetNxApexAsset();
		if ( asset )
		{
			ret = asset->getAssetNxParameterized();
		}
		return ret;
	}

	UBOOL FindActor(FIApexActor *Act) const
	{
		UBOOL ret = FALSE;
		for (TApexActorVector::const_iterator i=Actors.begin(); i!=Actors.end(); ++i)
		{
			if ( Act == static_cast<FIApexActor *>(*i) )
			{
				ret = TRUE;
				break;
			}
		}
		return ret;
	}

  /**
	* Increases the reference count
  	* @param [in,out]	Actor - If non-null, it will put the actor on a list for this asset. 
  	*
  	* @return	The reference count 
  **/
  virtual physx::PxI32 IncRefCount(FIApexActor *Act)
  {
  	  if ( Act )
  	  {
  	  	UBOOL found = FindActor( Act );
  	  	check( !found );
  	  	if ( !found )
  	  	{
    		for (TApexActorVector::iterator i=Actors.begin(); i!=Actors.end(); ++i)
    		{
    			if ( (*i) == NULL )
    			{
    				(*i) = Act;
    				found = TRUE;
    				break;
    			}
    		}
    		if ( !found )
    		{
  	  			Actors.push_back(Act);
  	  		}
		}
	  }
	  RefCount++;
	  return RefCount;
  }

  /**
	* Decrements the reference count
  	* @param [in,out]	Actor - If non-null, it will put the actor on a list for this asset. 
  	*
  	* @return	The reference count 
  **/
  virtual physx::PxI32 DecRefCount(FIApexActor *Act)
  {
  	  if ( Act )
  	  {
  	  	UBOOL found = FALSE;
		for (TApexActorVector::iterator i=Actors.begin(); i!=Actors.end(); ++i)
		{
			if ( Act == (*i) )
			{
				found = TRUE;
				(*i) = NULL;
				break;
			}
		}
		check( found );
	  }
	  RefCount--;
	  physx::PxI32 ret = RefCount;
	  check( RefCount >= 0 );
	  if ( RefCount == 0 )
	  {
		  delete this;
	  }
	  return ret;
  }

  /**
	* Gets the asset name
  	* @return	null if it fails, else the asset name. 
  **/
  virtual const char * GetAssetName(void) const
  {
	  return Name;
  }

  UBOOL BaseNameMatch(const char *baseName) const
  {
	  UBOOL ret = FALSE;

	  const char *apb = strstr(Name,".apb");
	  if ( !apb ) return FALSE;
	  const char *scan = apb-1;
	  const char *name = NULL;
	  while ( scan >= Name )
	  {
		  if ( *scan == '.' || *scan == SUBOBJECT_DELIMITER_CHAR )
		  {
			  name = scan+1;
			  break;
		  }
		  scan--;
	  }
	  if ( name )
	  {
		  if ( strcmp(baseName,name) == 0 )
		  {
			  ret = TRUE;
		  }
	  }
	  return ret;
  }

  /**
  * Renames the asset
  * @param	Fname	the new name. 
  **/
  virtual void Rename(const char * /*fname*/)
  {

  }

  virtual UBOOL IsValidForActorCreation(FIApexScene *scene)  
  {
	  UBOOL ret = FALSE;

	  NxApexAsset *aa = GetNxApexAsset();
	  NxApexScene *apex_scene = scene->GetApexScene();
	  const NxParameterized::Interface *params = aa ? aa->getAssetNxParameterized() : NULL;
	  if ( aa && apex_scene && params )
	  {
		  if ( aa->isValidForActorCreation(*params,*apex_scene) )
		  {
			  ret = TRUE;
		  }
	  }
	  return ret;
  }

	virtual physx::PxU32 GetMeshCount(void) const // return the number of UE3 Static Meshes referenced by this asset.
	{
		physx::PxU32 ret = 0;

// NVCHANGE_BEGIN: DJS - APEX particle support
#if WITH_APEX_PARTICLES
		if ( (Type == AAT_IOFX_MESH) && ApexAssetObject )
		{
			NxIofxAsset *iofxAsset = static_cast<NxIofxAsset *>(ApexAssetObject);
			NxParameterized::Interface *iofxAssetInterface = (NxParameterized::Interface *)iofxAsset->getAssetNxParameterized();
			NxParameterized::Interface *spriteMaterialNamedRef=NULL;
			NxParameterized::getParamRef(const_cast<NxParameterized::Interface&>(*iofxAssetInterface), "iofxType", spriteMaterialNamedRef);
			if( spriteMaterialNamedRef )
			{
				if ( strcmp(spriteMaterialNamedRef->className(), "MeshIofxParameters") == 0 )
				{
					NxParameterized::Handle handle(*iofxAssetInterface);
					if ( NxParameterized::findParam(*iofxAssetInterface,"iofxType.renderMeshList",handle) )
					{
						physx::PxI32 size;
						if ( handle.getArraySize(size,0) == NxParameterized::ERROR_NONE )
						{
							ret = (physx::PxU32)size;
						}
					}
				}
			}
		}
#endif	// WITH_APEX_PARTICLES
// NVCHANGE_END: DJS - APEX particle support

		return ret;
	}

	virtual UStaticMesh * GetMesh(physx::PxU32 index) const 
	{
		UStaticMesh *ret = NULL;

		// NVCHANGE_BEGIN: DJS - APEX particle support
#if WITH_APEX_PARTICLES
		if ( (Type == AAT_IOFX_MESH) && ApexAssetObject )
		{
			NxIofxAsset *iofxAsset = static_cast<NxIofxAsset *>(ApexAssetObject);
			NxParameterized::Interface *iofxAssetInterface = (NxParameterized::Interface *)iofxAsset->getAssetNxParameterized();
			NxParameterized::Interface *spriteMaterialNamedRef=NULL;
			NxParameterized::getParamRef(const_cast<NxParameterized::Interface&>(*iofxAssetInterface), "iofxType", spriteMaterialNamedRef);
			if( spriteMaterialNamedRef )
			{
				if ( strcmp(spriteMaterialNamedRef->className(), "MeshIofxParameters") == 0 )
				{
					NxParameterized::Handle handle(*iofxAssetInterface);
					if ( NxParameterized::findParam(*iofxAssetInterface,"iofxType.renderMeshList",handle) )
					{
						physx::PxI32 size;
						if ( handle.getArraySize(size,0) == NxParameterized::ERROR_NONE && index < (physx::PxU32)size)
						{
							NxParameterized::Handle elementHandle(*iofxAssetInterface);
							handle.getChildHandle(index, elementHandle);
							NxParameterized::Handle meshHandle(*iofxAssetInterface);
							if ( elementHandle.getChildHandle(iofxAssetInterface,"meshAssetName", meshHandle) == NxParameterized::ERROR_NONE )
							{
								const char *refName=NULL;
								NxParameterized::Interface *val=NULL;
								meshHandle.getParamRef(val);
								if ( val && val->name() )
								{
									void *tmp = GApexManager->GetApexSDK()->getNamedResourceProvider()->getResource( APEX_OPAQUE_MESH_NAME_SPACE, val->name());
									UStaticMesh *staticMesh = (UStaticMesh *)tmp;
									ret = staticMesh;
								}
							}
						}
					}
				}
			}
		}
#endif	// WITH_APEX_PARTICLES
// NVCHANGE_END: DJS - APEX particle support

		return ret;
	}


  virtual UMaterial * GetCurrentMaterial(void) const 
  {
	  UMaterial *ret = NULL;

// NVCHANGE_BEGIN: DJS - APEX particle support
#if WITH_APEX_PARTICLES
	  if ( (Type == AAT_IOFX_SPRITE || Type == AAT_IOFX_MESH) && ApexAssetObject )
	  {
		  NxIofxAsset *iofxAsset = static_cast<NxIofxAsset *>(ApexAssetObject);
		  NxParameterized::Interface *iofxAssetInterface = (NxParameterized::Interface *)iofxAsset->getAssetNxParameterized();
		  NxParameterized::Interface *spriteMaterialNamedRef=NULL;
		  NxParameterized::getParamRef(const_cast<NxParameterized::Interface&>(*iofxAssetInterface), "iofxType", spriteMaterialNamedRef);
		  if( spriteMaterialNamedRef )
		  {
			  if ( strcmp(spriteMaterialNamedRef->className(), "SpriteIofxParameters") == 0 )
			  {
				  NxParameterized::getParamRef(const_cast<NxParameterized::Interface&>(*iofxAssetInterface), "iofxType.spriteMaterialName", spriteMaterialNamedRef);
				  if ( spriteMaterialNamedRef && spriteMaterialNamedRef->name() )
				  {
					void *tmp = GApexManager->GetApexSDK()->getNamedResourceProvider()->getResource( APEX_MATERIALS_NAME_SPACE, spriteMaterialNamedRef->name() );
					ret = (UMaterial *)tmp;
				  }
			  }
			  else
			  {
				  NxParameterized::Handle handle(*iofxAssetInterface);
				  if ( NxParameterized::findParam(*iofxAssetInterface,"iofxType.renderMeshList",handle) )
				  {
					  physx::PxI32 size;
					  if ( handle.getArraySize(size,0) == NxParameterized::ERROR_NONE )
					  {
						  NxParameterized::Handle elementHandle(*iofxAssetInterface);
						  handle.getChildHandle(0, elementHandle);
						  NxParameterized::Handle meshHandle(*iofxAssetInterface);
						  if ( elementHandle.getChildHandle(iofxAssetInterface,"meshAssetName", meshHandle) == NxParameterized::ERROR_NONE )
						  {
							  const char *refName=NULL;
							  NxParameterized::Interface *val=NULL;
							  meshHandle.getParamRef(val);
							  if ( val && val->name() )
							  {
								  void *tmp = GApexManager->GetApexSDK()->getNamedResourceProvider()->getResource( APEX_OPAQUE_MESH_NAME_SPACE, val->name());
								  UStaticMesh *staticMesh = (UStaticMesh *)tmp;
								  if ( staticMesh && staticMesh->LODModels.Num() > 0  )
								  {
									  const FStaticMeshRenderData&       LODModel = staticMesh->LODModels(0);
									  const FStaticMeshElement&          Element  = LODModel.Elements(0);
									  if ( Element.Material )
									  {
										  ret = Element.Material->GetMaterial();
									  }
								  }
							  }
						  }
					  }
				  }
			  }
		  }
	  }
#endif	// WITH_APEX_PARTICLES
// NVCHANGE_END: DJS - APEX particle support

	  return ret;
  }

  /**
  * Gets the submesh count
  * @return	The submesh count.
  **/
  virtual physx::PxU32 GetNumMaterials(void)
  {
	  physx::PxU32 ret = 0;

// NVCHANGE_BEGIN: DJS - APEX particle support
#if WITH_APEX_PARTICLES
	  if ( GetNxApexAsset() )
	  {
		  const NxParameterized::Interface *iface = GetNxApexAsset()->getAssetNxParameterized();
		  if ( iface )
		  {
			  if ( GetType() == AAT_IOFX_MESH || GetType() == AAT_IOFX_SPRITE )
			  {
				  NxIofxAsset *iofxAsset = static_cast<NxIofxAsset *>(ApexAssetObject);
				  NxParameterized::Interface *iofxAssetInterface = (NxParameterized::Interface *)iofxAsset->getAssetNxParameterized();
				  NxParameterized::Interface *spriteMaterialNamedRef=NULL;
				  NxParameterized::getParamRef(const_cast<NxParameterized::Interface&>(*iofxAssetInterface), "iofxType", spriteMaterialNamedRef);
				  if( spriteMaterialNamedRef )
				  {
					  if ( strcmp(spriteMaterialNamedRef->className(), "SpriteIofxParameters") == 0 )
					  {
						  NxParameterized::Handle handle(*iofxAssetInterface);
						  if ( NxParameterized::findParam(*iofxAssetInterface,"iofxType.spriteMaterialName",handle) )
  						  {
							  ret = 1;
						  }
					  }
				  }
			  }
			  else
			  {
				  physx::PxU32 count;
				  const NxParameterized::ParamResult *results = NxParameterized::getParamList(*iface,NULL,"materialNames",count,true,false,GApexManager->GetApexSDK()->getParameterizedTraits());
				  if ( results )
				  {
					  for (physx::PxU32 i=0; i<count; i++)
					  {
						  const NxParameterized::ParamResult &r = results[i];
						  if ( r.mDataType == NxParameterized::TYPE_ARRAY )
						  {
							ret+=r.mArraySize;
						  }
					  }
					  NxParameterized::releaseParamList(count,results,GApexManager->GetApexSDK()->getParameterizedTraits());
				  }
			  }
		  }
	  }
#endif	// WITH_APEX_PARTICLES
// NVCHANGE_END: DJS - APEX particle support

	  return ret;
  }

  /**
  * Returns the material name
  * @param	Index - The material index.
  *
  * @return	null if it fails, else the material name. 
  **/
  virtual const char * GetMaterialName(physx::PxU32 findIndex)
  {
	  const char *ret = NULL;

	  physx::PxU32 index = 0;

	  if ( GetNxApexAsset() )
	  {
		  const NxParameterized::Interface *iface = GetNxApexAsset()->getAssetNxParameterized();
		  if ( iface )
		  {
// NVCHANGE_BEGIN: DJS - APEX particle support
#if WITH_APEX_PARTICLES
			  if ( GetType() == AAT_IOFX_MESH || GetType() == AAT_IOFX_SPRITE )
			  {
				  NxIofxAsset *iofxAsset = static_cast<NxIofxAsset *>(ApexAssetObject);
				  NxParameterized::Interface *iofxAssetInterface = (NxParameterized::Interface *)iofxAsset->getAssetNxParameterized();
				  NxParameterized::Interface *spriteMaterialNamedRef=NULL;
				  NxParameterized::getParamRef(const_cast<NxParameterized::Interface&>(*iofxAssetInterface), "iofxType", spriteMaterialNamedRef);
				  if( spriteMaterialNamedRef )
				  {
					  if ( strcmp(spriteMaterialNamedRef->className(), "SpriteIofxParameters") == 0 )
					  {
						  NxParameterized::Handle handle(*iofxAssetInterface);
						  if ( NxParameterized::findParam(*iofxAssetInterface,"iofxType.spriteMaterialName",handle) )
						  {
							  NxParameterized::Interface *val;
							  handle.getParamRef(val);
							  if ( val )
							  {
								  ret = val->name();
							  }
						  }
					  }
				  }
			  }
			  else
#endif	// WITH_APEX_SUPPORT
// NVCHANGE_END: DJS - APEX particle support
			  {
				  physx::PxU32 count;
				  const NxParameterized::ParamResult *results = NxParameterized::getParamList(*iface,NULL,"materialNames",count,true,false,GApexManager->GetApexSDK()->getParameterizedTraits());
				  if ( results )
				  {
					  for (physx::PxU32 i=0; i<count && ret==NULL; i++)
					  {
						  const NxParameterized::ParamResult &r = results[i];
						  if ( r.mDataType != NxParameterized::TYPE_ARRAY )
						  {
							  for (physx::PxI32 j=0; j<r.mArraySize; j++)
							  {
								  if ( index == findIndex )
								  {
									  NxParameterized::Handle handle(r.mHandle);
									  handle.set(j);
									  handle.getParamString(ret);
									  handle.popIndex();
									  break;
								  }
								  index++;
							  }
						  }
					  }
					  NxParameterized::releaseParamList(count,results,GApexManager->GetApexSDK()->getParameterizedTraits());
				  }
			  }
		  }
	  }

	  return ret;
  }

  	/**
	 * Serialize the asset so the game engine can save it out and restore it later.
  	 * @param [in,out]	Length - the length. 
  	 *
  	 * @return	null if it fails, else. 
  	**/
  	virtual const void * Serialize(physx::PxU32 &len) // serialize the asset so the game engine can save it out and restore it later.
  	{
  		void *ret = NULL;
  		len = 0;

		if ( !GetNxApexAsset( ) ) return NULL;

		physx::PxFileBuf *buffer = GApexManager->GetApexSDK()->createMemoryWriteStream();
		const NxParameterized::Interface *obj = GetNxApexAsset() ? GetNxApexAsset()->getAssetNxParameterized() : NULL;

		// NVCHANGE_BEGIN: APAN - Code review
		if ( obj && buffer )
		{
   			NxParameterized::Serializer *ser = GApexManager->GetApexSDK()->createSerializer(NxParameterized::Serializer::NST_BINARY);
   			check(ser);
   			if ( ser )
   			{
   				ser->serialize(*buffer,&obj,1);
   				ser->release();
   			}

			const void *data = GApexManager->GetApexSDK()->getMemoryWriteBuffer(*buffer,len);
			PX_ASSERT(data);
			if ( data )
			{
				ret = appMalloc(len);
				memcpy(ret,data,len);
			}
		}

		if ( buffer )
		{
			GApexManager->GetApexSDK()->releaseMemoryWriteStream(*buffer);
		}
		// NVCHANGE_END: APAN - Code review
  		return ret;
	}

  	/**
	 * Releases the serialized memory
  	 * @param	Mem - a pointer to the memory.
  	**/
	void ReleaseSerializeMemory(const void *mem)
	{
		appFree((void *)mem);
	}

	/**
	 * Gets the bind transformation for the asset
	 * @return	The bind transformation.
	**/
	virtual const physx::PxMat44& GetBindTransformation(void)
	{
		return Transformation;
	}

	void flipUV(::NxParameterized::Interface *ApexRenderMeshAssetAuthoring,bool flipU,bool flipV)
	{
		if ( !flipU && !flipV ) return;

		::NxParameterized::Handle handle(*ApexRenderMeshAssetAuthoring);
		::NxParameterized::Interface *iface = NxParameterized::findParam(*ApexRenderMeshAssetAuthoring,"submeshes",handle);
		if ( iface )
		{
			int NumSubmeshes=0;
			if ( handle.getArraySize(NumSubmeshes,0) == ::NxParameterized::ERROR_NONE )
			{
				// iterate each submesh
				for (physx::PxI32 i = 0; i < NumSubmeshes; i++)
				{
					// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
					char scratch[MAX_SPRINTF];

					appSprintfANSI( scratch, "submeshes[%d].vertexBuffer.vertexFormat.bufferFormats", i );
					// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions
					iface = NxParameterized::findParam(*ApexRenderMeshAssetAuthoring,scratch,handle);
					physx::PxI32 BufArraySize = 0;
					::NxParameterized::ErrorType BufArrayErrorType = handle.getArraySize(BufArraySize, 0);
					check(BufArrayErrorType ==::NxParameterized::ERROR_NONE);

					// iterate each vertex buffer of this submesh
					for (physx::PxI32 j = 0; j < BufArraySize; j++)
					{
						// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
						appSprintfANSI( scratch, "submeshes[%d].vertexBuffer.vertexFormat.bufferFormats[%d].semantic", i, j );
						// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions
						iface = NxParameterized::findParam(*ApexRenderMeshAssetAuthoring, scratch, handle);

						if (iface)
						{
							physx::PxI32 bufSemantic;
							::NxParameterized::ErrorType BufSemanticErrorType = handle.getParamI32(bufSemantic);
							check(BufSemanticErrorType ==::NxParameterized::ERROR_NONE);

							// this vertex buffer is a texCoords one
							if ( bufSemantic <= physx::NxRenderVertexSemantic::TEXCOORD3
								&& bufSemantic >= physx::NxRenderVertexSemantic::TEXCOORD0 )
							{
								// retrieve the data handle, It is not an F32x2 array it is an struct array
								// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
								appSprintfANSI( scratch, "submeshes[%d].vertexBuffer.buffers[%d].data", i, j );
								// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions
								iface = NxParameterized::findParam(*ApexRenderMeshAssetAuthoring, scratch, handle);
								physx::PxI32 DataElementSize = 0;

								if( ensure(handle.getArraySize(DataElementSize, 0) ==::NxParameterized::ERROR_NONE) )
								{
									physx::PxF32 maxTexelU = -1e9;
									physx::PxF32 maxTexelV = -1e9;

									for (physx::PxI32 k = 0; k < DataElementSize; k++)
									{
										handle.set(k); // push handle to select array element k
	                                    
										physx::PxF32 texel[2];
										handle.set(0); // read element 0
										handle.getParamF32(texel[0]);
										handle.popIndex();
										handle.set(1); // read element 1
										handle.getParamF32(texel[1]);
										handle.popIndex();

										maxTexelU = Max(maxTexelU, texel[0] - 0.0001f);
										maxTexelV = Max(maxTexelV, texel[1] - 0.0001f);
										handle.popIndex(); // pop handle back to array
									}

									maxTexelU = ::floor(maxTexelU) + 1.0f;
									maxTexelV = ::floor(maxTexelV) + 1.0f;

									for (physx::PxI32 k = 0; k < DataElementSize; k++)
									{
										handle.set(k); // push handle to select array element k

										physx::PxF32 texel;
										handle.set(1); // read element 1
										handle.getParamF32(texel);
										handle.popIndex();

										if (flipU)
										{
											handle.set(0); // read element 0
											handle.getParamF32(texel);
											handle.setParamF32(maxTexelU - texel);
											handle.popIndex();
										}

										if (flipV)
										{
											handle.set(1); // read element 1
											handle.getParamF32(texel);
											handle.setParamF32(maxTexelV - texel);
											handle.popIndex();
										}

										handle.popIndex(); // pop handle back to array
									}
								}
							}
						}
					}
				}
			}
		}
	}


#if !WITH_APEX_SHIPPING

// NVCHANGE_BEGIN: BRG - WITH_APEX_PARTICLES blocks so that this compiles if WITH_APEX_PARTICLES == 0
		virtual UBOOL NeedsRebuild(void) const
		{
#if WITH_APEX_PARTICLES
			return TweakInterface || bEditPending;
#else
			return FALSE;
#endif
		}

		virtual void ApplyOutstandingEdits(void)
		{
#if WITH_APEX_PARTICLES
			// if there have been tweakbles applied, and no other editing changes..
			if ( TweakInterface )
			{
				NotifyAssetGoingAway();

				// sometimes TweakInterface == ApexAsset's parameterized here causing problems...
				const NxParameterized::Interface *assetInterface = ApexAssetObject->getAssetNxParameterized();
				if ( bEdited || TweakInterface == assetInterface )
				{
					ApexAssetObject->releaseAndReturnNxParameterizedInterface();
				}
				else
				{
					GApexManager->GetApexSDK()->releaseAsset(*ApexAssetObject);
				}
				ApexAssetObject = GApexManager->GetApexSDK()->createAsset(TweakInterface,Name);
				NotifyAssetImport();
				TweakInterface = NULL;
			}
			else
			{
				GetNxApexAsset(TRUE);
			}
#endif	// #if WITH_APEX_PARTICLES
// NVCHANGE_END: BRG - WITH_APEX_PARTICLES blocks so that this compiles if WITH_APEX_PARTICLES == 0
		}

	/**
	 * Applies a transformation to the asset
	 * @param	Transformation - The transformation.
	 * @param	Scale -	The scale.
	 * @param	bApplyToGraphics - TRUE to apply to graphics.
	 * @param	bApplyToPhysics	- TRUE to apply to physics.
	 * @param	bClockWise - TRUE to clock wise.
	 *
	 * @return	TRUE if it succeeds, FALSE if it fails.
	**/
	virtual UBOOL ApplyTransformation(const physx::PxMat44 &transformation,physx::PxF32 scale,UBOOL applyToGraphics,UBOOL applyToPhysics,UBOOL /*clockWise*/)
  	{
  		UBOOL ret = FALSE;

		if ( Type == AAT_DESTRUCTIBLE && GetNxApexAsset() )
		{
			NotifyAssetGoingAway();

			NxApexSDK *apexSDK = GApexManager->GetApexSDK();
			NxApexAsset *apexAsset = GetNxApexAsset();
			NxDestructibleAsset *ApexDestructibleAsset = static_cast< NxDestructibleAsset *>(apexAsset);
			NxModuleDestructible *dmodule = GApexManager->GetModuleDestructible();

			// create an NxDestructibleAssetAuthoring instance of the asset...
			NxDestructibleAssetAuthoring *ApexDestructibleAssetAuthoring = static_cast<NxDestructibleAssetAuthoring*>(ApexAuthoringFromAsset(GetNxApexAsset(), apexSDK));

			check(ApexDestructibleAssetAuthoring);

			if( ensure(ApexDestructibleAssetAuthoring) )
			{
				//Avoid using NxDestructibleAssetAuthoring::getRenderMeshAssetAuthoring function since it will return you a 
				//copy instead of a reference of the render mesh asset authoring
				NxParameterized::Interface* _ApexDestructibleInterface = ApexDestructibleAssetAuthoring->getNxParameterized();
				
				if ( ensure(_ApexDestructibleInterface) )
				{
					NxParameterized::Interface *iface = NULL;
					bool flipU = false;
					bool flipV = true;

					NxParameterized::getParamRef( *_ApexDestructibleInterface, "renderMeshAsset", iface );
					PX_ASSERT( iface );
#if 0
					NxParameterized::Handle handle(iface);
					NxParameterized::Interface *uVOriginInterface = NxParameterized::findParam(*iface,"textureUVOrigin",handle);
					if ( uVOriginInterface )
					{
						physx::PxU32 textureUVOrigin;
						handle.getParamU32(textureUVOrigin);
						textureUVOrigin = NxTextureUVOrigin::ORIGIN_BOTTOM_LEFT;
						switch ( textureUVOrigin )
						{
							case NxTextureUVOrigin::ORIGIN_TOP_LEFT:
								flipU = false;
								flipV = false;
								break;
							case NxTextureUVOrigin::ORIGIN_TOP_RIGHT:	
								flipU = true;
								flipV = false;
								break;
							case NxTextureUVOrigin::ORIGIN_BOTTOM_LEFT:
								flipU = false;
								flipV = true;
								break;
							case NxTextureUVOrigin::ORIGIN_BOTTOM_RIGHT:
								flipU = false;
								flipV = false;
								break;
						}
						handle.setParamU32(NxTextureUVOrigin::ORIGIN_TOP_LEFT);
					}
#endif
					if( ensure(iface) )
					{
						flipUV(iface,flipU,flipV);
					}
					if( iface == NULL )
					{
						debugf( NAME_DevPhysics, TEXT("Unable to find renderMeshAsset in the APEX destructible asset") );
						return FALSE;
					}
				}

				ApexDestructibleAssetAuthoring->applyTransformation(transformation,scale);

				// cache off asset name...
				char ApexAssetName[1024] = {0};
				// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
				appStrncpyANSI( ApexAssetName, ApexDestructibleAsset->getName(), sizeof(ApexAssetName) );
				// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions

				// destroy old asset...
				ApexDestructibleAsset->release();
				ApexDestructibleAsset = NULL;

				// create new asset from the authoring...
				ApexDestructibleAsset = (NxDestructibleAsset*)apexSDK->createAsset( *ApexDestructibleAssetAuthoring, ApexAssetName );
				PX_ASSERT(ApexDestructibleAsset);

				// release authoring...
				ApexDestructibleAssetAuthoring->release();

				SetNxApexAsset(ApexDestructibleAsset);
				NotifyAssetImport();

			}
		}
		else if ( Type == AAT_CLOTHING && GetNxApexAsset() )
		{
  			NotifyAssetGoingAway();

   			NxApexSDK *apexSDK = GApexManager->GetApexSDK();
   			NxApexAsset *apexAsset = GetNxApexAsset();
   			NxClothingAsset *ApexClothingAsset = static_cast< NxClothingAsset *>(apexAsset);
   			NxModuleClothing *dmodule = GApexManager->GetModuleClothing();

   			// create an NxClothingAssetAuthoring instance of the asset...
			NxClothingAssetAuthoring *ApexClothingAssetAuthoring = static_cast<NxClothingAssetAuthoring*>(ApexAuthoringFromAsset(GetNxApexAsset(), apexSDK));

   			check(ApexClothingAssetAuthoring);

   			if(ApexClothingAssetAuthoring)
   			{
				ApexClothingAssetAuthoring->applyTransformation(transformation,scale,applyToGraphics ? true : false,applyToPhysics ? true : false);
				Transformation = transformation;
				TPxMat44Vector newBindPoses;
				for (physx::PxU32 i=0; i<ApexClothingAsset->getNumUsedBones(); i++)
				{
					physx::PxMat44 mat;
					ApexClothingAssetAuthoring->getBoneBindPose(i,mat);
					physx::PxMat44 pmat;
					pmat = mat * transformation;
					newBindPoses.push_back(pmat);
				}

				ApexClothingAssetAuthoring->updateBindPoses(&newBindPoses[0], newBindPoses.size(), TRUE, TRUE);

    			// destroy and create a new asset based off the authoring version...
    			if(ApexClothingAssetAuthoring)
    			{

					for (physx::PxU32 i=0; i<256; i++)
					{
						NxParameterized::Interface *ApexRenderMeshAssetAuthoring = ApexClothingAssetAuthoring->getRenderMeshAssetAuthoring(i);
						if ( ApexRenderMeshAssetAuthoring )
						{
							NxParameterized::Handle handle(ApexRenderMeshAssetAuthoring);
							bool flipU = false;
							bool flipV = false;
							NxParameterized::Interface *iface = NxParameterized::findParam(*ApexRenderMeshAssetAuthoring,"textureUVOrigin",handle);
							if ( iface )
							{
								physx::PxU32 textureUVOrigin;
								handle.getParamU32(textureUVOrigin);
								textureUVOrigin = NxTextureUVOrigin::ORIGIN_BOTTOM_LEFT;
								switch ( textureUVOrigin )
								{
									case NxTextureUVOrigin::ORIGIN_TOP_LEFT:
										flipU = false;
										flipV = false;
										break;
									case NxTextureUVOrigin::ORIGIN_TOP_RIGHT:	
										flipU = true;
										flipV = false;
										break;
									case NxTextureUVOrigin::ORIGIN_BOTTOM_LEFT:
										flipU = false;
										flipV = true;
										break;
									case NxTextureUVOrigin::ORIGIN_BOTTOM_RIGHT:
										flipU = false;
										flipV = false;
										break;
								}
								handle.setParamU32(NxTextureUVOrigin::ORIGIN_TOP_LEFT);
							}
							flipUV(ApexRenderMeshAssetAuthoring,flipU,flipV);
#if 0  // experimental code to flip the winding order of the mesh
							{
								char scratch[512];
								physx::string::sprintf_s(scratch,512,"submeshes[%d].indexBuffer",i);
								iface = NxParameterized::findParam(*ApexRenderMeshAssetAuthoring,scratch,handle);
								PxI32 size=0;
								if ( ensure(iface->getArraySize(handle,size,0) == NxParameterized::ERROR_NONE) )
								{
									physx::PxU32 tcount = size/3;
									for (physx::PxU32 i=0; i<tcount; i++)
									{
										physx::PxU32 indices[3];
										NxParameterized::ErrorType type = handle.getParamU32Array(indices,3,(i*3));
										check( type == NxParameterized::ERROR_NONE );
										physx::PxU32 t = indices[0];
										indices[0] = indices[2];
										indices[2] = t;
										iface->setParamU32Array(handle,indices,3,(i*3));
									}
								}
							}
#endif
#if 0 // experimental code to inverse the normals of the mesh
							{
								char scratch[512];
								physx::string::sprintf_s(scratch,512,"submeshes[%d].vertexBuffer.normals",i);
								iface = NxParameterized::findParam(*ApexRenderMeshAssetAuthoring,scratch,handle);
								PxI32 size=0;
								if ( ensure(iface->getArraySize(handle,size,0) == NxParameterized::ERROR_NONE) )
								{
									physx::PxU32 nc = size;
									for (physx::PxU32 i=0; i<nc; i++)
									{
										physx::PxVec3 normal;
										NxParameterized::ErrorType type = iface->getParamVec3Array(handle,&normal,1,i);
										check( type == NxParameterized::ERROR_NONE );
										normal*=-1; // flip normal sign
										iface->setParamVec3Array(handle,&normal,1,i);
									}
								}
							}
#endif
						}
						else
						{
							break;
						}


					}

    				// cache off asset name...
    				char ApexAssetName[1024] = {0};
					// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
					appStrncpyANSI( ApexAssetName, ApexClothingAsset->getName(), sizeof(ApexAssetName) );
					// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions

    				// destroy old asset...
    				ApexClothingAsset->release();
    				ApexClothingAsset = NULL;

    				// create new asset from the authoring...
    				ApexClothingAsset = (NxClothingAsset*)apexSDK->createAsset( *ApexClothingAssetAuthoring, ApexAssetName );
    				check(ApexClothingAsset);

    				// release authoring...
    				ApexClothingAssetAuthoring->release();
    			}

    			SetNxApexAsset(ApexClothingAsset);

   				NotifyAssetImport();
   			}
		}

		return ret;
	}


	/**
	 * Flips a clothing asset's index buffer triangle winding (used for 0.9 assets that were never flipped)
	 * @return	TRUE if it succeeds, FALSE if it fails. 
	**/
	virtual UBOOL MirrorLegacyClothingNormalsAndTris()
	{
		UBOOL ret = FALSE;
		if ( Type == AAT_CLOTHING && GetNxApexAsset() )
		{
  			NotifyAssetGoingAway();

   			NxApexSDK *apexSDK = GApexManager->GetApexSDK();
   			NxApexAsset *apexAsset = GetNxApexAsset();
   			NxClothingAsset *ApexClothingAsset = static_cast< NxClothingAsset *>(apexAsset);
   			NxModuleClothing *dmodule = GApexManager->GetModuleClothing();

			NxClothingAssetAuthoring *ApexClothingAssetAuthoring = static_cast<NxClothingAssetAuthoring*>(ApexAuthoringFromAsset(GetNxApexAsset(), apexSDK));

   			PX_ASSERT(ApexClothingAssetAuthoring);

			// destroy and create a new asset based off the authoring version...
			if(ApexClothingAssetAuthoring)
			{

				for (physx::PxU32 i=0; i<256; i++)
				{
					NxParameterized::Interface *ApexRenderMeshAssetAuthoring = ApexClothingAssetAuthoring->getRenderMeshAssetAuthoring(i);
					if ( ApexRenderMeshAssetAuthoring )
					{
						NxParameterized::Handle handle(ApexRenderMeshAssetAuthoring);
						NxParameterized::Interface *iface = 0;
						// reverse triangle winding
						{
							// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
							char ibName[MAX_SPRINTF];
							appSprintfANSI( ibName, "submeshes[%d].indexBuffer", i );
							// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions
							iface = NxParameterized::findParam( *ApexRenderMeshAssetAuthoring, ibName, handle );
							PX_ASSERT( iface );
							physx::PxI32 size=0;
							if ( handle.getArraySize(size,0) == NxParameterized::ERROR_NONE )
							{
								physx::PxU32 tcount = size/3;
								for (physx::PxU32 i=0; i<tcount; i++)
								{
									physx::PxU32 indices[3];
									NxParameterized::ErrorType type = handle.getParamU32Array( indices, 3, (i*3) );
									PX_ASSERT( type == NxParameterized::ERROR_NONE );
									physx::PxU32 t = indices[1];
									indices[1] = indices[2];
									indices[2] = t;
									handle.setParamU32Array( indices, 3, (i*3) );

									ret = TRUE;
								}
							}
							else
							{
								PX_ASSERT(0);
							}
						}

						// reverse normals
						{
							// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
							char vbName[MAX_SPRINTF];

							physx::PxI32 numFormats = 0;
							appSprintfANSI( vbName, "submeshes[%d].vertexBuffer.vertexFormat.bufferFormats", i );
							// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions
							iface = NxParameterized::findParam( *ApexRenderMeshAssetAuthoring, vbName, handle );
							handle.getArraySize(numFormats);

							for ( physx::PxI32 j=0; j<numFormats; j++ )
							{
								// look at each buffer semantic in the VertexFormatParameters
								physx::PxI32 semantic = physx::NxRenderVertexSemantic::NUM_SEMANTICS;

								// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
								appSprintfANSI( vbName, "submeshes[%d].vertexBuffer.vertexFormat.bufferFormats[%d].semantic", i, j );
								// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions
								iface = NxParameterized::findParam( *ApexRenderMeshAssetAuthoring, vbName, handle );
								handle.getParamI32(semantic);

								if ( physx::NxRenderVertexSemantic::NORMAL == semantic )
								{
									// verify this semantic is FLOAT3
									physx::PxU32 format = physx::NxRenderDataFormat::UNSPECIFIED;
									// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
									appSprintfANSI( vbName, "submeshes[%d].vertexBuffer.vertexFormat.bufferFormats[%d].format", i, j);
									// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions
									iface = NxParameterized::findParam( *ApexRenderMeshAssetAuthoring, vbName, handle );
									handle.getParamU32(format);

									PX_ASSERT( format == physx::NxRenderDataFormat::FLOAT3 );
									if ( format != physx::NxRenderDataFormat::FLOAT3 )
									{
										break;
									}

									// get a handle to the proper data buffer
									// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
									appSprintfANSI( vbName, "submeshes[%d].vertexBuffer.buffers[%d].data", i, j );
									// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions
									iface = NxParameterized::findParam( *ApexRenderMeshAssetAuthoring, vbName, handle );

									physx::PxI32 size=0;
									if ( handle.getArraySize(size) == NxParameterized::ERROR_NONE )
									{
										// This transform is a complete inversion, not just along y-axis.
										// The reason for this is that the old transform that was erronously applied was a 3x3 diagonal matrix (-1, 1, -1)
										// The real transform that should have happened is a 3x3 diagonal matrix (1, -1, 1).
										// To undo the old and do the right transform, the combined transform is the 3x3 diagonal matrix (-1, -1, -1)
										physx::PxMat33 mirrorTransform = -physx::PxMat33::createIdentity();

										for (physx::PxI32 k=0; k<size; k++)
										{
											physx::PxVec3 normal;

											handle.set(k);
											//PX_ASSERT( NxParameterized::DataType::TYPE_VEC3 == handle.parameterDefinition()->type() );

											NxParameterized::ErrorType e = handle.getParamVec3( normal );
											PX_ASSERT( e == NxParameterized::ERROR_NONE );

											// apply the transformation
											normal = mirrorTransform.transform( normal );
											handle.setParamVec3( normal );

											handle.popIndex();
											ret = TRUE;
										}
									}
									else
									{
										PX_ASSERT(0);
									}
								}
							}
						}

					}
					else
					{
						break;
					}
				}

				// cache off asset name...
				char ApexAssetName[1024] = {0};
				// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
				appStrncpyANSI( ApexAssetName, ApexClothingAsset->getName(), sizeof(ApexAssetName) );
				// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions

				// destroy old asset...
				ApexClothingAsset->release();
				ApexClothingAsset = NULL;

				// create new asset from the authoring...
				ApexClothingAsset = (NxClothingAsset*)apexSDK->createAsset( *ApexClothingAssetAuthoring, ApexAssetName );
				PX_ASSERT(ApexClothingAsset);

				// release authoring...
				ApexClothingAssetAuthoring->release();
			}

			SetNxApexAsset(ApexClothingAsset);

			NotifyAssetImport();
		
		}

		return ret;
	}

#endif

	virtual UBOOL GetDesc(char *dest,physx::PxU32 slen)
	{
		UBOOL ret = FALSE;
		if ( GetNxApexAsset() )
		{
			const char *type = "UNKNOWN";

			switch ( GetType() )
			{
			case AAT_DESTRUCTIBLE:
				type = "Destructible";
				break;
			case AAT_CLOTHING:
				type = "Clothing";
				break;
			case AAT_APEX_EMITTER:		// asset used to construct emitters.
				type = "Shape Emitter";
				break;
			case AAT_RENDER_MESH:
				type = "Render Mesh";
				break;
			case AAT_GENERIC: // just generic data, we don't resolve it to a specific type.
				check(0);
				break;
			case AAT_GROUND_EMITTER:
				type = "Ground Emitter";
				break;
			case AAT_IMPACT_EMITTER:
				type = "Impact Emitter";
				break;
			case AAT_BASIC_IOS:
				type = "Basic IOS";
				break;
				// NVCHANGE_BEGIN: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
#if WITH_APEX_FLUID_IOS
			case AAT_FLUID_IOS:
				type = "Fluid IOS";
				break;
#endif
				// NVCHANGE_END: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
			case AAT_IOFX_SPRITE:
				type = "IOFX Sprite";
				break;
			case AAT_IOFX_MESH:
				type = "IOFX Mesh";
				break;
			default:
				check(0);
				break;

			}

			// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
			appStrncpyANSI( dest, type, slen );
			// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions

			ret = TRUE;
		}
		else
		{
			// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
			appStrncpyANSI( dest, "NO ASSET", slen );
			// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions
		}
		return ret;
	}

	// only keep the root file name; so skip path information up to all 'slashes'
	// also, truncate any postfix '.apx' '.apb', etc.
	void normalizeName(const char *source,char *_dest,physx::PxU32 len)
	{
		char *dest = _dest;
		const char *begin = source;
		const char *stop = &source[len];
		while ( *source && source < stop )
		{
			if ( *source == '\\' )
			{
				begin = source+1;
			}
			else if ( *source == '/' )
			{
				begin = source+1;
			}
			source++;
		}
		while ( *begin && begin < stop && *begin != '.' )
		{
			*dest++ = *begin++;
		}
		*dest = 0;
		// Convert _dest to lower case, no appStrlwrANSI exists
		for (ANSICHAR* Lower = _dest; *Lower != '\0'; ++Lower)
		{
			*Lower = tolower(*Lower);
		}
	}

	BOOL NameMatch(const char *typeName,const char *fname,BOOL &exactMatch)
	{
		BOOL ret = FALSE;

		if ( GetNxApexAsset() )
		{
			const char *objName = GetNxApexAsset()->getObjTypeName();

			if ( typeName == NULL )
			{
				typeName = "";
				objName = "";
			}
			if ( strcmp(objName,typeName) == 0 )
			{
				if ( strcmp(fname,Name) == 0 ) // if the base names match, it's already cool!
				{
					exactMatch = TRUE;
					ret = TRUE;
				}
				else
				{
					char scratch1[512];
					normalizeName(fname,scratch1,512);
					char scratch2[512];
					normalizeName(OriginalApexName,scratch2,512);
					if ( strcmp(scratch1,scratch2) == 0 )
					{
						ret = TRUE;
					}
				}
			}
		}

		return ret;
	}

	void SetNxApexAsset(NxApexAsset *asset)
	{
#if WITH_APEX_PARTICLES
		check( bEditPending == FALSE );
		check( NewInterface == NULL );
		check( TweakInterface == NULL );
#endif
		ApexAssetObject = asset;
	}


  ApexAssetType			Type;
  char *				Name;
  char *				OriginalApexName;
  physx::PxI32			RefCount;


  TApexActorVector		Actors;  // the actors associated with
  physx::PxMat44		Transformation;
  UBOOL					bForceLoad;
  UApexAsset			*Object;
  
#if WITH_APEX_PARTICLES
    UBOOL				bTweaked;
	UBOOL				bEdited; // true if this asset was edited with the APEX properties editor.
	UBOOL				bEditPending;
	ApexEditInterface	*ApexEditInterface;
	NxParameterized::Interface *NewInterface;
	NxParameterized::Interface *TweakInterface;

	FLOAT				BoundingBoxExpansion;
#endif

	NxApexAsset		   *ApexAssetObject;

};



//**********************************************************************

#define APEX_DEBUG_ADD(x) GNRP->setResourceU32("AI_APEX_DEBUG",#x,NxApexParam::VISUALIZE_##x,TRUE)

enum ApexCommands
{
    ASC_NONE,
	ASC_APEXVIS,
	ASC_APEX_SHOW,
	ASC_APEXDESTRUCTIBLELOD,
	ASC_APEXDESTRUCTIBLEMAXDYNAMICCHUNKISLAND,
	ASC_APEXDESTRUCTIBLEMAXCHUNK,
	ASC_APEX_LOD_RESOURCE_BUDGET,
	ASC_APEX_PAUSE,
// NVCHANGE_BEGIN: hlanker - clothing actor recording
	ASC_APEXCLOTHINGREC
// NVCHANGE_END: hlanker - clothing actor recording
};

#define ASC_ADD(x) GNRP->setResourceU32("AI_COMMANDS",#x,ASC_##x,TRUE)

// NVCHANGE_BEGIN: PHATT - Dynamically generated debug rendering menu
#define MODULE_COUNT 14
static const char *moduleNames[MODULE_COUNT] =
{
	"Clothing",
	"Destructible",
	"Emitter",
	"Iofx",
	"Particles",
	"Vegetation",
	"Wind",
	"Explosion",
	"FieldBoundary",
	"BasicIos",
	"FluidIos",
	"FieldSampler",
	"BasicFS",
	"TurbulenceFS"
};
// NVCHANGE_END: PHATT - Dynamically generated debug rendering menu

class MyApexCommands : public FIApexCommands
{
public:
  MyApexCommands(void)
  {
	  LastBuffer = NULL;
	  LastName = NULL;
	  bPaused = FALSE;

  	GNRP = GApexManager->GetApexSDK()->getNamedResourceProvider();
	ApexRender = CreateApexRender();
	ResourceCallback = CreateResourceCallback();
	bShowApex = TRUE;

	GApexManager->SetResourceCallback(ResourceCallback);
	GApexManager->SetRenderResourceManager(ApexRender->GetRenderResourceManager() );

	bOnExit = FALSE;
    GApexCommands = this;
	ApexSDK = GApexManager->GetApexSDK();
	bVisualizeApex  = FALSE;
// NVCHANGE_BEGIN: hlanker - clothing actor recording
	bClothingRecording = FALSE;
// NVCHANGE_END: hlanker - clothing actor recording
    bFreshCommand = FALSE;


// NVCHANGE_BEGIN: PHATT - Dynamically generated debug rendering menu
	bDebugVisNamesInitialized = FALSE;
// NVCHANGE_END: PHATT - Dynamically generated debug rendering menu

#if WITH_APEX_PARTICLES
	StaticMesh = NULL;
#endif
	ASC_ADD(APEXVIS);
	ASC_ADD(APEX_SHOW);
	ASC_ADD(APEXDESTRUCTIBLELOD);
	ASC_ADD(APEXDESTRUCTIBLEMAXCHUNK);
	ASC_ADD(APEX_LOD_RESOURCE_BUDGET);
	ASC_ADD(APEX_PAUSE);
// NVCHANGE_BEGIN: hlanker - clothing actor recording
	ASC_ADD(APEXCLOTHINGREC);
// NVCHANGE_END: hlanker - clothing actor recording
  }

  ~MyApexCommands(void)
  {
	  appFree(LastBuffer);
	  appFree(LastName);
	bOnExit = TRUE;
	physx::PxU32 count;
	void **assets = GNRP->findAllResources("AI_APEX_ASSET",count);
	for (physx::PxU32 i=0; i<count; i++)
	{
		ApexAsset *aa = (ApexAsset *)assets[i];
		delete aa;
	}
	GNRP->releaseAllResourcesInNamespace("AI_APEX_ASSET");

	ReleaseApexRender(ApexRender);

	{
		physx::PxU32 count;
		void **strings = GNRP->findAllResources("AI_PERSISTENT_STRING",count);
		for (physx::PxU32 i=0; i<count; i++)
		{
			appFree(strings[i]);
		}
	}

	GNRP->releaseAllResourcesInNamespace("AI_APEX_DEBUG");
	GNRP->releaseAllResourcesInNamespace("AI_COMMANDS");
	GNRP->releaseAllResourcesInNamespace("AI_PERSISTENT_STRING");

	ReleaseResourceCallback(ResourceCallback);
	GApexManager->SetResourceCallback(0);
	// NVCHANGE_BEGIN: JCAO - Fix assertion during cooking
	NxApexAssetLookup.Empty();
	// NVCHANGE_END: JCAO - Fix assertion during cooking
  }

  virtual NxResourceCallback      * getResourceCallback(void)
  {
    return ResourceCallback;
  }

  virtual NxUserRenderResourceManager * getRenderResourceManager(void)
  {
  	return ApexRender->GetRenderResourceManager();
  }

  const char *skipSpaces(const char *str)
  {
    while ( *str && *str == 32 ) str++;
    if ( *str == 0 )
    {
        str = NULL;
    }
    return str;
  }

  const char * GetCommand(const char *cmd,char *command)
  {
    char *dest = command;
    char *edest = &command[510];
    while ( *cmd && *cmd != 32 )
    {
      *dest++ = *cmd++;
      if ( dest == edest )
      {
          break;
      }
    }
    *dest = NULL;
    cmd = skipSpaces(cmd);

	// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
	// Convert command to upper case, no appStruprANSI exists
	for (ANSICHAR* Upper = command; *Upper != '\0'; ++Upper)
	{
		*Upper = toupper(*Upper);
	}
	// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions

	return cmd;
  }

  virtual UBOOL isNumeric(const char *cmd)
  {
    UBOOL ret = FALSE;
    if ( cmd )
    {
      if ( *cmd == '-' || (*cmd >= '0' && *cmd <= '9') || *cmd == '.' )
      {
        ret = TRUE;
      }
    }
    return ret;
  }

  physx::PxF32 GetDebugValue(const char *cmd,physx::PxF32 v)
  {
	  if ( isNumeric(cmd) )
	  {
		  v = (physx::PxF32)atof(cmd);
	  }
	  else if ( strcmp(cmd,"TRUE") == 0 )
	  {
		  v = 1;
	  }
	  else if ( strcmp(cmd,"FALSE") == 0 )
	  {
		  v = 0;
	  }
	  return v;
  }

  bool IsNameToIgnore(const char* name) const
  {
	  return strcmp(name,"VISUALIZATION_ENABLE") == 0 ||
		  strcmp(name,"VISUALIZATION_SCALE") == 0  ||
		  strcmp(name,"VISUALIZE_ACTOR") == 0  ||
		  strcmp(name,"VISUALIZE_EMITTER_ACTOR") == 0  ||
		  strcmp(name,"VISUALIZE_DESTRUCTIBLE_ACTOR") == 0  ||
		  strcmp(name,"VISUALIZE_IOFX_ACTOR") == 0  ||
		  strcmp(name,"VISUALIZE_GROUND_EMITTER_ACTOR") == 0  ||
		  strcmp(name,"VISUALIZE_IMPACT_EMITTER_ACTOR") == 0  ||
		  strcmp(name,"VISUALIZE_BASIC_IOS_ACTOR") == 0  ||
		  strcmp(name,"VISUALIZE_FLUID_IOS_ACTOR") == 0  ||
		  strcmp(name,"VISUALIZE_CLOTHING_ACTOR") == 0 ||
		  strcmp(name,"moduleName") == 0;
  }

  /**
  * Process commands
  * @param	Format - Describes the format to use. 
  *
  * @return	TRUE if it succeeds, FALSE if it fails. 
  **/
  virtual UBOOL ProcessCommand(const char* Format, FOutputDevice* Ar)
  {
	UBOOL ret = TRUE;

	const char *cmd = Format;

    char command[512];
    cmd = GetCommand(cmd,command);
    ApexCommands asc = (ApexCommands)GNRP->findResourceU32("AI_COMMANDS",command);
    switch ( asc )
    {
		case ASC_APEX_PAUSE:
			bPaused = bPaused ? FALSE : TRUE;
			break;
		case ASC_APEX_LOD_RESOURCE_BUDGET:
			if ( cmd )
			{
				physx::PxF32 LOD = 0.0f;
				if ( 0 == appStricmp(ANSI_TO_TCHAR(cmd), TEXT("default")) )
				{
					LOD = GSystemSettings.ApexLODResourceBudget;
				}
				else
				{
					LOD = atof( cmd );
				}
				if ( GWorld && GWorld->RBPhysScene && GWorld->RBPhysScene->ApexScene )
				{
					physx::apex::NxApexScene *Scene = GWorld->RBPhysScene->ApexScene->GetApexScene();
					if ( Scene )
					{
						Scene->setLODResourceBudget( LOD );
						Ar->Logf(NAME_DevPhysics, TEXT("SetApexLODResourceBudget(%f)"), (FLOAT) LOD );
					}
				}
			}
			else if ( GWorld && GWorld->RBPhysScene && GWorld->RBPhysScene->ApexScene )
			{
				physx::apex::NxApexScene *scene = GWorld->RBPhysScene->ApexScene->GetApexScene();
				if ( scene )
				{
					physx::PxF32 LOD = scene->getLODResourceConsumed();
					Ar->Logf(NAME_DevPhysics, TEXT("ApexLODResourceConsumed(%f)"), (FLOAT) LOD );
				}
			}
			break;
		case ASC_APEXDESTRUCTIBLELOD:
			if ( cmd )
			{
				// Set new destructible LOD (must be between 0 and 1)
				physx::apex::NxModuleDestructible *DestructibleModule = GApexManager->GetModuleDestructible();
				if ( DestructibleModule  )
				{
					FLOAT NewLOD = 0.0f;
					if ( 0 == appStricmp(ANSI_TO_TCHAR(cmd), TEXT("default")) )
					{
						NewLOD = GSystemSettings.ApexDestructionMaxChunkSeparationLOD;
					}
					else
					{
						NewLOD = Clamp<FLOAT>( appAtof(ANSI_TO_TCHAR(cmd)), 0.0f, 1.0f );
					}

					DestructibleModule->setMaxChunkSeparationLOD( NewLOD );

					Ar->Logf( NAME_DevPhysics, TEXT("ApexDestructibleLod(%f)"), NewLOD );
				}
			}
			else
			{
				// Show usage
				Ar->Logf( NAME_DevPhysics, TEXT("ApexDestructibleLod [newLOD], new LOD between 0 and 1") );
			}
			break;
		// NVCHANGE_BEGIN: BRG - Apex 1.1 Support for maximum destructible shape count
		case ASC_APEXDESTRUCTIBLEMAXDYNAMICCHUNKISLAND:
			if ( cmd )
			{
				// Set new destructible max chunk setting
				physx::apex::NxModuleDestructible *DestructibleModule = GApexManager->GetModuleDestructible();
				if ( DestructibleModule  )
				{
					UINT MaxChunkCount = 0;
					if ( 0 == appStricmp(ANSI_TO_TCHAR(cmd), TEXT("default")) )
					{
						MaxChunkCount = GSystemSettings.ApexDestructionMaxChunkIslandCount;
					}
					else
					{
						MaxChunkCount = appAtoi(ANSI_TO_TCHAR(cmd));
					}

					DestructibleModule->setMaxDynamicChunkIslandCount( MaxChunkCount );

					Ar->Logf( NAME_DevPhysics, TEXT("ApexDestructibleMaxDynamicChunkIsland(%d)"), MaxChunkCount );
				}
			}
			else
			{
				// Show usage
				Ar->Logf( NAME_DevPhysics, TEXT("ApexDestructibleMaxDynamicChunkIsland [newMaxChunkCount]") );
			}
			break;
		case ASC_APEXDESTRUCTIBLEMAXCHUNK:
			if ( cmd )
			{
				// Set new destructible max chunk setting
				physx::apex::NxModuleDestructible *DestructibleModule = GApexManager->GetModuleDestructible();
				if ( DestructibleModule  )
				{
					UINT MaxChunkCount = 0;
					if ( 0 == appStricmp(ANSI_TO_TCHAR(cmd), TEXT("default")) )
					{
						MaxChunkCount = GSystemSettings.ApexDestructionMaxShapeCount;
					}
					else
					{
						MaxChunkCount = appAtoi(ANSI_TO_TCHAR(cmd));
					}

					DestructibleModule->setMaxChunkCount( MaxChunkCount );

					Ar->Logf( NAME_DevPhysics, TEXT("ApexDestructibleMaxChunk(%d)"), MaxChunkCount );
				}
			}
			// NVCHANGE_END: BRG - Apex 1.1 Support for maximum destructible shape count
			else
			{
				// Show usage
				Ar->Logf( NAME_DevPhysics, TEXT("ApexDestructibleMaxChunk [newMaxChunkCount]") );
			}
			break;
    	case ASC_APEX_SHOW:
    		bShowApex = bShowApex ? FALSE : TRUE;
			Ar->Logf(NAME_DevPhysics, TEXT("ShowApex=%s"), bShowApex ? TEXT("TRUE") : TEXT("FALSE") );
			break;
		// NVCHANGE_BEGIN: DMS - Debug visualization fixes
		case ASC_APEXVIS:
			if ( cmd != NULL && ApexScenes.size() )
			{
				cmd = GetCommand(cmd,command);
				for (UINT sno=0; sno<ApexScenes.size(); sno++)
				{
					FIApexScene *fscene = ApexScenes[sno];
					if ( fscene == NULL ) continue;
					NxApexScene *apexScene = fscene->GetApexScene();
					if ( apexScene == NULL ) continue;

					bool visualizeThis = false;
					bool visualizeAny = false;
					FLOAT visScale = 0.0f;
					NxParameterized::Interface *sceneDebugRenderParams=NULL;
					sceneDebugRenderParams = apexScene->getDebugRenderParams();
					NxParameterized::getParamF32(*sceneDebugRenderParams,"VISUALIZATION_SCALE",visScale);
					// set default visualization scale
					if(visScale == 0.0f)
					{
						visScale = U2PScale;
						NxParameterized::setParamF32( *sceneDebugRenderParams,"VISUALIZATION_SCALE",visScale );
					}
					UBOOL debugOffApex  = strcmp(command,"APEX_CLEAR_ALL") == 0;
					for (UINT m=0; m<=MODULE_COUNT; m++)
					{
						NxParameterized::Interface *debugRenderParams=NULL;

						const char *moduleName = "ApexSDK";

						if ( m == 0 )
						{
							debugRenderParams = apexScene->getDebugRenderParams();
						}
						else
						{
							debugRenderParams = apexScene->getModuleDebugRenderParams(moduleNames[m-1]);
							moduleName = moduleNames[m-1];
						}

						if ( debugRenderParams )
						{
							UINT count = debugRenderParams->numParameters();
							for (UINT i=0; i<count; i++)
							{
								const NxParameterized::Definition* child = debugRenderParams->parameterDefinition(i);
								const char* name = child->name();

								if (IsNameToIgnore(name))
								{
									continue;
								}
								if ( debugOffApex )
								{
									switch ( child->type() )
									{
									case NxParameterized::TYPE_BOOL:
										NxParameterized::setParamBool(*debugRenderParams, name, false);
										break;
									case NxParameterized::TYPE_F32:
										NxParameterized::setParamF32(*debugRenderParams, name, 0.0f);
										break;
									case NxParameterized::TYPE_ARRAY:
										// do nothing with array types.
										break;
									case NxParameterized::TYPE_REF:
										// do nothing
										break;
									case NxParameterized::TYPE_STRUCT:
										// do nothing
										break;
									// NVCHANGE_BEGIN: JCAO - DE5928 Fix Editor crashes when double-clicking on any skeletal mesh asset
									case NxParameterized::TYPE_U32:
										// do nothing
										break;
									case NxParameterized::TYPE_VEC3:
										// do nothing
										break;
									// NVCHANGE_END: JCAO - DE5928 Fix Editor crashes when double-clicking on any skeletal mesh asset
									default:
										PX_ALWAYS_ASSERT();
										break;
									}
								}
								else
								{
									if ( appStricmp(name, command) == 0 )
									{
										FLOAT vis = 0;
										switch ( child->type() )
										{
										case NxParameterized::TYPE_BOOL:
											{
												bool b;
												NxParameterized::getParamBool(*debugRenderParams, name, b);
												vis = b ? 1.0f : 0.0f;
											}
											break;
										case NxParameterized::TYPE_F32:
											NxParameterized::getParamF32(*debugRenderParams, name, vis);
											break;
										default:
											check(0);
											break;
										}
										vis = vis > 0 ? 0.0f : 1.0f;
										if ( cmd )
										{
											if ( appStricmp(cmd, "true") == 0 )
											{
												vis = 1.0f;
											}
											else if ( appStricmp(cmd, "false") == 0 )
											{
												vis = 0.0f;
											}
											else
											{
												vis = atof(cmd);
											}
										}
										visualizeThis = vis > 0.0f;

										switch ( child->type() )
										{
										case NxParameterized::TYPE_BOOL:
											NxParameterized::setParamBool(*debugRenderParams, name, vis > 0.0f);
											break;
										case NxParameterized::TYPE_F32:
											NxParameterized::setParamF32(*debugRenderParams, name, vis);
											break;
										case NxParameterized::TYPE_REF:
											// do nothing
											break;
										default:
											PX_ALWAYS_ASSERT();
											break;
										}
									}
									else
									{
										switch ( child->type() )
										{
										case NxParameterized::TYPE_BOOL:
											{
												bool b = false;
												NxParameterized::getParamBool(*debugRenderParams, name, b);
												visualizeAny |= b;
											}
											break;
										case NxParameterized::TYPE_F32:
											{
												float vis = 0.0f;
												NxParameterized::getParamF32(*debugRenderParams, name, vis);
												visualizeAny |= vis > 0.0f;
											}
											break;
										}
									}
								}
							}
						}
					}
					visualizeAny |= visualizeThis;
					NxParameterized::Interface *debugRenderParams = apexScene->getDebugRenderParams();
					NxParameterized::setParamBool( *debugRenderParams, "VISUALIZATION_ENABLE", visualizeAny );
					Ar->Logf(NAME_DevPhysics, TEXT("Apex Debug (Scene %d): apexvis %s %s"), sno, ANSI_TO_TCHAR(command), visualizeThis ? TEXT("TRUE") : TEXT("FALSE") );
					bVisualizeApex = visualizeAny ? TRUE : FALSE;
				}
				ret = TRUE;
			}
			// NVCHANGE_END: DMS - Debug visualization fixes
			break;
// NVCHANGE_BEGIN: hlanker - clothing actor recording
		case ASC_APEXCLOTHINGREC:
		{
			bClothingRecording = !bClothingRecording;
			ret = TRUE;
			break;
		}
// NVCHANGE_END: hlanker - clothing actor recording
		case ASC_NONE:
			ret = FALSE;
			break;
		}
		return ret;
	}


  	/**
	 * Returns the Apex Asset
  	 * @param	AssetName - Name of the asset. 
  	 *
  	 * @return	null if it fails, else a pointer to the APEX asset. 
  	**/
    virtual FIApexAsset * GetApexAsset(const char *fname,UBOOL incrementRefCount)
    {
        FIApexAsset *ret = NULL;

        if ( fname )
        {
			ApexAsset *found = (ApexAsset *)GNRP->findResource("AI_APEX_ASSET",fname);
			if ( found )
          	{
				if ( incrementRefCount )
				{
					found->RefCount++;
				}
            	ret = static_cast< FIApexAsset *>(found);
            }
#if 0
			else
			{
				physx::PxU32 count = 0;
				void **assets = GNRP->findAllResources("AI_APEX_ASSET",count);
				for (physx::PxU32 i=0; i<count; i++)
				{
					found = (ApexAsset *)assets[i];
					BOOL exactMatch;
					if ( found->NameMatch(NULL,fname,exactMatch) )
					{
						ret = static_cast< FIApexAsset *>(found);
						found->RefCount++;
						break;
					}
				}
			}
#endif
        }
        return ret;
    }

	/**
	* Returns the Apex Manager
	* @return	null if it fails, else returns a pointer to the FIApexManager.
	**/
	virtual FIApexManager * GetApexManager(void) const
	{
		return GApexManager;
	}

	/**
	 * Notifies the Apex Asset that ithas been removed
	 * @param [in,out]	Asset - If non-null, the asset. 
	**/
	void NotifyApexAssetGone(FIApexAsset *_aa)
	{
		ApexAsset *aa = static_cast< ApexAsset *>(_aa);
		for (TApexAssetVector::iterator i=PendingForceLoad.begin(); i!=PendingForceLoad.end(); ++i)
		{
			if ( (*i) == aa )
			{
				(*i) = NULL;
				break;
			}
		}
		if ( !bOnExit )
		{
			physx::PxU32 refCount = GNRP->releaseResource("AI_APEX_ASSET", aa->GetName() );
			PX_FORCE_PARAMETER_REFERENCE(refCount);
			check(refCount == 0);
		}
	}

	/**
	 * Returns the NxUserRenderer
	 * @return	null if it fails, a pointer to the NxUserRenderer
	**/
	virtual NxUserRenderer*	GetNxUserRenderer(void)
	{
		return ApexRender->GetNxUserRenderer();
	}

	/**
	 * Returns the Apex Render
	 * @return	null if it fails, else returns a pointer to the FIApexRender.
	**/
	virtual FIApexRender * GetApexRender(void)
	{
		return ApexRender;
	}

  	/**
	 * Returns the Apex Asset from memory
  	 * @param	AssetName - Name of the asset. 
  	 * @param	Data - The data. 
  	 * @param	DataLength - Length of the data. 
  	 *
  	 * @return	null if it fails, else returns the FIApexAsset from memory. 
  	**/
  	virtual FIApexAsset* GetApexAssetFromMemory(const char *fname,const void *data,physx::PxU32 dlen,const char *originalName,UApexAsset *obj)
  	{
  		FIApexAsset *ret = NULL;

		ApexAsset *aa = (ApexAsset *)GNRP->findResource("AI_APEX_ASSET",fname);
		if ( aa )
		{
			aa->NotifyAssetGoingAway();
		}
		aa = new ApexAsset(fname,data,dlen,NULL,originalName,obj);
		ret = static_cast< FIApexAsset *>(aa);
		GNRP->setResource("AI_APEX_ASSET",fname,aa,TRUE);
		return ret;
  	}

  	/**
	 * Returns the Apex Asset from NxParameterized::Interface
  	 * @param	AssetName - Name of the asset.
  	 * @param	Params - NxParameterized::Interface.
  	 *
  	 * @return	null if it fails, else returns the FIApexAsset from NxParameterized::Interface.
  	**/
  	virtual FIApexAsset * GetApexAssetFromParams(const char *fname,const void *params,const char *originalName,UApexAsset *obj)
	{
		FIApexAsset *ret = NULL;

		ApexAsset *aa = (ApexAsset *)GNRP->findResource("AI_APEX_ASSET",fname);
		if ( aa )
		{
        	aa->LoadFromParams(fname,fname,originalName,obj);
        	ret = static_cast< FIApexAsset *>(aa);
		}
		else
		{
        	ApexAsset *aa = new ApexAsset(fname,NULL,0,params,originalName,obj);
        	ret = static_cast< FIApexAsset *>(aa);
        	GNRP->setResource("AI_APEX_ASSET",fname,aa,TRUE);
        }
        return ret;
	}

	/**
	 * Sets the resource callback
	 * @param [in,out]	Callback - If non-null, a pointer to the callback function. 
	**/
	virtual void SetNxResourceCallback(NxResourceCallback *callback)
	{
		ResourceCallback = callback;
	}

	/**
	 * Releases the NxApexAsset
	 * @param [in,out]	Asset	If non-null, the asset. 
	 *
	 * @return	TRUE if it succeeds, FALSE if it fails. 
	**/
	virtual UBOOL ReleaseNxApexAsset(NxApexAsset *asset) // this apex asset instance is being requested to be released.
	{
		UBOOL ret = FALSE;

		physx::PxU32 count = 0;
		void **assets = GNRP->findAllResources("AI_APEX_ASSET",count);
		for (physx::PxU32 i=0; i<count; i++)
		{
			ApexAsset *aa = (ApexAsset *)assets[i];
			ret = aa->ReleaseNxApexAsset(asset);
			if ( ret )
			{
				break;
			}
		}

		return ret;
	}

	/**
	 * Returns the persistent name
	 * @param	String - The string.
	 *
	 * @return	null if it fails, else the persistent string.
	**/
	virtual const char * GetPersistentString(const char *str)
	{
		if ( str == 0 )
		{
		    str = "";
		}
		const char *ret = (const char *)GNRP->findResource("AI_PERSISTENT_STRING",str);
		if ( ret == 0 )
		{
			physx::PxU32 len = (physx::PxU32)strlen(str);
			char *tmp = (char *)appMalloc(len+1);
			memcpy(tmp,str,len+1);
			GNRP->setResource("AI_PERSISTENT_STRING",tmp,tmp,TRUE);
			ret = tmp;
		}
		return ret;
	}


	/**
	 * Check to see if Apex is being shown
	 * @return	TRUE if show apex, FALSE if not.
	**/
	virtual UBOOL IsShowApex(void) const { return bShowApex; };

	virtual UBOOL IsApexPaused(void) const { return bPaused; };

	/**
	 * Check to see if a command is an APEX command
	 * @param	Command	The command.
	 *
	 * @return	TRUE if apex command, FALSE if not.
	**/
	virtual UBOOL IsApexCommand(const char *cmd)
	{
		UBOOL ret = FALSE;

    	char command[512];
    	cmd = GetCommand(cmd,command);
    	ApexCommands asc = (ApexCommands)GNRP->findResourceU32("AI_COMMANDS",command);
    	if ( asc !=	ASC_NONE )
    	{
    		ret = TRUE;
    	}
    	return ret;
	}

	virtual UBOOL IsVisualizeApex(void) const
	{
		return bVisualizeApex;
	}

// NVCHANGE_BEGIN: hlanker - clothing actor recording
	virtual UBOOL IsClothingRecording()
	{
		return bClothingRecording;
	}
// NVCHANGE_END: hlanker - clothing actor recording

	virtual void NotifyAssetGoingAway(ApexAsset *asset)
	{
		for (TApexAssetVector::iterator i=PendingForceLoad.begin(); i!=PendingForceLoad.end(); ++i)
		{
			if ( (*i) == asset )
			{
				(*i) = NULL;
				break;
			}
		}
	}

	virtual void NotifyAssetImport(ApexAsset *asset)
	{
		for (TApexAssetVector::iterator i=PendingForceLoad.begin(); i!=PendingForceLoad.end(); ++i)
		{
			if ( (*i) == asset )
			{
				asset = NULL;
				break;
			}
		}
		if ( asset )
		{
			PendingForceLoad.push_back(asset);
		}
	}

	/**
	* Pump the ApexCommands system which will detect if any pending assets which have been recently loaded
	* need to have a 'force load' performed on them.  This is done because it must happen in the game thread.
	*/
	virtual void Pump(void)
	{
  	    GApexRender->ProcessFEnqueDataGameThread(); // clean up pending render resource buffers
		for (TUApexAssetVector::iterator i=UpdateMaterials.begin(); i!=UpdateMaterials.end(); ++i)
		{
			UApexAsset *ua = (*i);
			if ( ua )
			{
				ua->UpdateMaterials();
			}
		}
		UpdateMaterials.clear();

#if 1
		if ( !PendingForceLoad.empty() )
		{
			TApexAssetVector fload = PendingForceLoad; // make a copy of the pendling load list; because forcing a load can cause additional items to be added to the pending load list
			PendingForceLoad.clear();
			for (TApexAssetVector::iterator i=fload.begin(); i!=fload.end(); ++i)
			{
				if ( (*i) )
				{
					(*i)->ForceLoad();
				}
			}
		}
#endif
	}

	virtual void AddUpdateMaterials(UApexAsset *Asset) 
	{
		UBOOL found = FALSE;
		for (TUApexAssetVector::iterator i=UpdateMaterials.begin(); i!=UpdateMaterials.end(); ++i)
		{
			UApexAsset *ua = (*i);
			if ( ua == Asset )
			{
				found = TRUE;
				break;
			}
		}
		if ( !found )
		{
			UpdateMaterials.push_back(Asset);
		}

	}

	/***
	Notification that this clothing asset is about to be destroyed and should be removed from the pending material resolution queue.
	*/
	virtual void NotifyDestroy(UApexAsset *Asset) 
	{
		// NVCHANGE_BEGIN: JCAO - DE5584 unused material did not release from Apex Resource
		Asset->ReleaseApexMaterialResource();
		// NVCHANGE_END: JCAO - DE5584 unused material did not release from Apex Resource

		if ( GIsEditor )
		{
			physx::PxU32 count;
			void **assets = GNRP->findAllResources("AI_APEX_ASSET",count);
			for (physx::PxU32 i=0; i<count; i++)
			{
				ApexAsset *aa = (ApexAsset *)assets[i];
				UApexAsset *obj = aa->GetUE3Object();
				if ( obj != Asset )
				{
					if ( obj )
					{
						obj->RemoveNamedReference(Asset);
					}
				}
				else
				{
					aa->SetUE3Object(NULL);
				}
			}
		}

		for (TUApexAssetVector::iterator i=UpdateMaterials.begin(); i!=UpdateMaterials.end(); ++i)
		{
			UApexAsset *ua = (*i);
			if ( ua == Asset )
			{
				(*i) = NULL;
				break;
			}
		}
	}
	bool isAsciiText(const unsigned char *str) const
	{
		bool ret = true;
		while ( *str )
		{
			if ( *str < 32 || *str > 126 )
			{
				ret = false;
				break;
			}
			str++;
		}
		return ret;
	}

#if !WITH_APEX_SHIPPING
	virtual const char * GetApexAssetBuffer(physx::PxU32 objNumber,const void *Buffer,physx::PxU32 dlen,const void *&buffer,physx::PxU32 &bufferLen) 
	{
		const char *ret = NULL;
		buffer = NULL;
		bufferLen = 0;

		appFree(LastBuffer);
		appFree(LastName);
		LastBuffer = NULL;
		LastName = NULL;


		physx::PxU32 numObj=0;
		ApexAssetFormat fmt = GetApexAssetFormat(Buffer,dlen,numObj);
		if ( objNumber < numObj )
		{
			NxParameterized::Interface *iface = NULL;
			NxParameterized::Serializer *ser = NULL;
			switch ( fmt )
			{
				case AAF_DESTRUCTIBLE_LEGACY:
				case AAF_CLOTHING_LEGACY:
				case AAF_CLOTHING_MATERIAL_LEGACY:
				case AAF_RENDER_MESH_LEGACY:
					check(0);
					break;
				case AAF_DESTRUCTIBLE_XML:
				case AAF_CLOTHING_XML:
				case AAF_XML:
					{
						ser = GApexManager->GetApexSDK()->createSerializer(NxParameterized::Serializer::NST_XML);
					}
					break;
				case AAF_DESTRUCTIBLE_BINARY:
				case AAF_CLOTHING_BINARY:
				case AAF_BINARY:
					{
						ser = GApexManager->GetApexSDK()->createSerializer(NxParameterized::Serializer::NST_BINARY);
					}
					break;
			}
			if ( ser )
			{
				NxParameterized::Serializer::DeserializedData desData;
				physx::PxFileBuf *rbuff = GApexManager->GetApexSDK()->createMemoryReadStream(Buffer,dlen);
				NxParameterized::Serializer::ErrorType err = ser->deserialize(*rbuff,desData);
				GApexManager->GetApexSDK()->releaseMemoryReadStream(*rbuff);
 				if ( err == NxParameterized::Serializer::ERROR_NONE )
				{
					ser->release();
					ser = NULL;
					check( objNumber < desData.size() );
					if ( objNumber < desData.size() )
					{
						iface = desData[objNumber];
						physx::PxFileBuf *fbuffer = GApexManager->GetApexSDK()->createMemoryWriteStream();
						NxParameterized::Serializer *ser = GApexManager->GetApexSDK()->createSerializer(NxParameterized::Serializer::NST_BINARY);
						check(ser);
						const char *objName = iface->name();
						static physx::PxU32 oc = 0;
						if ( objName == NULL )
						{
							objName = GApexManager->GetCurrentImportFileName();
							iface->setName(objName);
						}
						if ( ser )
						{
							const NxParameterized::Interface *save = iface;
							ser->serialize(*fbuffer,&save,1);	
							ser->release();									
						}

						const void *data = GApexManager->GetApexSDK()->getMemoryWriteBuffer(*fbuffer,bufferLen);
						if ( data )
						{
							buffer = appMalloc(bufferLen);
							memcpy((void *)buffer,data,bufferLen);
							LastBuffer = (void *)buffer;
							physx::PxU32 slen = strlen(objName);
							LastName = (char *)appMalloc(slen+1);
							memcpy(LastName,objName,slen+1);
							char *scan = LastName;
							char *lastDot = NULL;
							while ( *scan )
							{
								if ( *scan == '.' )
								{
									lastDot = scan;
									*scan = '_';
								}
								scan++;
							}
							if ( lastDot )
							{
								*lastDot = 0;
							}
							ret = LastName;
						}

						if ( fbuffer )
						{
							GApexManager->GetApexSDK()->releaseMemoryWriteStream(*fbuffer);
						}

						for (physx::PxU32 i=0; i<numObj; i++)
						{
							NxParameterized::Interface *iface = desData[i];
							iface->destroy();
						}
					}
				}
				if ( ser )
				{
					ser->release();
				}
			}

		}

		return ret;
	}
#endif

	virtual ApexAssetFormat GetApexAssetFormat(const void *Buffer,physx::PxU32 dlen,physx::PxU32 &numObj)
	{
		ApexAssetFormat ret = AAF_LAST;

		numObj = 1;
		const physx::PxU32 *scan = (const physx::PxU32 *)Buffer;
		physx::PxU32 headerVersion = scan[0];
		physx::PxU32 stringLen     = scan[1];
		// tests legacy version...
		if (headerVersion >= 10 && headerVersion <= 48 && stringLen < 256 )
		{
			scan+=2;
			const char *str = (const char *)scan;
			char scratch[256];
			memcpy(scratch,str,stringLen);
			scratch[stringLen] = 0;
			physx::PxU32 slen = (physx::PxU32 )strlen(scratch);
			if ( slen == stringLen && isAsciiText((const unsigned char *)scratch) )
			{
				if ( strcmp(scratch,NX_CLOTHING_AUTHORING_TYPE_NAME) == 0 )
				{
					ret = AAF_CLOTHING_LEGACY;
				}
				else if ( strcmp(scratch,NX_DESTRUCTIBLE_AUTHORING_TYPE_NAME) == 0 )
				{
					ret = AAF_DESTRUCTIBLE_LEGACY;
				}
				else if ( strcmp(scratch,NX_RENDER_MESH_AUTHORING_TYPE_NAME) == 0 )
				{
					ret = AAF_RENDER_MESH_LEGACY;
				}
				else
				{
					check(0); // some unknown type!?
				}
			}
		}
		if ( ret == AAF_LAST )
		{
			::NxParameterized::Serializer::SerializeType stype = GApexManager->GetApexSDK()->getSerializeType(Buffer,dlen);
			if ( stype != ::NxParameterized::Serializer::NST_LAST )
			{
				::NxParameterized::Serializer *ser = GApexManager->GetApexSDK()->createSerializer(stype);
				check(ser);
				NxParameterized::Serializer::ErrorType err = ser->peekNumObjectsInplace(Buffer,dlen,numObj);
				if ( err == NxParameterized::Serializer::ERROR_NONE )
				{
					if ( numObj == 1 )
					{
						// ok.. we need to decipher what type it is...
						if ( stype == ::NxParameterized::Serializer::NST_XML )
						{
							char scratch[1024];
							const char *source = (const char *)Buffer;
							physx::PxU32 clen = dlen < 1023 ? dlen : 1023;
							// NVCHANGE_BEGIN: DJS - Replace physx::string with UE3's string processing functions
							appStrncpyANSI( scratch, (const char *)Buffer, clen );
							// NVCHANGE_END: DJS - Replace physx::string with UE3's string processing functions
							scratch[1023] = 0;
							// now look for the ascii identifier for a clothing asset.
							const char *cc = strstr(scratch,"\"ClothingAssetParameters\"");
							if ( cc )
							{
								ret = AAF_CLOTHING_XML;
							}
							else
							{
								const char *cc = strstr(scratch,"\"DestructibleAssetParameters\"");
								if ( cc )
								{
									ret = AAF_DESTRUCTIBLE_XML;
								}
								else 
								{
									ret = AAF_XML;
								}
							}
						}
						else
						{
							// need to find out the object type!  Unfortunately, with a binary object, the only way we can do so is by deserializing it.
							physx::PxFileBuf *ReadBuffer = GApexManager->GetApexSDK()->createMemoryReadStream(Buffer,dlen);
							::NxParameterized::Serializer::DeserializedData desData;
							ser->deserialize(*ReadBuffer,desData);
							if ( desData.size() )
							{
								NxParameterized::Interface *iface = desData[0];
								const char *className = iface->className();
								if ( strcmp(className,"DestructibleAssetParameters") == 0 )
								{
									ret = AAF_DESTRUCTIBLE_BINARY;
								}
								else if ( strcmp(className,"ClothingAssetParameters") == 0 )
								{
									ret = AAF_CLOTHING_BINARY;
								}
								else
								{
									ret = AAF_BINARY;
								}
								iface->destroy(); // release the interface
							}
							GApexManager->GetApexSDK()->releaseMemoryReadStream(*ReadBuffer);
						}
					}
					else
					{
						// if there is more than one object, then we always treat it as a generic for now...
						ret = (stype == ::NxParameterized::Serializer::NST_BINARY) ? AAF_BINARY : AAF_XML;
					}
					ser->release(); // release the serializer
				}
			}
		}

		if ( ret == AAF_LAST )
		{
			numObj = 0;
		}

		return ret;
	}

	virtual const char * namedReferenceCallback(const char *className,const char *namedReference)  
	{
		const char *ret = NULL;
		physx::PxU32 count;
		void **assets = GNRP->findAllResources("AI_APEX_ASSET",count);
		BOOL exactMatch=FALSE;

		for (physx::PxU32 i=0; i<count; i++)
		{
			ApexAsset *aa = (ApexAsset *)assets[i];
			if ( aa->NameMatch(className,namedReference,exactMatch) )
			{
				if ( !exactMatch )
				{
					ret = aa->GetAssetName();
				}
				break;
			}
		}
		return ret;
	}
#if WITH_APEX_PARTICLES
	virtual FIApexAsset * CreateDefaultApexAsset(const char *assetName,ApexAssetType type,UApexAsset *obj,UApexGenericAsset *dependency) 
	{
		FIApexAsset *asset = GetApexAsset(assetName,TRUE);
		if ( asset )
		{
			ApexAsset *aa = static_cast< ApexAsset *>(asset);
			aa->CreateDefaultApexAsset(type,obj,dependency);
		}
		else
		{
			ApexAsset *aa = new ApexAsset(assetName,type,obj,dependency);
			asset = static_cast< FIApexAsset *>(aa);
			GNRP->setResource("AI_APEX_ASSET",assetName,aa,TRUE);
		}
		return asset;
	}

// NVCHANGE_BEGIN: HL - Apex Emitter Asset Copy
	virtual FIApexAsset * CreateApexAsset(const char *assetName,ApexAssetType type,UApexAsset *obj,UApexGenericAsset *dependency, NxParameterized::Interface* iface) 
	{
		FIApexAsset *asset = GetApexAsset(assetName,TRUE);
		if ( asset )
		{
			ApexAsset *aa = static_cast< ApexAsset *>(asset);
			aa->CreateApexAsset(type,obj,dependency,iface);
		}
		else
		{
			ApexAsset *aa = new ApexAsset(assetName,type,obj,dependency,iface);
			asset = static_cast< FIApexAsset *>(aa);
			GNRP->setResource("AI_APEX_ASSET",assetName,aa,TRUE);
		}
		return asset;
	}
// NVCHANGE_END: HL - Apex Emitter Asset Copy
#endif

	virtual void AddApexScene(FIApexScene *scene) 
	{
		UBOOL added = FALSE;
		for (physx::PxU32 i=0; i<ApexScenes.size(); i++)
		{
			if ( ApexScenes[i] == NULL )
			{
				ApexScenes[i] = scene;
				added = TRUE;
				break;
			}
			check( ApexScenes[i] != scene );
		}
		if ( !added )
		{
			ApexScenes.push_back(scene);
		}

		InitDebugVisualizationNames();
	}

	virtual void RemoveApexScene(FIApexScene *scene) 
	{
		for (physx::PxU32 i=0; i<ApexScenes.size(); i++)
		{
			if ( ApexScenes[i] == scene )
			{
				ApexScenes[i] = NULL;
			}
		}
	}

// NVCHANGE_BEGIN: PHATT - Dynamically generated debug rendering menu
	void InitDebugVisualizationNames()
	{
		if (bDebugVisNamesInitialized)
			return;

		if (ApexScenes.size() == 0)
			return;

		FIApexScene* fscene = ApexScenes[0];
		if ( fscene == NULL )
			return;

		NxApexScene* apexScene = fscene->GetApexScene();
		if ( apexScene == NULL)
			return;

		bDebugVisNamesInitialized = TRUE;

		for (UINT i = 0; i < MODULE_COUNT; ++i)
		{
			// module params
			DebugParamIndex.Set(moduleNames[i], i); 
			NxParameterized::Interface *debugRenderParams = apexScene->getModuleDebugRenderParams(moduleNames[i]);
			if (debugRenderParams != NULL)
			{
				UINT count;
				const NxParameterized::ParamResult* results = NxParameterized::getParamList(*debugRenderParams, NULL, NULL, count, false, false, ApexSDK->getParameterizedTraits());
				for (UINT j = 0; j < count; ++j)
				{
					const char* name = results[j].mName;

					if (!IsNameToIgnore(name))
						DebugParams[i].AddItem(name);
				}
			}
		}


		DebugParamIndex.Set("", MODULE_COUNT); // apex params
		NxParameterized::Interface *debugRenderParams = apexScene->getDebugRenderParams();
		if (debugRenderParams != NULL)
		{
			UINT count;
			const NxParameterized::ParamResult* results = NxParameterized::getParamList(*debugRenderParams, NULL, NULL, count, false, false, ApexSDK->getParameterizedTraits());
			for (UINT i = 0; i < count; ++i)
			{
				const char* name = results[i].mName;

				if (!IsNameToIgnore(name))
					DebugParams[MODULE_COUNT].AddItem(name);
			}
		}
	}

	virtual UINT GetNumDebugVisualizationNames(const char* moduleName)
	{
		if (!bDebugVisNamesInitialized)
			return 0;

		if (moduleName == 0)
			moduleName = "";

		return DebugParams[DebugParamIndex.FindRef(moduleName)].Num();
	}

	virtual const char* GetDebugVisualizationName(const char* moduleName, UINT i)
	{
		if (!bDebugVisNamesInitialized)
			return 0;

		if (moduleName == 0)
			moduleName = "";

		return DebugParams[DebugParamIndex.FindRef(moduleName)](i);
	}

	virtual void GetDebugVisualizationNamePretty(FString& prettyName, const char* moduleName, UINT i)
	{
		getPrettyName(prettyName, GetDebugVisualizationName(moduleName, i));
	}


	void getPrettyName(FString& output, const char* uglyName)
	{
		if (strncmp(uglyName, "VISUALIZE_CLOTHING_", 19) == 0)
		{
			uglyName += 19;
		}

		bool capital = true;
		bool wasLower = false;

		while (*uglyName != 0)
		{
			if (*uglyName == '_')
			{
				capital = true;
				output.AppendChar(' ');
			}
			else if (wasLower && isupper(*uglyName))
			{
				output.AppendChar(' ');
				output.AppendChar(*uglyName);
			}
			else
			{
				char letter = capital ? *uglyName : tolower(*uglyName);
				output.AppendChar(letter);
				capital = false;
			}

			wasLower = islower(*uglyName) != 0;
			uglyName++;
		}
	}
// NVCHANGE_END: PHATT - Dynamically generated debug rendering menu

#if WITH_APEX_PARTICLES
	virtual void * GetStaticMesh(void) 
	{
		if ( StaticMesh == NULL )
		{
			InitUObjects();
		}
		return StaticMesh;
	}

	void NotifyAssetGoingAway(FIApexAsset *asset)
	{
		for (TApexSceneVector::iterator i=ApexScenes.begin(); i!= ApexScenes.end(); ++i)
		{
			FIApexScene *scene = *(i);
			if ( scene )
			{
				scene->NotifyAssetGoingAway(asset);
			}
		}
	}

	void NotifyAssetModified(FIApexAsset *asset)
	{
		for (TApexSceneVector::iterator i=ApexScenes.begin(); i!= ApexScenes.end(); ++i)
		{
			FIApexScene *scene = *(i);
			if ( scene )
			{
				scene->NotifyAssetModified(asset);
			}
		}

		asset->UpdateBoundingBoxExpansion();
	}

#endif

	virtual UApexAsset * GetAssetObject(physx::apex::NxApexAsset *asset)
	{
		UApexAsset *ret = NULL;
		physx::PxU32 count = 0;
		void **assets = GNRP->findAllResources("AI_APEX_ASSET",count);
		for (physx::PxU32 i=0; i<count; i++)
		{
			ApexAsset *aa = (ApexAsset *)assets[i];
			if ( aa->GetNxApexAsset(FALSE) == asset )
			{
				ret = aa->GetUE3Object();
				break;
			}
		}
		return ret;
	}

	// NVCHANGE_BEGIN: JCAO - Fix assertion during cooking
	virtual void SaveNvApexAsset(const char* assetName, physx::apex::NxApexAsset* nxApexAsset)
	{
		NxApexAssetLookup.Set(assetName, nxApexAsset);
	}
	
	virtual physx::apex::NxApexAsset* GetNvApexAssetByName(const char* assetName)
	{
		NxApexAsset** ExistingNxApexAsset = NxApexAssetLookup.Find(assetName);
		if(ExistingNxApexAsset)
		{
			return *ExistingNxApexAsset;
		}
		return NULL;
	}
	// NVCHANGE_END: JCAO - Fix assertion during cooking

private:
#if WITH_APEX_PARTICLES
	virtual void GetUniqueAssetName(ApexAssetType type,FString &name) 
	{
		const char *baseName = "APEX_ASSET";
		switch ( type )
		{
			case AAT_APEX_EMITTER:
				baseName = "APEX_EMITTER";
				break;
			case AAT_IOFX_SPRITE:
				baseName = "APEX_IOFX_SPRITE";
				break;
			case AAT_IOFX_MESH:
				baseName = "APEX_IOFX_MESH";
				break;
			default:
				check(0);
				break;
		}
		for (physx::PxU32 i=0; i<10000; i++)
		{
			char scratch[512];
			appSprintfANSI(scratch,"%s_%04d.apb", baseName, i+1 );
			const char *ret = NULL;
			physx::PxU32 count;
			void **assets = GNRP->findAllResources("AI_APEX_ASSET",count);
			UBOOL found = FALSE;
			for (physx::PxU32 j=0; j<count; j++)
			{
				ApexAsset *aa = (ApexAsset *)assets[j];
				if ( aa->BaseNameMatch(scratch) )
				{
					found = TRUE;
					break;
				}
			}
			if ( !found )
			{
				appSprintfANSI(scratch,"%s_%04d", baseName, i+1 );
				name = ANSI_TO_TCHAR(scratch);
				break;
			}
		}
	}


	void InitUObjects(void)
	{
		StaticMesh = (UStaticMesh *)UObject::StaticLoadObject(UStaticMesh::StaticClass(),NULL,TEXT("EngineApexResources.ApexParticleCube"),NULL,LOAD_None,NULL);
	}
	UStaticMesh					*StaticMesh;
#endif
	void						*LastBuffer;
	char						*LastName;
	UBOOL					   bShowApex;
	UBOOL					   bOnExit;
  	UBOOL 					   bVisualizeApex;
// NVCHANGE_BEGIN: hlanker - clothing actor recording
	UBOOL 					   bClothingRecording;
// NVCHANGE_END: hlanker - clothing actor recording
    UBOOL                      bFreshCommand;
    NxApexSDK                 *ApexSDK;
	FIApexRender              *ApexRender;
	NxResourceCallback		  *ResourceCallback;
	TApexAssetVector		   PendingForceLoad;
// NVCHANGE_BEGIN: PHATT - Dynamically generated debug rendering menu
	UBOOL						bDebugVisNamesInitialized;
	TMap<FString, UINT>			DebugParamIndex;
	TArray<const char*>			DebugParams[MODULE_COUNT+1];
// NVCHANGE_END: PHATT - Dynamically generated debug rendering menu
	UBOOL						bPaused;
	TUApexAssetVector			UpdateMaterials;
	TApexSceneVector			ApexScenes;
	// NVCHANGE_BEGIN: JCAO - Fix assertion during cooking
	TMap<FString, NxApexAsset*> NxApexAssetLookup;
	// NVCHANGE_END: JCAO - Fix assertion during cooking
};

/**
 * Implementation of FIApexAsset
**/
ApexAsset::ApexAsset(const char *fname,const void *data,physx::PxU32 dlen,const void* params, const char *originalName,UApexAsset *obj)
{
#if WITH_APEX_PARTICLES
	bEdited = FALSE;
	bEditPending = FALSE;
	ApexEditInterface = NULL;
	bTweaked = FALSE;
	NewInterface = NULL;
	TweakInterface = NULL;
#endif
	Object = obj;
	bForceLoad = FALSE;
	physx::PxU32 slen = (physx::PxU32)strlen(fname);
	OriginalApexName = NULL;
	Name = (char *)appMalloc(slen+1);
	memcpy(Name,fname,slen+1);
  	RefCount = 0;
  	Type = AAT_LAST;
  	ApexAssetObject = NULL;
// NVCHANGE_BEGIN: KHA - Apex 1.1 Update to Apex 1.1 interface variable type changes
	Transformation = physx::PxMat44::createIdentity();
// NVCHANGE_END: KHA - Apex 1.1 Update to Apex 1.1 interface variable type changes

  	if ( data )
  	{
  		LoadFromMemory(data,dlen,fname,originalName,obj);
  	}
	else if ( params )
	{
		LoadFromParams(params,fname,originalName,obj);
	}
	else
	{
		// NVCHANGE_BEGIN : hlanker - delete invalid assets 
		UObject* Object = obj;
		if ( UObject::IsReferenced( Object, GARBAGE_COLLECTION_KEEPFLAGS ) )
		{
			warnf(NAME_Warning, TEXT("ApexAsset (%s) is created without data. Object can not be deleted because it is referenced by other objects:"));

			// We cannot safely delete this object. Print out a list of objects referencing this one
			// that prevent us from being able to delete it.
			FStringOutputDevice Ar;
			obj->OutputReferencers(Ar, FALSE);
			warnf( TEXT("%s"), *Ar );		
		}
		else
		{
			warnf(NAME_Warning, TEXT("ApexAsset (%s) is created without data. Marking object for deletion."), *FString(fname));

			// Mark its package as dirty as we're going to delete the obj.
			obj->MarkPackageDirty();
			// Remove standalone flag so garbage collection can delete the object.
			obj->ClearFlags( RF_Standalone );
		}
		// NVCHANGE_END : hlanker - delete invalid assets
	}

#if WITH_APEX_PARTICLES
	UpdateBoundingBoxExpansion();
#endif
}

#if WITH_APEX_PARTICLES
ApexAsset::ApexAsset(const char *fname,ApexAssetType type,UApexAsset *obj,UApexGenericAsset *child)
{
	bEdited = FALSE;
	bEditPending = FALSE;
	NewInterface = NULL;
	TweakInterface = NULL;
	ApexEditInterface = NULL;
	Object = obj;
	bForceLoad = FALSE;
	physx::PxU32 slen = (physx::PxU32)strlen(fname);
	OriginalApexName = NULL;
	Name = (char *)appMalloc(slen+1);
	memcpy(Name,fname,slen+1);
	setOriginalName(Name);
	RefCount = 0;
	Type = AAT_LAST;

	ApexAssetObject = NULL;
	Transformation = physx::PxMat44::createIdentity();
	CreateDefaultApexAsset(type,obj,child);

	UpdateBoundingBoxExpansion();
}

// NVCHANGE_BEGIN: HL - Apex Emitter Asset Copy
ApexAsset::ApexAsset(const char *fname,ApexAssetType type,UApexAsset *obj,UApexGenericAsset *dependency,NxParameterized::Interface* iface)
{
	bEdited = FALSE;
	bEditPending = FALSE;
	NewInterface = NULL;
	TweakInterface = NULL;
	ApexEditInterface = NULL;
	Object = obj;
	bForceLoad = FALSE;
	physx::PxU32 slen = (physx::PxU32)strlen(fname);
	OriginalApexName = NULL;
	Name = (char *)appMalloc(slen+1);
	memcpy(Name,fname,slen+1);
	setOriginalName(Name);
	RefCount = 0;
	Type = AAT_LAST;
	ApexAssetObject = NULL;
	Transformation = physx::PxMat44::createIdentity();
	CreateApexAsset(type,obj,dependency, iface);

	UpdateBoundingBoxExpansion();
}
// NVCHANGE_END: HL - Apex Emitter Asset Copy

const char* ApexAsset::GetAuthoringTypeName(ApexAssetType type)
{
	const char *assetName = NULL;
	switch ( type )
	{
  		case AAT_DESTRUCTIBLE:	// used to construct destructible actors
  			assetName =	NX_DESTRUCTIBLE_AUTHORING_TYPE_NAME; 
  			break;
  		case AAT_CLOTHING:		// asset used to construct clothing actors
  			assetName = NX_CLOTHING_AUTHORING_TYPE_NAME;
  			break;
  		case AAT_APEX_EMITTER:		// asset used to construct emitters.
  			assetName = NX_APEX_EMITTER_AUTHORING_TYPE_NAME;
  			break;
		case AAT_GROUND_EMITTER:
			assetName = NX_GROUND_EMITTER_AUTHORING_TYPE_NAME;
			break;
		case AAT_IMPACT_EMITTER:
			assetName = NX_IMPACT_EMITTER_AUTHORING_TYPE_NAME;
			break;
			// NVCHANGE_BEGIN: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
#if WITH_APEX_FLUID_IOS
		case AAT_FLUID_IOS:
			assetName = NX_FLUID_IOS_AUTHORING_TYPE_NAME;
			break;
#endif
			// NVCHANGE_END: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
  		case AAT_BASIC_IOS:
  			assetName = NX_BASIC_IOS_AUTHORING_TYPE_NAME;
  			break;
		case AAT_IOFX_SPRITE:
		case AAT_IOFX_MESH:
  			assetName = NX_IOFX_AUTHORING_TYPE_NAME;
  			break;
  		case AAT_RENDER_MESH:
  			assetName = NX_RENDER_MESH_AUTHORING_TYPE_NAME;
  			break;
  		default:
			check(0);
  			break;
	}
	return assetName;
}

void ApexAsset::CreateDefaultApexAsset(ApexAssetType type,UApexAsset *obj,UApexGenericAsset *dependency)
{
	Object = obj;

	if ( Object )
	{
		Object->Modify();
		Object->MarkPackageDirty();
	}


	NotifyAssetGoingAway();
	Type = AAT_LAST;
  	NxApexSDK *apexSDK = GApexManager->GetApexSDK();
	if ( ApexAssetObject )
	{
		apexSDK->releaseAsset(*ApexAssetObject);
		ApexAssetObject = NULL;
	}

	const char *assetName = GetAuthoringTypeName(type);
	if ( assetName )
	{
		NxApexAssetAuthoring *authoring = apexSDK->createAssetAuthoring(assetName);
		check(authoring);
		if ( authoring )
		{
			NxParameterized::Interface *iface = authoring->getNxParameterized();
			iface->setName(Name);
			switch ( type )
			{
				case AAT_BASIC_IOS:
					{
						NxParameterized::setParamF32(*iface,"restDensity",1);
						NxParameterized::setParamF32(*iface,"particleRadius",0.1f);
						NxParameterized::setParamF32(*iface,"maxInjectedParticleCount",0.1f);
						// NVCHANGE_BEGIN: JCAO - US8900 BasicIOS collision channel support
						NxParameterized::setParamString(*iface,"collisionFilterDataName",Name);
						// NVCHANGE_END: JCAO - US8900 BasicIOS collision channel support
						// NVCHANGE_BEGIN: JCAO - Add field sampler group filter
						NxParameterized::setParamString(*iface,"fieldSamplerFilterDataName",Name);
						// NVCHANGE_END: JCAO - Add field sampler group filter
					}
					break;
					// NVCHANGE_BEGIN: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
#if WITH_APEX_FLUID_IOS
				case AAT_FLUID_IOS:
					{
						NxParameterized::setParamString(*iface,"collisionGroupName","PhysXFluidCollisionGroup");
						NxParameterized::setParamF32(*iface,"particleRadius", 0.1f);
					}
					break;
#endif
					// NVCHANGE_END: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
				case AAT_APEX_EMITTER:
					{
						UBOOL ok = false;
						NxParameterized::Handle handle(*iface);
						if ( NxParameterized::findParam(*iface,"iosAssetName",handle) )
						{
							NxParameterized::Interface *paramRef;
							// NVCHANGE_BEGIN: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
#if WITH_APEX_FLUID_IOS
							if ( handle.initParamRef("NxFluidIosAsset") == NxParameterized::ERROR_NONE )
							{
								if ( handle.getParamRef(paramRef) == NxParameterized::ERROR_NONE )
								{
									paramRef->setName("DefaultFluidIOS");
									ok = TRUE;
								}
							}
#else
							if ( handle.initParamRef("NxBasicIosAsset") == NxParameterized::ERROR_NONE )
							{
								if ( handle.getParamRef(paramRef) == NxParameterized::ERROR_NONE )
								{
									paramRef->setName("DefaultBasicIOS");
									ok = TRUE;
								}
							}
#endif
							// NVCHANGE_END: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
						}
						if ( ok && NxParameterized::findParam(*iface,"iofxAssetName",handle) )
						{
							NxParameterized::Interface *paramRef;
							if ( handle.initParamRef("IOFX") == NxParameterized::ERROR_NONE )
							{
								if ( handle.getParamRef(paramRef) == NxParameterized::ERROR_NONE )
								{
									if ( dependency && dependency->MApexAsset )
									{
										paramRef->setName( dependency->MApexAsset->GetAssetName() );
									}
									else
									{
										paramRef->setName("DefaultIOFX");
									}
									ok = TRUE;
								}
							}
						}
						if ( ok )
						{
							NxParameterized::setParamF32(*iface,"rateRange.min",10);
							NxParameterized::setParamF32(*iface,"rateRange.max",10000);
							NxParameterized::setParamVec3(*iface,"velocityRange.min",physx::PxVec3(0,0,0));
							NxParameterized::setParamVec3(*iface,"velocityRange.max",physx::PxVec3(0,0,0));
							NxParameterized::setParamF32(*iface,"lifetimeRange.min",2);
							NxParameterized::setParamF32(*iface,"lifetimeRange.max",5);
							NxParameterized::setParamU32(*iface,"maxSamples",100);
							NxParameterized::setParamF32(*iface,"lodParamDesc.maxDistance",10000);
							ok = FALSE;
							NxParameterized::Handle handle(*iface);
							if ( NxParameterized::findParam(*iface,"geometryType",handle) )
							{
								NxParameterized::Interface *paramRef;
								if ( handle.initParamRef("EmitterGeomSphereParams") == NxParameterized::ERROR_NONE )
								{
									if ( handle.getParamRef(paramRef) == NxParameterized::ERROR_NONE )
									{
										NxParameterized::setParamF32(*iface,"geometryType.radius",0.02f);
										ok = TRUE;
									}
								}

							}
						}
						check(ok);
					}
					break;
				case AAT_IOFX_SPRITE:
				case AAT_IOFX_MESH:
					{
						UBOOL ok = FALSE;
						NxParameterized::Handle handle(*iface);
						if ( type == AAT_IOFX_MESH )
						{
							if ( NxParameterized::findParam(*iface,"iofxType",handle) )
							{
								if ( handle.initParamRef("MeshIofxParameters") == NxParameterized::ERROR_NONE )
								{
									if ( NxParameterized::findParam(*iface,"iofxType.renderMeshList",handle) )
									{
										handle.resizeArray(1);
										NxParameterized::setParamU32(*iface,"iofxType.renderMeshList[0].weight",1);
										if ( NxParameterized::findParam(*iface,"iofxType.renderMeshList[0].meshAssetName",handle))
										{
											NxParameterized::Interface *paramRef;
											if ( handle.initParamRef("ApexOpaqueMesh") == NxParameterized::ERROR_NONE )
											{
												if ( handle.getParamRef(paramRef) == NxParameterized::ERROR_NONE )
												{
													paramRef->setName("EngineApexResources.ApexParticleCube");
													ok = TRUE;
												}
											}
										}
									}
								}
							}
						}
						else
						{
							if ( NxParameterized::findParam(*iface,"iofxType",handle) )
							{
								if ( handle.initParamRef("SpriteIofxParameters") == NxParameterized::ERROR_NONE )
								{
									if ( NxParameterized::findParam(*iface,"iofxType.spriteMaterialName",handle))
									{
										NxParameterized::Interface *paramRef;
										if ( handle.initParamRef(NULL) == NxParameterized::ERROR_NONE )
										{
											if ( handle.getParamRef(paramRef) == NxParameterized::ERROR_NONE )
											{
												paramRef->setName("EngineApexResources.DefaultApexParticle");
												ok = TRUE;
											}
										}
									}
								}
							}
						}
						check(ok);
					}
					break;
			}
			SetNxApexAsset( apexSDK->createAsset(*authoring,Name) );
			apexSDK->releaseAssetAuthoring(*authoring);
		}
	}
	if ( GetNxApexAsset() )
	{
		Type = type;
		NotifyAssetImport();
	}
}

// NVCHANGE_BEGIN: HL - Apex Emitter Asset Copy
void ApexAsset::CreateApexAsset(ApexAssetType type,UApexAsset *obj,UApexGenericAsset *dependency, NxParameterized::Interface* iface)
{
	Object = obj;

	if ( Object )
	{
		Object->Modify();
		Object->MarkPackageDirty();
	}

	NotifyAssetGoingAway();
	Type = AAT_LAST;
	NxApexSDK *apexSDK = GApexManager->GetApexSDK();
	if ( ApexAssetObject )
	{
		apexSDK->releaseAsset(*ApexAssetObject);
		ApexAssetObject = NULL;
	}

	if (type == AAT_APEX_EMITTER)
	{
		NxParameterized::Handle handle(*iface);
		if ( NxParameterized::findParam(*iface,"iofxAssetName",handle) )
		{
			NxParameterized::Interface *paramRef;
			if ( handle.initParamRef("IOFX") == NxParameterized::ERROR_NONE )
			{
				if ( handle.getParamRef(paramRef) == NxParameterized::ERROR_NONE )
				{
					if ( dependency && dependency->MApexAsset )
					{
						paramRef->setName( dependency->MApexAsset->GetAssetName() );
					}
				}
			}
		}
	}

	SetNxApexAsset( apexSDK->createAsset(iface, Name) );

	if ( GetNxApexAsset() )
	{
		Type = type;
		NotifyAssetImport();
	}
}
// NVCHANGE_END: HL - Apex Emitter Asset Copy
#endif

/**
 * Destructor
**/
ApexAsset::~ApexAsset(void)
{
#if WITH_APEX_PARTICLES
	if ( ApexEditInterface )
	{
		ApexEditInterface->NotifyObjectDestroy(Object);
	}
#endif
	GApexCommands->NotifyApexAssetGone(this);
	if ( ApexAssetObject )
	{
		NxApexSDK *sdk = GApexManager->GetApexSDK();
		sdk->releaseAsset(*ApexAssetObject);
	}
	appFree(Name);
	appFree(OriginalApexName);
}

/**
* Notifies the asset that it is being removed
**/
void ApexAsset::NotifyAssetGoingAway(void)
{
	bForceLoad = FALSE; // clear the 'force load' semaphore
	MyApexCommands *mc = static_cast< MyApexCommands *>(GApexCommands);
	mc->NotifyAssetGoingAway(this);
	{
    	for (TApexActorVector::iterator i=Actors.begin(); i!=Actors.end(); ++i)
        {
          FIApexActor *Act = (*i);
          if ( Act )
          {
            Act->NotifyAssetGone();
          }
        }
	}
#if WITH_APEX_PARTICLES
#if !WITH_APEX_SHIPPING
	switch ( Type )
	{
		case AAT_BASIC_IOS: 
		// NVCHANGE_BEGIN: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
#if WITH_APEX_FLUID_IOS
		case AAT_FLUID_IOS:
#endif
		// NVCHANGE_END: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
		case AAT_IOFX_MESH:
		case AAT_IOFX_SPRITE:
			GApexCommands->NotifyAssetGoingAway(this);
			break;
	}
#endif
#endif
	// if there is any apex actors that are still pending deferred deletion, process the deletion 
	if(FApexCleanUp::PendingObjects)
	{
		FlushRenderingCommands();
	}
}

// Set to 1 to write out ACA, PDA, ACML, etc files
#define DUMP_APEX_FILES 0

/**
* Loads the asset from memory
* @param	Mem	- a pointer to the memory location.
* @param	Length - 	the length of the asset.
*
* @return	TRUE if it succeeds, FALSE if it fails.
**/
UBOOL ApexAsset::LoadFromMemory(const void *data,physx::PxU32 dlen,const char *assetName,const char *originalName,UApexAsset *obj)
{
#if ALLOW_DEBUG_FILES && DUMP_APEX_FILES
	{
		// dump file
		FString OutputFileName = appGameLogDir() + FString( assetName );
		FArchive* OutputFile = GFileManager->CreateDebugFileWriter( *OutputFileName );
		if( OutputFile != NULL )
		{
			OutputFile->Serialize( const_cast<void*>(data), dlen );
			delete OutputFile;
		}
	}
#endif

	check(obj == Object );

	// NVCHANGE_BEGIN: APAN - Code review improve performance
	if ( originalName == NULL || originalName[0] == '\0' )
	// NVCHANGE_END: APAN - Code review improve performance
	{
		originalName = assetName;
	}
#if !WITH_APEX_PARTICLES
	originalName = assetName;
#endif
	setOriginalName(originalName);

	physx::PxU32 numObj;
	ApexAssetFormat fmt = GApexCommands->GetApexAssetFormat(data,dlen,numObj);
	if ( fmt == AAF_LAST )
		return FALSE;
	check(numObj ==  1 ); // we should never have more than one apex asset embedded!
	if ( numObj != 1 )
		return FALSE;

	NotifyAssetGoingAway();
	Type = AAT_LAST;
	FIApexCommands *ic = GApexCommands;
  	check(ic);
  	FIApexManager *pm = ic->GetApexManager();
  	check(pm);
  	NxApexSDK *apexSDK = pm->GetApexSDK();
	if ( ApexAssetObject )
	{
#if WITH_APEX_PARTICLES
		ClearEdit();
#endif
		apexSDK->releaseAsset(*ApexAssetObject);
		SetNxApexAsset(NULL);
	}
	physx::PxFileBuf *mrb = GApexManager->GetApexSDK()->createMemoryReadStream(data,dlen);
	check(mrb);
	switch ( fmt )
	{
		case AAF_DESTRUCTIBLE_LEGACY:
		case AAF_CLOTHING_LEGACY:
		case AAF_CLOTHING_MATERIAL_LEGACY:
			if ( mrb )
			{
				// NVCHANGE_BEGIN: LRR - the legacy asset interface is gone in APEX 1.1
				warnf(NAME_Warning, TEXT("Legacy APEX Assets are no longer supported by APEX 1.1"));

				check(0);
				// SetNxApexAsset(apexSDK->createAsset(*mrb,assetName));

				// NVCHANGE_END: LRR - the legacy asset interface is gone in APEX 1.1
			}
			break;
		case AAF_DESTRUCTIBLE_BINARY:
		case AAF_CLOTHING_BINARY:
		case AAF_BINARY:
		case AAF_CLOTHING_XML:
		case AAF_DESTRUCTIBLE_XML:
		case AAF_XML:
			if ( mrb )
			{
				NxParameterized::Serializer::SerializeType type = NxParameterized::Serializer::NST_BINARY;
				if ( fmt == AAF_CLOTHING_XML ||	fmt == AAF_DESTRUCTIBLE_XML || fmt ==AAF_XML )
				{
					type = NxParameterized::Serializer::NST_XML;
				}
				NxParameterized::Serializer *ser = GApexManager->GetApexSDK()->createSerializer(type);
				check(ser);
				if ( ser )
				{
					::NxParameterized::Serializer::DeserializedData desData;
					bool bIsUpdated = false;
					ser->deserialize(*mrb,desData,bIsUpdated);
					// NVCHANGE_BEGIN : hlanker - print warning when asset needs to be resaved
					// NVCHANGE_END : hlanker - print warning when asset needs to be resaved

					check( desData.size());
					check( desData.size()==1);
					if ( ensure(desData.size()) )
					{
						::NxParameterized::Interface *iface = desData[0];
						const char *name = iface->name();
						if ( name )
						{
							setOriginalName(name);
						}
						SetNxApexAsset(apexSDK->createAsset(iface,assetName));
						// NVCHANGE_BEGIN: JCAO - Fix assertion during cooking
						if(GIsCooking == TRUE)
						{
							NxApexAsset* nxApexAsset = GetNxApexAsset();
							if(nxApexAsset)
							{
								GApexCommands->SaveNvApexAsset(assetName, nxApexAsset);
							}
							else
							{
								SetNxApexAsset(GApexCommands->GetNvApexAssetByName(assetName));
							}
						}
						// NVCHANGE_END: JCAO - Fix assertion during cooking
						check( GetNxApexAsset() );

						// NVCHANGE_BEGIN : hlanker - print warning when asset needs to be resaved
						bIsUpdated |= GetNxApexAsset()->isDirty();
						// NVCHANGE_END : hlanker - print warning when asset needs to be resaved
					}
					ser->release();

					// NVCHANGE_BEGIN : hlanker - print warning when asset needs to be resaved
					if (bIsUpdated && !GIsCooking)
					{
						warnf(NAME_DevPhysics, TEXT("ApexAsset %s (%s) has been updated and needs to be resaved. Use the ResavePackages commandlet or resave in editor."), *obj->GetPathName(), *FString(assetName));
						
						// Uncomment the following line if you want the asset's package to be 
						// automatically marked dirty when the APEX asset is out of date.
						//obj->MarkPackageDirty();
					}
					// NVCHANGE_END : hlanker - print warning when asset needs to be resaved
				}
			}
			break;
	}

	GApexManager->GetApexSDK()->releaseMemoryReadStream(*mrb);

	NxApexAsset* nxApexAsset = GetNxApexAsset();
	if ( nxApexAsset )
	{
		const char *typeName = nxApexAsset->getObjTypeName();
		if ( strcmp(typeName,NX_DESTRUCTIBLE_AUTHORING_TYPE_NAME) == 0 )
		{
			Type = AAT_DESTRUCTIBLE;
		}
		else if ( strcmp(typeName,NX_CLOTHING_AUTHORING_TYPE_NAME) == 0)
		{
			Type = AAT_CLOTHING;
		}
		else if ( strcmp(typeName,NX_RENDER_MESH_AUTHORING_TYPE_NAME) == 0 )
		{
			Type = AAT_RENDER_MESH;
		}
// NVCHANGE_BEGIN: DJS - APEX 1.2 - Clean up include paths
#if WITH_APEX_PARTICLES
		else if ( strcmp(typeName,NX_BASIC_IOS_AUTHORING_TYPE_NAME) == 0 )
		{
			Type = AAT_BASIC_IOS;
		}
		// NVCHANGE_BEGIN: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
#if WITH_APEX_FLUID_IOS
		else if ( strcmp(typeName,NX_FLUID_IOS_AUTHORING_TYPE_NAME) == 0 )
		{
			Type = AAT_FLUID_IOS;
		}
#endif
		// NVCHANGE_END: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
		else if ( strcmp(typeName,NX_APEX_EMITTER_AUTHORING_TYPE_NAME) == 0 )
		{
			Type = AAT_APEX_EMITTER;
		}
		else if ( strcmp(typeName,NX_IOFX_AUTHORING_TYPE_NAME ) == 0 )
		{
			NxIofxAsset* nxIofxAsset = (NxIofxAsset*)nxApexAsset;
			if (nxIofxAsset->getMeshAssetCount() == 0)
				Type = AAT_IOFX_SPRITE;
			else
				Type = AAT_IOFX_MESH;
		}
#endif
// NVCHANGE_END: DJS - APEX 1.2 - Clean up include paths
		else
		{
			PX_ALWAYS_ASSERT(); // unexpected asset type...
			Type = AAT_GENERIC;
		}
	}

	// NVCHANGE_BEGIN: JCAO - US8900 BasicIOS collision channel support
#if WITH_APEX_PARTICLES
	if(Type == AAT_BASIC_IOS)
	{
		if(nxApexAsset)
		{
			const NxParameterized::Interface *assetInterface = nxApexAsset->getAssetNxParameterized();
			NxParameterized::Handle handle(*assetInterface);
			if ( NxParameterized::findParam(*assetInterface, "collisionFilterDataName", handle) )
			{
				const char *collisionFilterDataName=NULL;
				handle.getParamString(collisionFilterDataName);
				if( collisionFilterDataName == NULL || strcmp(collisionFilterDataName, nxApexAsset->getName()) != 0)
				{
					NxParameterized::Interface *iface = nxApexAsset->releaseAndReturnNxParameterizedInterface();
					SetNxApexAsset(apexSDK->createAsset(iface,assetName));
					NxApexAsset* newNxApexAsset = GetNxApexAsset();
					if(newNxApexAsset)
					{
						NxParameterized::setParamString(*iface, "collisionFilterDataName", newNxApexAsset->getName());
						NxParameterized::setParamString(*iface, "fieldSamplerFilterDataName", newNxApexAsset->getName());
					}
				}
			}
		}
	}
#endif
	// NVCHANGE_END: JCAO - US8900 BasicIOS collision channel support

	UBOOL ret = IsValid();

	if ( ret )
	{
		// Note that this call will queue up a force load command, which will cause APEX to call the 
		// NRP getResource callback for any unknown resources such as materials. If this is done during 
		// async package loading the call to getResource will return NULL because the package is still 
		// loading. To prevent that from happening, call setResource before the call to forceLoadAssets 
		// has a chance to fire off. Then you won't get any getResource callback.
		NotifyAssetImport();
	}

	return ret;
}

/**
* Loads the asset from memory
* @param	Mem	- a pointer to the memory location.
* @param	Length - 	the length of the asset.
*
* @return	TRUE if it succeeds, FALSE if it fails.
**/
UBOOL ApexAsset::LoadFromParams(const void *params,const char *assetName,const char *originalName,UApexAsset *obj)
{
	PX_ASSERT(obj == Object );

	if ( originalName == NULL || originalName[0] == '\0' )
	{
		originalName = assetName;
	}
#if !WITH_APEX_PARTICLES
	originalName = assetName;
#endif
	setOriginalName(originalName);

	NotifyAssetGoingAway();
	Type = AAT_LAST;
	FIApexCommands *ic = GApexCommands;
  	PX_ASSERT(ic);
  	FIApexManager *pm = ic->GetApexManager();
  	PX_ASSERT(pm);
  	NxApexSDK *apexSDK = pm->GetApexSDK();
	if ( ApexAssetObject )
	{
		apexSDK->releaseAsset(*ApexAssetObject);
		ApexAssetObject = NULL;
	}
	
	::NxParameterized::Interface *iface = (::NxParameterized::Interface*)params;
	const char *name = iface->name();
	if ( name )
	{
		setOriginalName(name);
	}
	ApexAssetObject = apexSDK->createAsset(iface,assetName);
	PX_ASSERT(ApexAssetObject);

	if ( ApexAssetObject )
	{
		const char *typeName = ApexAssetObject->getObjTypeName();
		if ( strcmp(typeName,NX_DESTRUCTIBLE_AUTHORING_TYPE_NAME) == 0 )
		{
			Type = AAT_DESTRUCTIBLE;
		}
		else if ( strcmp(typeName,NX_CLOTHING_AUTHORING_TYPE_NAME) == 0)
		{
			Type = AAT_CLOTHING;
		}
		else if ( strcmp(typeName,NX_RENDER_MESH_AUTHORING_TYPE_NAME) == 0 )
		{
			Type = AAT_RENDER_MESH;
		}
// NVCHANGE_BEGIN: DJS - APEX particle support
#if WITH_APEX_PARTICLES
		else if ( strcmp(typeName,NX_BASIC_IOS_AUTHORING_TYPE_NAME) == 0 )
		{
			Type = AAT_BASIC_IOS;
		}
		// NVCHANGE_BEGIN: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
#if WITH_APEX_FLUID_IOS
		else if ( strcmp(typeName,NX_FLUID_IOS_AUTHORING_TYPE_NAME) == 0 )
		{
			Type = AAT_FLUID_IOS;
		}
#endif
		// NVCHANGE_END: JCAO - Use WITH_APEX_FLUID_IOS to control whether to load Fluid IOS Module and create Fluid IOS Asset
		else if ( strcmp(typeName,NX_APEX_EMITTER_AUTHORING_TYPE_NAME) == 0 )
		{
			Type = AAT_APEX_EMITTER;
		}
		else if ( strcmp(typeName,NX_IOFX_AUTHORING_TYPE_NAME ) == 0 )
		{
			NxIofxAsset* nxIofxAsset = (NxIofxAsset*)ApexAssetObject;
			if (nxIofxAsset->getMeshAssetCount() == 0)
				Type = AAT_IOFX_SPRITE;
			else
				Type = AAT_IOFX_MESH;
		}
#endif	// WITH_APEX_PARTICLES
// NVCHANGE_END: DJS - APEX particle support
		else
		{
			check(0); // unexpected asset type...
			Type = AAT_GENERIC;
		}
	}

	UBOOL ret = IsValid();

	if ( ret )
	{
		// Note that this call will queue up a force load command, which will cause APEX to call the 
		// NRP getResource callback for any unknown resources such as materials. If this is done during 
		// async package loading the call to getResource will return NULL because the package is still 
		// loading. To prevent that from happening, call setResource before the call to forceLoadAssets 
		// has a chance to fire off. Then you won't get any getResource callback.
		NotifyAssetImport();
	}

	return ret;
}

UBOOL ApexAsset::ReleaseNxApexAsset(NxApexAsset *apexAsset)
{
	UBOOL ret = FALSE;
	if ( apexAsset == ApexAssetObject )
	{
		NotifyAssetGoingAway();
		NxApexSDK *sdk = GApexManager->GetApexSDK();
		sdk->releaseAsset(*ApexAssetObject);
#if WITH_APEX_PARTICLES
		ClearEdit();
#endif
		SetNxApexAsset(NULL);
		ret = TRUE;
	}
	return ret;
}

/**
* Notifies the actor that a fresh asset has been imported and the actor can be re-created.
**/
void ApexAsset::NotifyAssetImport(void)
{
	MyApexCommands *mc = static_cast< MyApexCommands *>(GApexCommands);
	mc->NotifyAssetImport(this);
	for (TApexActorVector::iterator i=Actors.begin(); i!=Actors.end(); ++i)
	{
		FIApexActor *Act = (*i);
		if ( Act )
		{
			Act->NotifyAssetImport();
		}
	}
}

physx::apex::NxApexAssetAuthoring* ApexAuthoringFromAsset(physx::apex::NxApexAsset* Asset, physx::apex::NxApexSDK* ApexSDK)
{
	const ::NxParameterized::Interface* OldInterface = Asset->getAssetNxParameterized();

	// make the copy
	::NxParameterized::Interface* NewInterface = ApexSDK->getParameterizedTraits()->createNxParameterized(OldInterface->className());
	NewInterface->copy(*OldInterface);

	return ApexSDK->createAssetAuthoring(NewInterface, "AuthoringFromAsset");
}


}; // end of namespace

using namespace NI_APEX_SUPPORT;



FIApexCommands * CreateApexCommands(void)
{
    FIApexCommands *ret = NULL;

    MyApexCommands *ap = new MyApexCommands;
    ret = static_cast< FIApexCommands *>(ap);

    return ret;
}

void ReleaseApexCommands(FIApexCommands *ic)
{
	MyApexCommands *m = static_cast< MyApexCommands *>(ic);
	delete m;
}


void InitializeApex(void)
{
	if ( GApexManager == 0 )
	{
		GApexManager = CreateApexManager( GNovodexSDK, GNovodexCooking );
		CreateApexCommands();
		if ( GWorld && GWorld->RBPhysScene )
		{
			NxScene *scene = GWorld->RBPhysScene->GetNovodexPrimaryScene();
			if ( scene )
			{
				FIApexScene *apexScene = CreateApexScene(scene, GApexManager, true );
				GWorld->RBPhysScene->SetApexScene(apexScene);
			}
		}
	}
}

// NVCHANGE_BEGIN: JCAO - DE5584 unused material did not release from Apex Resource
void ReleaseApexResource(const char* materialName)
{
	if ( GApexManager )
	{
		NxApexSDK *apexSDK = GApexManager->GetApexSDK();
		NxResourceProvider* ApexResourceProvider = apexSDK->getNamedResourceProvider();
		check( ApexResourceProvider );

		ApexResourceProvider->releaseResource( APEX_MATERIALS_NAME_SPACE, materialName);
	}
}
// NVCHANGE_END: JCAO - DE5584 unused material did not release from Apex Resource
#endif

#endif

/*=============================================================================
	BoundShaderStateCache.cpp: Bound shader state cache implementation.
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#include "EnginePrivate.h"
#include "BoundShaderStateCache.h"

typedef TMap<FBoundShaderStateKey,FCachedBoundShaderStateLink*> FBoundShaderStateCache;

/** Lazily initialized bound shader state cache singleton. */
static FBoundShaderStateCache& GetBoundShaderStateCache()
{
	static FBoundShaderStateCache BoundShaderStateCache;
	return BoundShaderStateCache;
}

#if PLATFORM_SUPPORTS_D3D10_PLUS
FCachedBoundShaderStateLink::FCachedBoundShaderStateLink(
	FVertexDeclarationRHIParamRef VertexDeclaration,
	DWORD* StreamStrides,
	FVertexShaderRHIParamRef VertexShader,
	FPixelShaderRHIParamRef PixelShader,
	FHullShaderRHIParamRef HullShader,
	FDomainShaderRHIParamRef DomainShader,
	FGeometryShaderRHIParamRef GeometryShader,
	FBoundShaderStateRHIParamRef InBoundShaderState
	):
	BoundShaderState(InBoundShaderState),
	Key(VertexDeclaration,StreamStrides,VertexShader,PixelShader,HullShader,DomainShader,GeometryShader)
{
	// Add this bound shader state to the cache.
	GetBoundShaderStateCache().Set(Key,this);
}
#endif

FCachedBoundShaderStateLink::FCachedBoundShaderStateLink(
	FVertexDeclarationRHIParamRef VertexDeclaration,
	DWORD* StreamStrides,
	FVertexShaderRHIParamRef VertexShader,
	FPixelShaderRHIParamRef PixelShader,
	FBoundShaderStateRHIParamRef InBoundShaderState
	):
	BoundShaderState(InBoundShaderState),
	Key(VertexDeclaration,StreamStrides,VertexShader,PixelShader)
{
	// Add this bound shader state to the cache.
	GetBoundShaderStateCache().Set(Key,this);
}

FCachedBoundShaderStateLink::~FCachedBoundShaderStateLink()
{
	GetBoundShaderStateCache().Remove(Key);
}

FCachedBoundShaderStateLink* GetCachedBoundShaderState(
	FVertexDeclarationRHIParamRef VertexDeclaration,
	DWORD* StreamStrides,
	FVertexShaderRHIParamRef VertexShader,
	FPixelShaderRHIParamRef PixelShader
#if PLATFORM_SUPPORTS_D3D10_PLUS
	,FHullShaderRHIRef HullShader
	,FDomainShaderRHIRef DomainShader
	,FGeometryShaderRHIRef GeometryShader
#endif
	)
{
	// Find the existing bound shader state in the cache.
	return GetBoundShaderStateCache().FindRef(
		FBoundShaderStateKey(VertexDeclaration,StreamStrides,VertexShader,PixelShader
#if PLATFORM_SUPPORTS_D3D10_PLUS
			, HullShader, DomainShader, GeometryShader
#endif
			)
		);
}

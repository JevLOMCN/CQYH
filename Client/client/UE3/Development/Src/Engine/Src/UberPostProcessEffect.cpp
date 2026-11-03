/*=============================================================================
UberPostProcessEffect.cpp: combines DOF, bloom and material coloring.

Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#include "EnginePrivate.h"
#include "ScenePrivate.h"
#include "SceneFilterRendering.h"
#include "UnMotionBlurEffect.h"
#include "BokehDOF.h"
#include "PostProcessAA.h"

IMPLEMENT_CLASS(UUberPostProcessEffect);

//shanling@snda
//TGlobalResource<FToneMapingHistogramImageVertexBuffer> GToneMapingHistogramImageVertex;
//extern TGlobalResource<FToneMapingHistogramImageVertexDeclaration> GToneMapingHistogramImageVertexDeclaration;
//shanling@snda

/**
* Called when properties change.
*/
void UUberPostProcessEffect::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// clamp to 0..1
	SceneDesaturation = Clamp(SceneDesaturation, 0.0f, 1.0f);

	// UberPostProcessEffect should only ever exists in the SDPG_PostProcess scene
	SceneDPG=SDPG_PostProcess;

	Super::PostEditChangeProperty(PropertyChangedEvent);
}	

/**
* Called after this instance has been serialized.
*/
void UUberPostProcessEffect::PostLoad()
{
	Super::PostLoad();

	// UberPostProcessEffect should only ever exists in the SDPG_PostProcess scene
	SceneDPG=SDPG_PostProcess;

	// clamp desaturation to 0..1 (fixup for old data)
	SceneDesaturation = Clamp(SceneDesaturation, 0.f, 1.f);

	ULinkerLoad* LFLinkerLoad = GetLinker();
	if(LFLinkerLoad && (LFLinkerLoad->Ver() < VER_TONEMAPPER_ENUM))
	{
		TonemapperType = bEnableHDRTonemapper_DEPRECATED ? Tonemapper_Filmic : Tonemapper_Off;
		TonemapperScale = SceneHDRTonemapperScale_DEPRECATED;
	}
}




/*-----------------------------------------------------------------------------
FUberPostProcessBlendPixelShader
-----------------------------------------------------------------------------*/

extern TGlobalResource<FFilterVertexDeclaration> GFilterVertexDeclaration;

/** 
 * Encapsulates the blend pixel shader.
 */
class FUberPostProcessBlendPixelShaderBase : public FDOFAndBloomBlendPixelShader
{
protected:
	static UBOOL ShouldCache(EShaderPlatform Platform)
	{
		return TRUE;
	}

	static void ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment, UINT DepthOfFieldFullResMode, UINT TemplTonemapperType, UBOOL bUseImageGrain, UBOOL bSeparateTranslucency, UBOOL bTemporalAA)
	{
		OutEnvironment.Definitions.Set(TEXT("DOF_FULLRES_MODE"),*FString::Printf(TEXT("%u"), DepthOfFieldFullResMode));
		OutEnvironment.Definitions.Set(TEXT("USE_IMAGEGRAIN"),bUseImageGrain ? TEXT("1") : TEXT("0"));
		OutEnvironment.Definitions.Set(TEXT("USE_TONEMAPPERTYPE"),*FString::Printf(TEXT("%u"), TemplTonemapperType));
		OutEnvironment.Definitions.Set(TEXT("USE_SEPARATE_TRANSLUCENCY"), bSeparateTranslucency ? TEXT("1") : TEXT("0"));
		OutEnvironment.Definitions.Set(TEXT("USE_TEMPORAL_AA"),bTemporalAA ? TEXT("1") : TEXT("0"));
	}

	/** Default constructor. */
	FUberPostProcessBlendPixelShaderBase() {}

public:
	FShaderResourceParameter		LowResPostProcessBuffer;
	FShaderParameter				ImageAdjustments1Parameter;
	FShaderParameter				ImageAdjustments2Parameter;
	FShaderParameter				ImageAdjustments3Parameter;
	FShaderResourceParameter		ColorGradingLUTTextureParameter;
	FShaderParameter				HalfResMaskRectParameter;
	FShaderResourceParameter		BokehDOFLayerTextureParameter;
	FMotionBlurShaderParameters		MotionBlurShaderParameters;
	FShaderResourceParameter		NoiseTextureParameter;
	FShaderResourceParameter		LuminanceTextureParameter;	// zhaojun@snda
	FShaderParameter				GammaCoefficientParameter;
	FShaderParameter				LuminanceThresholdParameter;// zhaojun@snda
	

	/** Initialization constructor. */
	FUberPostProcessBlendPixelShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		:	FDOFAndBloomBlendPixelShader(Initializer)
		,	MotionBlurShaderParameters(Initializer.ParameterMap)
	{
		LowResPostProcessBuffer.Bind(Initializer.ParameterMap, TEXT("LowResPostProcessBuffer"), TRUE );
		ImageAdjustments1Parameter.Bind(Initializer.ParameterMap,TEXT("ImageAdjustments1"),TRUE);
		ImageAdjustments2Parameter.Bind(Initializer.ParameterMap,TEXT("ImageAdjustments2"),TRUE);
		ImageAdjustments3Parameter.Bind(Initializer.ParameterMap,TEXT("ImageAdjustments3"),TRUE);
		ColorGradingLUTTextureParameter.Bind(Initializer.ParameterMap,TEXT("ColorGradingLUT"), TRUE);
		HalfResMaskRectParameter.Bind(Initializer.ParameterMap,TEXT("HalfResMaskRect"),TRUE);
		BokehDOFLayerTextureParameter.Bind(Initializer.ParameterMap, TEXT("BokehDOFLayerTexture"), TRUE);
		NoiseTextureParameter.Bind(Initializer.ParameterMap,TEXT("NoiseTexture"),TRUE);
		LuminanceTextureParameter.Bind(Initializer.ParameterMap,TEXT("LuminanceTexture"),TRUE);	//zhaojun@snda
		GammaCoefficientParameter.Bind(Initializer.ParameterMap,TEXT("GammaCoefficient"),TRUE);
		LuminanceThresholdParameter.Bind(Initializer.ParameterMap,TEXT("LuminanceThreshold"),TRUE);	// zhaojun@snda

	}

	static const TCHAR* GetSourceFilename()
	{
		return TEXT("UberPostProcessBlendPixelShader");
	}

	static const TCHAR* GetFunctionName()
	{
		return TEXT("Main");
	}

	// FShader interface.
	virtual UBOOL Serialize(FArchive& Ar)
	{
		UBOOL bShaderHasOutdatedParameters = FDOFAndBloomBlendPixelShader::Serialize(Ar);
		Ar	<< LowResPostProcessBuffer << ImageAdjustments1Parameter << ImageAdjustments2Parameter
			<< ImageAdjustments3Parameter << ColorGradingLUTTextureParameter << HalfResMaskRectParameter
			<< BokehDOFLayerTextureParameter << MotionBlurShaderParameters
			<< NoiseTextureParameter << LuminanceTextureParameter << GammaCoefficientParameter << LuminanceThresholdParameter;

		ImageAdjustments1Parameter.SetShaderParamName(TEXT("ImageAdjustments1"));
		ImageAdjustments2Parameter.SetShaderParamName(TEXT("ImageAdjustments2"));
		ImageAdjustments3Parameter.SetShaderParamName(TEXT("ImageAdjustments3"));
		HalfResMaskRectParameter.SetShaderParamName(TEXT("HalfResMaskRect"));
		GammaCoefficientParameter.SetShaderParamName(TEXT("GammaCoefficient"));
		LuminanceThresholdParameter.SetShaderParamName(TEXT("LuminanceThreshold"));

#if WITH_MOBILE_RHI
		if (GUsingMobileRHI)
		{
			// Hard-coded to match the shader (it's serialized based on the PC shader, not the NGP shader).
			SceneTextureParameters.SceneColorTextureParameter.SetBaseIndex(0, TRUE);
			ColorGradingLUTTextureParameter.SetBaseIndex( 2 );
		}
#endif

		return bShaderHasOutdatedParameters;
	}
};

/** 
 * Permutations of the blend pixel shader.
 * @param DepthOfFieldFullResMode 0:off 1:ReferenceDOF 2:BokehDOF
 * @param TemplTonemapperType see "TonemapperType" console command, is never -1
 * @param bUseImageGrain whether to enable image grain
 * @param bSeparateTranslucency whether to enable separate translucency
 * @param bTemporalAA whether to enable temporal AA
 */
template<UINT DepthOfFieldFullResMode, UINT TemplTonemapperType, UBOOL bUseImageGrain, UBOOL bSeparateTranslucency, UBOOL bTemporalAA>
class TUberPostProcessBlendPixelShader : public FUberPostProcessBlendPixelShaderBase
{
	DECLARE_SHADER_TYPE( TUberPostProcessBlendPixelShader, Global );

	static void ModifyCompilationEnvironment( EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment )
	{
		FUberPostProcessBlendPixelShaderBase::ModifyCompilationEnvironment( Platform, OutEnvironment, DepthOfFieldFullResMode, TemplTonemapperType, bUseImageGrain, bSeparateTranslucency, bTemporalAA );
	}

protected:
	/** Default constructor. */
	TUberPostProcessBlendPixelShader() {}

public:
	/** Initialization constructor. */
	TUberPostProcessBlendPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		:	FUberPostProcessBlendPixelShaderBase(Initializer)
	{
	}
};

// #define avoids a lot of code duplication
#define VARIATION1(A)		VARIATION2(A,0)			VARIATION2(A,1)			VARIATION2(A,2)
#define VARIATION2(A,B)		VARIATION3(A,B,0)		VARIATION3(A,B,1)
#define VARIATION3(A,B,C)	VARIATION4(A,B,C,0)		VARIATION4(A,B,C,1)
#define VARIATION4(A,B,C,D)	VARIATION5(A,B,C,D,0)	VARIATION5(A,B,C,D,1)
#define VARIATION5(A, B, C, D, E) typedef TUberPostProcessBlendPixelShader<A, B, C, D, E> FUberPostProcessBlendPixelShader##A##B##C##D##E; \
	IMPLEMENT_SHADER_TYPE2(template<>, FUberPostProcessBlendPixelShader##A##B##C##D##E, SF_Pixel, VER_BLOOM_AFTER_MOTIONBLUR, 0);

	VARIATION1(0) VARIATION1(1) VARIATION1(2)

#undef VARIATION1
#undef VARIATION2
#undef VARIATION3
#undef VARIATION4
#undef VARIATION5



/** Encapsulates the motion blur pixel shader for the uber postprocess. */
class FUberHalfResPixelShaderBase : public FDOFAndBloomBlendPixelShader
{
protected:
	static UBOOL ShouldCache(EShaderPlatform Platform, UBOOL bTMotionBlur, UBOOL bUseDoFBlurBuffer, UBOOL bUseSoftEdgeMotionBlur)
	{
		// Blur buffer currently only supported on 360.
		if(bUseDoFBlurBuffer)
		{
			return Platform == SP_XBOXD3D;
		}

		if(!bTMotionBlur && bUseSoftEdgeMotionBlur)
		{
			// bUseSoftEdgeMotionBlur without bMotionBlur is useless
			return FALSE;
		}

		return TRUE;
	}

	static void ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment, UBOOL bTMotionBlur, UBOOL bUseDoFBlurBuffer, UINT DepthOfFieldHalfResMode, UBOOL bUseSoftEdgeMotionBlur)
	{
		OutEnvironment.Definitions.Set(TEXT("MOTION_BLUR"),bTMotionBlur ? TEXT("1") : TEXT("0"));
		OutEnvironment.Definitions.Set(TEXT("USE_DOF_BLUR_BUFFER"),bUseDoFBlurBuffer ? TEXT("1") : TEXT("0"));
		OutEnvironment.Definitions.Set(TEXT("DOF_HALFRES_MODE"), *FString::Printf(TEXT("%u"), DepthOfFieldHalfResMode));
		OutEnvironment.Definitions.Set(TEXT("USE_SOFTEDGE_MOTIONBLUR"),bUseSoftEdgeMotionBlur ? TEXT("1") : TEXT("0"));
	}

	/** Default constructor. */
	FUberHalfResPixelShaderBase() {}

public:
	FMotionBlurShaderParameters		MotionBlurParameters;
	FShaderResourceParameter		LowResPostProcessBufferPointParameter;
	FShaderResourceParameter		BokehDOFLayerTextureParameter;

	/** Initialization constructor. */
	FUberHalfResPixelShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		:	FDOFAndBloomBlendPixelShader(Initializer)
		,	MotionBlurParameters(Initializer.ParameterMap)
	{
		LowResPostProcessBufferPointParameter.Bind(Initializer.ParameterMap, TEXT("LowResPostProcessBufferPoint"), TRUE );
		BokehDOFLayerTextureParameter.Bind(Initializer.ParameterMap, TEXT("BokehDOFLayerTexture"), TRUE);
	}

	static const TCHAR* GetSourceFilename()
	{
		return TEXT("UberPostProcessBlendPixelShader");
	}

	static const TCHAR* GetFunctionName()
	{
		return TEXT("UberHalfResMain");
	}

	// FShader interface.
	virtual UBOOL Serialize(FArchive& Ar)
	{
		UBOOL bShaderHasOutdatedParameters = FDOFAndBloomBlendPixelShader::Serialize(Ar);
		Ar << MotionBlurParameters << LowResPostProcessBufferPointParameter << BokehDOFLayerTextureParameter;
		return bShaderHasOutdatedParameters;
	}
};

/** Permutations of the motion blur pixel shader for the uber postprocess. */
template<UBOOL bTMotionBlur, UBOOL bUseDoFBlurBuffer, UINT DepthOfFieldHalfResMode, UBOOL bUseSoftEdgeMotionBlur>
class TUberHalfResPixelShader : public FUberHalfResPixelShaderBase
{
	DECLARE_SHADER_TYPE( TUberHalfResPixelShader, Global );

	static UBOOL ShouldCache( EShaderPlatform Platform )
	{
		return FUberHalfResPixelShaderBase::ShouldCache( Platform, bTMotionBlur, bUseDoFBlurBuffer, bUseSoftEdgeMotionBlur );
	}

	static void ModifyCompilationEnvironment( EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment ) 
	{
		FUberHalfResPixelShaderBase::ModifyCompilationEnvironment( Platform, OutEnvironment, bTMotionBlur, bUseDoFBlurBuffer, DepthOfFieldHalfResMode, bUseSoftEdgeMotionBlur );
	}

protected:
	/** Default constructor. */
	TUberHalfResPixelShader() {}

public:
	/** Initialization constructor. */
	TUberHalfResPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		:	FUberHalfResPixelShaderBase(Initializer)
	{
	}
};

//////////////////////////////////////////////////////////////////////////
class HDRVertexShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(HDRVertexShader,Global);

	static UBOOL ShouldCache(EShaderPlatform platform )
	{
		return TRUE;
	}
	static void ModifyCompilationEnvironment(EShaderPlatform platform, FShaderCompilerEnvironment& OutEnvironment)
	{
	}

	HDRVertexShader()
	{

	}

public:
	HDRVertexShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer )
		: FGlobalShader(Initializer)
	{
	}

	virtual UBOOL Serialize(FArchive& Ar)
	{
		return FGlobalShader::Serialize(Ar);
	}
};

IMPLEMENT_SHADER_TYPE(,HDRVertexShader,TEXT("LuminanceAndToneMapping"),TEXT("HDRVSMain"),SF_Vertex,0,0);


//////////////////////////////////////////////////////////////////////////

// pixel shader
class FRT2DownSamplePixelShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FRT2DownSamplePixelShader,Global);

	static UBOOL ShouldCache(EShaderPlatform platform )
	{
		return TRUE;
	}
	static void ModifyCompilationEnvironment(EShaderPlatform platform, FShaderCompilerEnvironment& OutEnvironment)
	{

	}

	FRT2DownSamplePixelShader()
	{
	}

	FShaderResourceParameter	SrcTexture;
	//shanling@snda
	FShaderParameter			InvTex;
	//shanling@snda

public:

	FRT2DownSamplePixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer )
		: FGlobalShader(Initializer)
	{
		SrcTexture.Bind( Initializer.ParameterMap, TEXT("SrcTexture"), FALSE );
		InvTex.Bind(Initializer.ParameterMap, TEXT("InvTexWH"), TRUE);
	}

	virtual UBOOL Serialize(FArchive& Ar)
	{
		UBOOL bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		Ar << SrcTexture;
		//shanling@snda
		Ar << InvTex;
		//shanling@snda
		return bShaderHasOutdatedParameters;
	}

	FShaderResourceParameter& GetSrcTextureParam()
	{
		return SrcTexture;
	}

	const FShaderResourceParameter& GetSrcTextureParam() const
	{
		return SrcTexture;
	}
	//shanling@snda
	FShaderParameter& GetInvTexParam()
	{
		return InvTex;
	}

	const FShaderParameter& GetInvTexParam() const
	{
		return InvTex;
	}
	//shanling@snda
};

IMPLEMENT_SHADER_TYPE(,FRT2DownSamplePixelShader,TEXT("LuminanceAndToneMapping"),TEXT("RT2DownSampleMain"),SF_Pixel,0,0);

////////////////////////////begin zhaojun@snda//////////////////////////////

// pixel shader
class FDownSamplePixelShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FDownSamplePixelShader,Global);

	static UBOOL ShouldCache(EShaderPlatform platform)
	{
		return TRUE;
	}
	static void ModifyCompilationEnvironment(EShaderPlatform platform, FShaderCompilerEnvironment& OutEnvironment)
	{

	}

	FDownSamplePixelShader() 
	{

	}

	void SetParameters(FVector4 UVOffsetsIn[9])
	{
		SetShaderValues( GetPixelShader(), UVOffsets, UVOffsetsIn, 9 );
	}

	FShaderResourceParameter	SrcTexture;
	FShaderParameter			UVOffsets;

public:
	FDownSamplePixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		SrcTexture.Bind( Initializer.ParameterMap, TEXT("SrcTexture"), FALSE );
		UVOffsets.Bind( Initializer.ParameterMap, TEXT("UVOffsets"), TRUE );
	}

	virtual UBOOL Serialize(FArchive& Ar)
	{
		//return FALSE;
		UBOOL bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		Ar << SrcTexture;
		Ar << UVOffsets;

		UVOffsets.SetShaderParamName(TEXT("UVOffsets"));

		return bShaderHasOutdatedParameters;
	}

	FShaderResourceParameter& GetSrcTextureParam()
	{
		return SrcTexture;
	}

	const FShaderResourceParameter& GetSrcTextureParam() const
	{
		return SrcTexture;
	}

};

IMPLEMENT_SHADER_TYPE(,FDownSamplePixelShader,TEXT("LuminanceAndToneMapping"),TEXT("DownSampleMain"),SF_Pixel,0,0);

////////////////////////////begin shanling@snda//////////////////////////////
// pixel shader
template< UINT bUseLuminanceClamp >
class FDofDownSampleShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FDofDownSampleShader,Global);

	static UBOOL ShouldCache(EShaderPlatform platform)
	{
		return TRUE;
	}
	static void ModifyCompilationEnvironment(EShaderPlatform platform, FShaderCompilerEnvironment& OutEnvironment )
	{
		OutEnvironment.Definitions.Set(TEXT("USE_LUMINANCE_CLAMP"), *FString::Printf(TEXT("%u"), bUseLuminanceClamp));
	}

	FDofDownSampleShader() 
	{

	}

	FDOFShaderParameters		DOFParameters;
	FShaderResourceParameter FilterTextureParameter;
	FSceneTextureShaderParameters SceneTextureParameters;
	FShaderResourceParameter VelocityTextureParameter;
	FShaderParameter         LuminanceClampParameters;
public:
	FDofDownSampleShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
		,DOFParameters(Initializer.ParameterMap)
	{
		FilterTextureParameter.Bind(Initializer.ParameterMap,TEXT("FilterTexture"),TRUE);
		SceneTextureParameters.Bind(Initializer.ParameterMap);
		VelocityTextureParameter.Bind(Initializer.ParameterMap, TEXT("VelocityTexture"), TRUE );
		LuminanceClampParameters.Bind( Initializer.ParameterMap, TEXT("LuminanceClampParameters"), TRUE );
	}

	virtual UBOOL Serialize(FArchive& Ar)
	{
		UBOOL bShaderHasOutdatedParameters = FShader::Serialize(Ar);
		Ar << FilterTextureParameter << VelocityTextureParameter << DOFParameters<<SceneTextureParameters<<LuminanceClampParameters;

#if WITH_MOBILE_RHI
		if (GUsingMobileRHI)
		{
			SceneTextureParameters.SceneColorTextureParameter.Unbind();
		}
#endif
		return bShaderHasOutdatedParameters;
	}

	/** @param View needed for depth in alpha */
	void SetParameters(const FSceneView* View, FSamplerStateRHIParamRef SamplerStateRHI,FTextureRHIParamRef TextureRHI, float clampParameters )
	{																													
		SetTextureParameterDirectly(GetPixelShader(),FilterTextureParameter,SamplerStateRHI,TextureRHI);

		SceneTextureParameters.Set(View, this);

		//shan@snda
		DOFParameters.SetPS( this, View->DepthOfFieldParams);
		SetShaderValue( GetPixelShader(), LuminanceClampParameters, clampParameters );
		//shan@snda

#if WITH_MOBILE_RHI
		if (GUsingMobileRHI)
		{
			FilterTextureParameter.SetBaseIndex( 0, TRUE );
		}
#endif
		//SceneTextureParameters.Set(View, this);

		// can be invalid, e.g. if MotionBlur is disabled in BaseEngine.ini
		if(IsValidRef(GSceneRenderTargets.GetVelocityTexture())) 
		{
			// on Xbox360 the velocity texture is half res, using bilinear filtering and 4 samples expands the mask a bit to get rif of artifacts
			// on other platforms the bilinear filtering doesn't matter but the 4 samples still improve quality
			SetTextureParameterDirectly(this->GetPixelShader(), VelocityTextureParameter, TStaticSamplerState<SF_Bilinear>::GetRHI(), GSceneRenderTargets.GetVelocityTexture());
		}
	}

};

IMPLEMENT_SHADER_TYPE(,FDofDownSampleShader<0>,TEXT("FilterPixelShader"),TEXT("DownsampleSceneforDof"),SF_Pixel,0,0);
IMPLEMENT_SHADER_TYPE(,FDofDownSampleShader<1>,TEXT("FilterPixelShader"),TEXT("DownsampleSceneforDof"),SF_Pixel,0,0);
// pixel shader
class FCalculateLuminanceShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FCalculateLuminanceShader,Global);

	static UBOOL ShouldCache(EShaderPlatform platform)
	{
		return TRUE;
	}
	static void ModifyCompilationEnvironment(EShaderPlatform platform, FShaderCompilerEnvironment& OutEnvironment)
	{

	}

	FCalculateLuminanceShader() 
	{

	}

	FShaderResourceParameter	SrcTexture;
public:
	FCalculateLuminanceShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		SrcTexture.Bind( Initializer.ParameterMap, TEXT("SrcTexture"), FALSE );
	}

	virtual UBOOL Serialize(FArchive& Ar)
	{
		//return FALSE;
		UBOOL bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		Ar << SrcTexture;

		return bShaderHasOutdatedParameters;
	}

	FShaderResourceParameter& GetSrcTextureParam()
	{
		return SrcTexture;
	}

	const FShaderResourceParameter& GetSrcTextureParam() const
	{
		return SrcTexture;
	}

};

IMPLEMENT_SHADER_TYPE(,FCalculateLuminanceShader,TEXT("LuminanceAndToneMapping"),TEXT("CalculateLuminance"),SF_Pixel,0,0);

// Vertex shader
class FToneMapingHistogramImageVertexShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FToneMapingHistogramImageVertexShader,Global);

	static UBOOL ShouldCache(EShaderPlatform platform)
	{
		return TRUE;
	}
	static void ModifyCompilationEnvironment(EShaderPlatform platform, FShaderCompilerEnvironment& OutEnvironment)
	{

	}

	FToneMapingHistogramImageVertexShader() 
	{

	}

	FShaderResourceParameter	SrcTexture;
	FShaderResourceParameter	LuminanceTexture;
	FShaderParameter			BufferWidthHeight;
	FShaderParameter			InvLuminanceBufferWidthHeight;
public:
	FToneMapingHistogramImageVertexShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		SrcTexture.Bind( Initializer.ParameterMap, TEXT("SrcTexture"), FALSE );
		LuminanceTexture.Bind( Initializer.ParameterMap, TEXT("LuminanceTexture"), FALSE );
		BufferWidthHeight.Bind( Initializer.ParameterMap, TEXT("BufferWidthHeight"), FALSE );
		InvLuminanceBufferWidthHeight.Bind( Initializer.ParameterMap, TEXT("InvLuminanceBufferWidthHeight"), FALSE );
	}

	virtual UBOOL Serialize(FArchive& Ar)
	{
		//return FALSE;
		UBOOL bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		Ar << SrcTexture<<LuminanceTexture<<BufferWidthHeight<<InvLuminanceBufferWidthHeight;

		return bShaderHasOutdatedParameters;
	}

	FShaderParameter& GeBufferWidthHeightParam()
	{
		return BufferWidthHeight;
	}

	const FShaderParameter& GetBufferWidthHeightParam() const
	{
		return BufferWidthHeight;
	}

	FShaderResourceParameter& GetLuminanceTextureParam()
	{
		return LuminanceTexture;
	}

	const FShaderResourceParameter& GetLuminanceTextureParam() const
	{
		return LuminanceTexture;
	}

	FShaderResourceParameter& GetSrcTextureParam()
	{
		return SrcTexture;
	}

	const FShaderResourceParameter& GetSrcTextureParam() const
	{
		return SrcTexture;
	}

	FShaderParameter& GetInvLuminanceBufferWidthHeightParam()
	{
		return InvLuminanceBufferWidthHeight;
	}

	const FShaderParameter& GetInvLuminanceBufferWidthHeightParam() const
	{
		return InvLuminanceBufferWidthHeight;
	}

};

IMPLEMENT_SHADER_TYPE(,FToneMapingHistogramImageVertexShader,TEXT("LuminanceAndToneMapping"),TEXT("ToneMapingHistogramImageVS"),SF_Vertex,0,0);


// pixel shader
class FToneMapingHistogramImagePixelShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FToneMapingHistogramImagePixelShader,Global);

	static UBOOL ShouldCache(EShaderPlatform platform)
	{
		return TRUE;
	}
	static void ModifyCompilationEnvironment(EShaderPlatform platform, FShaderCompilerEnvironment& OutEnvironment)
	{

	}

	FToneMapingHistogramImagePixelShader() 
	{

	}

public:
	FToneMapingHistogramImagePixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		
	}

	virtual UBOOL Serialize(FArchive& Ar)
	{
		//return FALSE;
		UBOOL bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		
		return bShaderHasOutdatedParameters;
	}

};

IMPLEMENT_SHADER_TYPE(,FToneMapingHistogramImagePixelShader,TEXT("LuminanceAndToneMapping"),TEXT("ToneMapingHistogramImagePS"),SF_Pixel,0,0);

// pixel shader
class FCalculateAdaptedLumPixelShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FCalculateAdaptedLumPixelShader,Global);

	static UBOOL ShouldCache(EShaderPlatform platform)
	{
		return TRUE;
	}
	static void ModifyCompilationEnvironment(EShaderPlatform platform, FShaderCompilerEnvironment& OutEnvironment)
	{

	}

	FCalculateAdaptedLumPixelShader() 
	{

	}

	FShaderParameter			HDRLuminanceParameter;      // shan@snda
public:
	FCalculateAdaptedLumPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		HDRLuminanceParameter.Bind(Initializer.ParameterMap,TEXT("HDRLuminanceParameter"),TRUE);
	}

	virtual UBOOL Serialize(FArchive& Ar)
	{
		//return FALSE;
		UBOOL bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		Ar << HDRLuminanceParameter;
		return bShaderHasOutdatedParameters;
	}

	FShaderParameter& GetLuminanceParameter() { return HDRLuminanceParameter; }
	const FShaderParameter& GetLuminanceParameter() const { return HDRLuminanceParameter; }

};

IMPLEMENT_SHADER_TYPE(,FCalculateAdaptedLumPixelShader,TEXT("LuminanceAndToneMapping"),TEXT("CalculateAdaptedLumPS"),SF_Pixel,0,0);

// pixel shader
class FDownSampleCalculateLumPixelShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FDownSampleCalculateLumPixelShader,Global);

	static UBOOL ShouldCache(EShaderPlatform platform)
	{
		return TRUE;
	}
	static void ModifyCompilationEnvironment(EShaderPlatform platform, FShaderCompilerEnvironment& OutEnvironment)
	{

	}

	FDownSampleCalculateLumPixelShader() 
	{

	}

	FShaderResourceParameter	SrcTexture;
	FShaderParameter			UVOffsets;
public:
	FDownSampleCalculateLumPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		SrcTexture.Bind( Initializer.ParameterMap, TEXT("SrcTexture"), FALSE );
		UVOffsets.Bind( Initializer.ParameterMap, TEXT("UVOffsets"), FALSE );

	}

	virtual UBOOL Serialize(FArchive& Ar)
	{
		//return FALSE;
		UBOOL bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		Ar<<SrcTexture<<UVOffsets;
		return bShaderHasOutdatedParameters;
	}

	void SetParameters(FVector4 UVOffsetsIn[9])
	{
		SetShaderValues( GetPixelShader(), UVOffsets, UVOffsetsIn, 9 );
	}

	FShaderResourceParameter& GetSrcTextureParam()
	{
		return SrcTexture;
	}

	const FShaderResourceParameter& GetSrcTextureParam() const
	{
		return SrcTexture;
	}

};

IMPLEMENT_SHADER_TYPE(,FDownSampleCalculateLumPixelShader,TEXT("LuminanceAndToneMapping"),TEXT("DownSampleCalculateLuminancePS"),SF_Pixel,0,0);

////////////////////////////end shanling@snda//////////////////////////////

//////////////////////////////////////////////////////////////////////////
//// now merge into uberpostprocessblendpixelshader.usf
//class FToneMappingPixelShader : public FGlobalShader
//{
//	DECLARE_SHADER_TYPE(FToneMappingPixelShader,Global);
//
//	static UBOOL ShouldCache(EShaderPlatform platform )
//	{
//		return TRUE;
//	}
//	static void ModifyCompilationEnvironment(EShaderPlatform platform, FShaderCompilerEnvironment& OutEnvironment)
//	{
//	}
//
//	FToneMappingPixelShader() 
//	{
//
//	}
//
//	FShaderResourceParameter	SrcTexture;
//	FShaderResourceParameter	LuminanceTexture;
//
//	//FShaderParameter			UVOffset;
//	FShaderParameter			fExposure;		// A user configurable bias to under/over expose the image
//	FShaderParameter			fGaussianScalar;//
//
//public:
//	FToneMappingPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer )
//		: FGlobalShader(Initializer)
//	{
//		SrcTexture.Bind( Initializer.ParameterMap, TEXT("SrcTexture"), FALSE );
//		LuminanceTexture.Bind( Initializer.ParameterMap, TEXT("LuminanceTexture"), FALSE );
//		
//		fExposure.Bind( Initializer.ParameterMap, TEXT("fExposure"), FALSE );
//		fGaussianScalar.Bind( Initializer.ParameterMap, TEXT("fGaussianScalar"), FALSE );
//	}
//
//	virtual UBOOL Serialize(FArchive& Ar)
//	{
//		UBOOL bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
//
//		Ar << SrcTexture;
//		Ar << LuminanceTexture;
//		Ar << fExposure;
//		Ar << fGaussianScalar;
//
//		return bShaderHasOutdatedParameters;
//	}
//
//	FShaderResourceParameter& GetSrcTextureParam()
//	{
//		return SrcTexture;
//	}
//
//	const FShaderResourceParameter& GetSrcTextureParam() const
//	{
//		return SrcTexture;
//	}
//
//	FShaderResourceParameter& GetLuminanceTextureParam()
//	{
//		return LuminanceTexture;
//	}
//
//	const FShaderResourceParameter& GetLuminanceTextureParam() const
//	{
//		return LuminanceTexture;
//	}
//
//	void SetParameters(float Exposure, float GaussianScalar)
//	{
//		SetShaderValue( GetPixelShader(), fExposure, Exposure );
//		SetShaderValue( GetPixelShader(), fGaussianScalar, GaussianScalar );
//	}
//};
//
//IMPLEMENT_SHADER_TYPE(,FToneMappingPixelShader,TEXT("LuminanceAndToneMapping"),TEXT("ToneMappingMain"),SF_Pixel,0,0);



///////////////////////end zhaojun@snda////////////////////////////////

// #define avoids a lot of code duplication
#define VARIATION(bA, bB, C, bD) typedef TUberHalfResPixelShader<bA, bB, C, bD> FUberHalfResPixelShader##bA##bB##C##bD; \
	IMPLEMENT_SHADER_TYPE2(template<>, FUberHalfResPixelShader##bA##bB##C##bD, SF_Pixel, VER_DWORD_SKELETAL_MESH_INDICES_FIXUP, 0);
VARIATION(0,0,0,0) VARIATION(0,0,0,1) VARIATION(0,0,1,0) VARIATION(0,0,1,1)
VARIATION(0,0,2,0) VARIATION(0,0,2,1)
VARIATION(0,1,0,0) VARIATION(0,1,0,1) VARIATION(0,1,1,0) VARIATION(0,1,1,1)
VARIATION(0,1,2,0) VARIATION(0,1,2,1)
VARIATION(1,0,0,0) VARIATION(1,0,0,1) VARIATION(1,0,1,0) VARIATION(1,0,1,1)
VARIATION(1,0,2,0) VARIATION(1,0,2,1)
VARIATION(1,1,0,0) VARIATION(1,1,0,1) VARIATION(1,1,1,0) VARIATION(1,1,1,1)
VARIATION(1,1,2,0) VARIATION(1,1,2,1)
//
#undef VARIATION

/*-----------------------------------------------------------------------------
FUberPostProcessVertexShader
-----------------------------------------------------------------------------*/

/** Encapsulates the UberPostProcess vertex shader. */
class FUberPostProcessVertexShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FUberPostProcessVertexShader,Global);

	static UBOOL ShouldCache(EShaderPlatform Platform)
	{
		return TRUE;
	}
	static void ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment)
	{
	}

	/** Default constructor. */
	FUberPostProcessVertexShader() {}
	
	FUberPostProcessVertexShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		:	FGlobalShader(Initializer)
	{
		SceneCoordinate1ScaleBiasParameter.Bind(Initializer.ParameterMap,TEXT("SceneCoordinate1ScaleBias"),TRUE);
		SceneCoordinate2ScaleBiasParameter.Bind(Initializer.ParameterMap,TEXT("SceneCoordinate2ScaleBias"),TRUE);
		SceneCoordinate3ScaleBiasParameter.Bind(Initializer.ParameterMap,TEXT("SceneCoordinate3ScaleBias"),TRUE);
	}

	UBOOL Serialize(FArchive& Ar)
	{
		UBOOL bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		Ar << SceneCoordinate1ScaleBiasParameter << SceneCoordinate2ScaleBiasParameter << SceneCoordinate3ScaleBiasParameter;

		SceneCoordinate1ScaleBiasParameter.SetShaderParamName(TEXT("SceneCoordinate1ScaleBias"));
		SceneCoordinate2ScaleBiasParameter.SetShaderParamName(TEXT("SceneCoordinate2ScaleBias"));
		SceneCoordinate3ScaleBiasParameter.SetShaderParamName(TEXT("SceneCoordinate3ScaleBias"));
		return bShaderHasOutdatedParameters;
	}

public:
	FShaderParameter SceneCoordinate1ScaleBiasParameter;
	FShaderParameter SceneCoordinate2ScaleBiasParameter;
	FShaderParameter SceneCoordinate3ScaleBiasParameter;
};

IMPLEMENT_SHADER_TYPE(,FUberPostProcessVertexShader,TEXT("UberPostProcessVertexShader"),TEXT("Main"),SF_Vertex,VER_SOFTEDGEMOTIONBLUR,0);


// y = TonemapperAFunc(x)
// computes y for a given x
static FLOAT TonemapperAFunc(FLOAT x, FLOAT A, FLOAT B)
{
	return x / (x + A) * B;
}
// y = TonemapperAFunc(x)
// computes x for a given y
//static FLOAT InverseTonemapperAFunc(FLOAT y, FLOAT A, FLOAT B)
//{
//	return y * A / (B - y);
//}
// dy = TonemapperFunc(x) derived
// computes dy for a given x
static FLOAT TonemapperADerivedFunc(FLOAT x, FLOAT A, FLOAT B)
{
	return A / Square(x + A) * B;
}
// y = TonemapperFunc(x) derived
// computes x for a given dy
static FLOAT InverseTonemapperADerivedFunc(FLOAT dy, FLOAT A, FLOAT B)
{
	return appSqrt(A * B / dy) - A;
}
 
/*-----------------------------------------------------------------------------
FUberPostProcessSceneProxy
-----------------------------------------------------------------------------*/

extern TGlobalResource<FFilterVertexDeclaration> GFilterVertexDeclaration;

class FUberPostProcessSceneProxy : public FDOFAndBloomPostProcessSceneProxy
{
public:
	/** 
	* Initialization constructor. 
	* @param InEffect - Uber post process effect to mirror in this proxy, must not be 0
	* @param WorldSettings can be 0
	* @param InTonemapperType see "TonemapperType" console command, is never -1
	*/
	FUberPostProcessSceneProxy(const UUberPostProcessEffect* InEffect,const FPostProcessSettings* WorldSettings, UINT InColorGradingCVar, UINT InTonemapperType, UBOOL bInMotionBlur, UBOOL bInImageGrain)
		:	FDOFAndBloomPostProcessSceneProxy(InEffect, WorldSettings)
		,	MotionBlurSoftEdgeKernelSize(InEffect->MotionBlurSoftEdgeKernelSize)
		,	TonemapperToeFactor(InEffect->TonemapperToeFactor)
		,	TonemapperRange(InEffect->TonemapperRange)
		,	ColorGradingCVar(InColorGradingCVar)
		,	TonemapperType(InTonemapperType)
		,	bMotionBlur(bInMotionBlur)
		,	bScaleEffectsWithViewSize(InEffect->bScaleEffectsWithViewSize)
		,	bEnableSceneEffect( TRUE )
		,	PostProcessAA(InEffect, WorldSettings)
	{
		checkSlow(IsInGameThread());
		check(InEffect);

		SET_POSTPROCESS_PROPERTY1(Scene, TonemapperScale);
		SET_POSTPROCESS_PROPERTY2(Scene, ImageGrainScale);

		ColorTransform.Shadows = GET_POSTPROCESS_PROPERTY2(Scene, Shadows);
		ColorTransform.HighLights = GET_POSTPROCESS_PROPERTY2(Scene, HighLights);
		ColorTransform.MidTones = GET_POSTPROCESS_PROPERTY2(Scene, MidTones);
		ColorTransform.Desaturation = GET_POSTPROCESS_PROPERTY2(Scene, Desaturation);
		ColorTransform.Colorize = GET_POSTPROCESS_PROPERTY2(Scene, Colorize);

		MotionBlurParams.RotationThreshold =	GET_POSTPROCESS_PROPERTY1(MotionBlur, CameraRotationThreshold);
		MotionBlurParams.TranslationThreshold = GET_POSTPROCESS_PROPERTY1(MotionBlur, CameraTranslationThreshold);
		MotionBlurParams.MaxVelocity =			GET_POSTPROCESS_PROPERTY1(MotionBlur, MaxVelocity);
		MotionBlurParams.MotionBlurAmount =		GET_POSTPROCESS_PROPERTY2(MotionBlur, Amount);

		MotionBlurParams.bFullMotionBlur = InEffect->FullMotionBlur;

		if(WorldSettings)
		{
			if(WorldSettings->bOverride_MotionBlur_FullMotionBlur)
			{
				MotionBlurParams.bFullMotionBlur = WorldSettings->MotionBlur_FullMotionBlur;
			}

			if( WorldSettings->bOverride_EnableSceneEffect )
			{
				bEnableSceneEffect = WorldSettings->bEnableSceneEffect;
			}
		}

		// Compute weights from user settings
		{
			FLOAT Small = Max(InEffect->BloomWeightSmall, 0.0f);
			FLOAT Medium = Max(InEffect->BloomWeightMedium, 0.01f);
			FLOAT Large = Max(InEffect->BloomWeightLarge, 0.0f);

			// normalize weights
			{
				FLOAT Inv = 1.0f / (Small + Medium + Large);

				BloomWeightSmall = Small * Inv;
				BloomWeightLarge = Large * Inv;
			}

			checkSlow(BloomWeightSmall >= 0.0f && BloomWeightLarge <= 1.0f);
			checkSlow(BloomWeightLarge >= 0.0f && BloomWeightLarge <= 1.0f);
		}

		// console command override
		if(GBloomWeightSmall >= 0)
		{
			BloomWeightSmall = GBloomWeightSmall;
		}

		// console command override
		if(GBloomWeightLarge >= 0)
		{
			BloomWeightLarge = GBloomWeightLarge;
		}

#if WITH_MOBILE_RHI
		if (GUsingMobileRHI)
		{
			// currently, mobile only works with bloom type 3
			// @todo, investigate why only 3 works
			BloomType = 3;
		}
		else
#endif
		// find best BloomType 
		{
			// 1 Gaussian bell curve unless we change it in the following code
			BloomType = 3;

			// we pick the method depending on the specified weights
			if(BloomWeightLarge > 0.01f)
			{
				// 3 Gaussian bell curves combined
				BloomType = 2;
			}
			else
			{
				if(BloomWeightSmall > 0.01f)
				{
					// 2 Gaussian bell curves combined, inner radius reduced
					BloomType = 1;
				}
			}

			// console variable override
			{
				static IConsoleVariable* CVar = GConsoleManager->FindConsoleVariable(TEXT("BloomType")); 
				INT Value = CVar ? CVar->GetInt() : -1;

				if(Value >= 0)
				{
					BloomType = Value;
				}
			}
		}

		checkSlow(BloomType >= 0);

		BloomQuality = 1;

		if(!bInImageGrain)
		{
			SceneImageGrainScale = 0.0f;
		}

#if !FINAL_RELEASE
		{
			// console variable override
			static IConsoleVariable* CVar = GConsoleManager->FindConsoleVariable(TEXT("BloomSize")); 
			FLOAT Value = CVar->GetFloat();

			if(Value >= 0.0f)
			{
				BlurBloomKernelSize = Value;
			}
		}

		{
			// console variable override
			static IConsoleVariable* CVar = GConsoleManager->FindConsoleVariable(TEXT("BloomThreshold")); 
			FLOAT Value = CVar->GetFloat();

			if(Value >= 0.0f)
			{
				BloomThreshold = Value;
			}
		}

		{
			// console variable override
			static IConsoleVariable* CVar = GConsoleManager->FindConsoleVariable(TEXT("BloomScale")); 
			FLOAT Value = CVar->GetFloat();

			if(Value >= 0.0f)
			{
				BloomScale = Value;
			}
		}

		{
			// console variable override
			static IConsoleVariable* CVar = GConsoleManager->FindConsoleVariable(TEXT("MotionBlurSoftEdge")); 
			FLOAT Value = CVar->GetFloat();

			if(Value >= 0.0f)
			{
				MotionBlurSoftEdgeKernelSize = Value;
			}
		}

		{
			// console variable override
			static IConsoleVariable* CVar = GConsoleManager->FindConsoleVariable(TEXT("MotionBlurMaxVelocity")); 
			FLOAT Value = CVar->GetFloat();

			if(Value >= 0.0f)
			{
				MotionBlurParams.MaxVelocity = Value;
			}
		}

		{
			// console variable override
			static IConsoleVariable* CVar = GConsoleManager->FindConsoleVariable(TEXT("MotionBlurAmount")); 
			FLOAT Value = CVar->GetFloat();

			if(Value >= 0.0f)
			{
				MotionBlurParams.MotionBlurAmount = Value;
			}
		}

		{
			// console variable override
			static IConsoleVariable* CVar = GConsoleManager->FindConsoleVariable(TEXT("BloomQuality")); 
			INT Value = CVar->GetInt();

			if(Value >= 0)
			{
				BloomQuality = Value;
			}
		}

		{
			// console variable override
			static IConsoleVariable* CVar = GConsoleManager->FindConsoleVariable(TEXT("ImageGrain")); 
			FLOAT Value = CVar->GetFloat();

			if(Value >= 0.0f)
			{
				SceneImageGrainScale = Value;
			}
		}
#endif // !FINAL_RELEASE

		// ensure that desat values out of range get clamped (this can happen in the editor currently)
		ColorTransform.Desaturation = Clamp(ColorTransform.Desaturation, 0.f, 1.f);

		// blend multiple LUT to produce final LUT (3D color lookup texture) for color grading
		if(WorldSettings && ColorGradingCVar != 0)
		{
			if(!WorldSettings->ColorGradingLUT.IsLUTEmpty())
			{
				// game filled out the table (soft transitions)
				WorldSettings->ColorGradingLUT.CopyToRenderThread(ColorGradingBlenderRTCopy);
			}
			else
			{
				// game wasn't filling the table so we set the editor LUT (hard switch)
				FLUTBlender LocalColorGradingLUT;

				LocalColorGradingLUT.Reset();

				if(ColorGrading_LookupTable)
				{
					LocalColorGradingLUT.LerpTo(ColorGrading_LookupTable, 1.0f);
				}

				LocalColorGradingLUT.CopyToRenderThread(ColorGradingBlenderRTCopy);
			}
		}

		extern INT GMotionBlurFullMotionBlur;
		MotionBlurParams.bFullMotionBlur = GMotionBlurFullMotionBlur < 0 ? MotionBlurParams.bFullMotionBlur : (GMotionBlurFullMotionBlur > 0);

		const FLOAT MinRotationThreshold=5.0f;
		MotionBlurParams.RotationThreshold = Max<FLOAT>(MinRotationThreshold, MotionBlurParams.RotationThreshold);
		const FLOAT MinTranslationThreshold=10.0f;
		MotionBlurParams.TranslationThreshold = Max<FLOAT>(MinTranslationThreshold, MotionBlurParams.TranslationThreshold);

		if(WorldSettings && !WorldSettings->bEnableSceneEffect)
		{
			ColorTransform.Reset();
		}

		// compute bloom radius within reasonable ranges
		// is done in full filter buffer resolution (supports up to 0..64 kernel size)
		BloomKernelSizeSmall = BlurBloomKernelSize * Clamp(InEffect->BloomSizeScaleSmall, 0.0f, 1.0f);
		// is done in half filter buffer resolution (scale is limited by 2 so it still support 64 kernel size)
		BloomKernelSizeMedium = BlurBloomKernelSize * Clamp(InEffect->BloomSizeScaleMedium, 0.25f, 2.0f);
		// is done in quarter filter buffer resolution (scale should be limited by 4 so it still support 64 kernel size,
		// however if feels better to have this range not as tightly clamped so we allow 8 and hope the clamped kernel
		// size not an issue.
		BloomKernelSizeLarge = BlurBloomKernelSize * Clamp(InEffect->BloomSizeScaleLarge, 1.0f, 8.0f);
	}

	/**
	 * Selects the correct shaders and renders the half-res variation of the uber postprocess.
	 * @param View - current view.
	 * @param LocalDepthOfFieldType - the type of depth-of-field.
	 * @param EffectSizeScale - amount by which to scale the radius of the effect.
	 */
	template<UBOOL bUseDoFBlurBuffer, UBOOL bUseSoftEdgeMotionBlur, UINT DepthOfFieldHalfResMode, UBOOL bTMotionBlur>
	void RenderVariationHalfRes(FViewInfo& View, EDOFType LocalDepthOfFieldType, FLOAT EffectSizeScale, UBOOL bSeparateTranslucency)
	{
		RenderVariationHalfRes_DoFAndMotionBlur( View, LocalDepthOfFieldType, EffectSizeScale, bUseSoftEdgeMotionBlur, bSeparateTranslucency );

		static FGlobalBoundShaderState BoundShaderState;
		TShaderMapRef<FUberPostProcessVertexShader> BlendVertexShader(GetGlobalShaderMap());
		TShaderMapRef<TUberHalfResPixelShader<bTMotionBlur, bUseDoFBlurBuffer, DepthOfFieldHalfResMode, bUseSoftEdgeMotionBlur> > BlendPixelShader(GetGlobalShaderMap());
		SetGlobalBoundShaderState(BoundShaderState, GFilterVertexDeclaration.VertexDeclarationRHI, *BlendVertexShader, *BlendPixelShader, sizeof(FFilterVertex));
		RenderVariationHalfRes( View, LocalDepthOfFieldType, bUseDoFBlurBuffer, *BlendVertexShader, *BlendPixelShader );
	}

	/**
	 * Renders the half-res depth-of-field and motion blur passes.
	 * @param View - current view.
	 * @param LocalDepthOfFieldType - the type of depth-of-field.
	 * @param EffectSizeScale - amount by which to scale the radius of the effect.
	 * @param bUseSoftEdgeMotionBlur - whether to enable soft edge motion blur.
	 * @param bSeparateTranslucency - ???
	 */
	void RenderVariationHalfRes_DoFAndMotionBlur(
		FViewInfo& View,
		EDOFType LocalDepthOfFieldType,
		FLOAT EffectSizeScale,
		UBOOL bUseSoftEdgeMotionBlur,
		UBOOL bSeparateTranslucency)
	{
		// full resolution (this can be much bigger than the viewport needed size, half and quarter is computed from that size)
		const UINT BufferSizeX = GSceneRenderTargets.GetBufferSizeX();
		const UINT BufferSizeY = GSceneRenderTargets.GetBufferSizeY();
		const UINT FilterDownsampleFactor = GSceneRenderTargets.GetFilterDownsampleFactor();
		// quarter resolution of the view without border
		const UINT DownsampledSizeX = View.RenderTargetSizeX / FilterDownsampleFactor;
		const UINT DownsampledSizeY = View.RenderTargetSizeY / FilterDownsampleFactor;

		FVector2D SampleMaskMin(0 / (FLOAT)BufferSizeX, 0 / (FLOAT)BufferSizeY);
		FVector2D SampleMaskMaxA((0 + View.SizeX - 1) / (FLOAT)BufferSizeX, (0 + View.SizeY - 1) / (FLOAT)BufferSizeY);

		// Depth Of Field
		if(LocalDepthOfFieldType == DOFType_SimpleDOF)
		{
			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("DOF"));

			//Use default quality param so we can add the depth of field quality param
			UINT Quality = 0;

			//if high quality, only render foreground.  If other quality, render both background and foreground
			//foreground should not clamp the min depth
			MinDepth = -MAX_FLT;
			MaxDepth = MAX_FLT;
			// render quarter res DepthOfField
			RenderGatherPass(View, EGD_DepthOfField, SRTI_FilterColor0, 1.0f, Quality);
			GaussianBlurFilterBuffer(View,View.SizeX, DownsampledSizeX + AntiLeakBorder, DownsampledSizeY + AntiLeakBorder, BlurKernelSize * EffectSizeScale, 1.0f, SRTI_FilterColor0, SampleMaskMin, SampleMaskMaxA);
			// SRTI_FilterColor0 now has the full scene/foreground DOF scene content
		}

		// SoftEdge MotionBlur
		if(bUseSoftEdgeMotionBlur)
		{
			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("SoftEdgeMotionBlur"));

			// render quarter res MotionBlur vector
			RenderGatherPass(View, EGD_MotionBlur, SRTI_FilterColor2, 1.0f);
			GaussianBlurFilterBuffer(View,View.SizeX, DownsampledSizeX + AntiLeakBorder, DownsampledSizeY + AntiLeakBorder, MotionBlurSoftEdgeKernelSize, 1.0f, SRTI_FilterColor2, SampleMaskMin, SampleMaskMaxA);

			// SRTI_FilterColor2 now has the blurred velocity vector content
		}

		// on some hardware TranslucencyBuffer and HalfResPostProcess overlaps so if we debug TranslucencyBuffer we don't draw that pass
#if WITH_D3D11_TESSELLATION
		extern INT GVisualizeTexture;
		if((GVisualizeTexture - 1) != (INT)TranslucencyBuffer)
		{
			if(LocalDepthOfFieldType == DOFType_BokehDOF)
			{
				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("BokehDOF"));

				FBokehDOFRenderer BokehDOFRenderer;

				BokehDOFRenderer.RenderBokehDOF(View,
					View.DepthOfFieldParams,
					BlurKernelSize * EffectSizeScale,
					DepthOfFieldQuality,
					BokehTexture,
					bSeparateTranslucency);
			}
		}
#endif // WITH_D3D11_TESSELLATION
	}

	/**
	* Render the half-res variation of the post process effect.
	* @param View - current view.
	* @param LocalDepthOfFieldType - the type of depth-of-field.
	* @param bUseDoFBlurBuffer - whether to use the depth-of-field blur buffer.
	* @param BlendVertexShader - the vertex shader with which to render.
	* @param BlendPixelShader - the pixel shader with which to render.
	*/
	void RenderVariationHalfRes(
		FViewInfo& View,
		EDOFType LocalDepthOfFieldType,
		UBOOL bUseDoFBlurBuffer,
		FUberPostProcessVertexShader* BlendVertexShader,
		FUberHalfResPixelShaderBase* BlendPixelShader )
	{
		// full resolution (this can be much bigger than the viewport needed size, half and quarter is computed from that size)
		const UINT BufferSizeX = GSceneRenderTargets.GetBufferSizeX();
		const UINT BufferSizeY = GSceneRenderTargets.GetBufferSizeY();
		// quarter resolution +2 pixel border
		const UINT FilterBufferSizeX = GSceneRenderTargets.GetFilterBufferSizeX();
		const UINT FilterBufferSizeY = GSceneRenderTargets.GetFilterBufferSizeY();
		const UINT FilterDownsampleFactor = GSceneRenderTargets.GetFilterDownsampleFactor();
		// half resolution
		const UINT HalfSizeX = BufferSizeX / 2;
		const UINT HalfSizeY = BufferSizeY / 2;
		// quarter resolution
		const UINT QuarterSizeX = BufferSizeX / FilterDownsampleFactor;
		const UINT QuarterSizeY = BufferSizeY / FilterDownsampleFactor;

		// on some hardware TranslucencyBuffer and HalfResPostProcess overlaps so if we debug TranslucencyBuffer we don't draw that pass
		extern INT GVisualizeTexture;
		if((GVisualizeTexture - 1) != (INT)TranslucencyBuffer)
		{
			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("UberHalfRes"));

			RHISetRenderTarget(GSceneRenderTargets.GetHalfResPostProcessSurface(), 0);

			// Set the DOF and bloom parameters
			BlendPixelShader->DOFParameters.SetPS(BlendPixelShader, View.DepthOfFieldParams);

			BlendPixelShader->PostProcessParameters.SetPS(BlendPixelShader, BlurKernelSize, DOFOcclusionTweak);

			BlendPixelShader->MotionBlurParameters.Set(
				BlendPixelShader->GetPixelShader(),
				View,
				MotionBlurParams);

			// Setup DoF blur buffer
			if(bUseDoFBlurBuffer)
			{
				SetTextureParameterDirectly(
					BlendPixelShader->GetPixelShader(),
					BlendPixelShader->DoFBlurBufferParameter,
					TStaticSamplerState<SF_Bilinear>::GetRHI(),
					GSceneRenderTargets.GetDoFBlurBufferTexture()
					);
			}

			// Setup the scene texture
			BlendPixelShader->SceneTextureParameters.Set(&View, BlendPixelShader, SF_Point);

			// half res scene point samples
			SetTextureParameterDirectly(
				BlendPixelShader->GetPixelShader(),
				BlendPixelShader->LowResPostProcessBufferPointParameter, 
				TStaticSamplerState<SF_Point>::GetRHI(),
				GSceneRenderTargets.GetTranslucencyBufferTexture() );

			if(LocalDepthOfFieldType == DOFType_BokehDOF)
			{
				SetTextureParameter(
					BlendPixelShader->GetPixelShader(),
					BlendPixelShader->BokehDOFLayerTextureParameter, 
					TStaticSamplerState<SF_Point>::GetRHI(),
					GSceneRenderTargets.GetBokehDOFTexture() );
			}

			FVector4 SceneCoordinate1ScaleBias(
				0.5f * HalfSizeX,
				-0.5f * HalfSizeY,
				0.5f * HalfSizeY + GPixelCenterOffset - View.RenderTargetY / 2,
				0.5f * HalfSizeX + GPixelCenterOffset - View.RenderTargetX / 2);

			// compute pixel position from screen pos, unify D3D9 half texel shift (done like this for cleaner code), flip y
			SetVertexShaderValue(BlendVertexShader->GetVertexShader(), BlendVertexShader->SceneCoordinate1ScaleBiasParameter, SceneCoordinate1ScaleBias);

			// used to scale to UV half res
			SetVertexShaderValue(BlendVertexShader->GetVertexShader(), BlendVertexShader->SceneCoordinate2ScaleBiasParameter,
				FVector4(
					1.0f / HalfSizeX,
					1.0f / HalfSizeY,
					0.5f + GPixelCenterOffset / HalfSizeY + SceneCoordinate1ScaleBias.Z / SceneCoordinate1ScaleBias.Y * 0.5f,
					0.5f + GPixelCenterOffset / HalfSizeX - SceneCoordinate1ScaleBias.W / SceneCoordinate1ScaleBias.X * 0.5f ));

			// used to scale to quarter res (which is left top screen +1 border texel)
			{
				FLOAT BorderScaleX = QuarterSizeX / (FLOAT)FilterBufferSizeX;
				FLOAT BorderScaleY = QuarterSizeY / (FLOAT)FilterBufferSizeY;
				SetVertexShaderValue(
					BlendVertexShader->GetVertexShader(),
					BlendVertexShader->SceneCoordinate3ScaleBiasParameter,
					FVector4(
						BorderScaleX / HalfSizeX,
						BorderScaleY / HalfSizeY,
						0.5f + (GPixelCenterOffset / HalfSizeY - View.RenderTargetY / (FLOAT)BufferSizeY) * BorderScaleY 
						+ SceneCoordinate1ScaleBias.Z / SceneCoordinate1ScaleBias.Y * 0.5f * BorderScaleY,
						0.5f + (GPixelCenterOffset / HalfSizeX - View.RenderTargetX / (FLOAT)BufferSizeX) * BorderScaleX 
						- SceneCoordinate1ScaleBias.W / SceneCoordinate1ScaleBias.X * 0.5f * BorderScaleX)
					);
			}

			// Draw a quad to do the half resolution post processing pass before the final uber post processing pass
			// UV is setup to map to the full resolution
			DrawDenormalizedQuad(
				View.RenderTargetX / 2, View.RenderTargetY / 2, View.RenderTargetSizeX / 2, View.RenderTargetSizeY / 2,
				View.RenderTargetX, View.RenderTargetY, View.RenderTargetSizeX, View.RenderTargetSizeY,
				HalfSizeX, HalfSizeY,
				BufferSizeX, BufferSizeY);

			RHICopyToResolveTarget(GSceneRenderTargets.GetHalfResPostProcessSurface(), FALSE, FResolveParams());
		}
	}
	
	// return luminance texture;
	//shanling@snda
	const FTexture2DRHIRef* RenderLuminance(FViewInfo& View, FLOAT EffectSizeScale, UBOOL bSeparateTranslucency)
	{

		SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("RenderLuminance"));

// 		const FLOAT MAX_SCENE_COLOR = 4.0f;
// 
// 		FVector2D RenderRects[10];//{1, 3, 9, 27, 81, 243};
// 
// 		const UINT RenderStepArraySize = sizeof(RenderRects) /  sizeof(FLOAT);
// 		const UINT FilterDownsampleFactor = GSceneRenderTargets.GetFilterDownsampleFactor();
// 
// 		const float SilhouetteBufferX = GSceneRenderTargets.GetSilhouetteBufferSizeX();
// 		const float SilhouetteBufferY = GSceneRenderTargets.GetSilhouetteBufferSizeY();
// 
// 		const float FilterBufferSizeX = GSceneRenderTargets.GetFilterBufferSizeX();
// 		const float FilterBufferSizeY = GSceneRenderTargets.GetFilterBufferSizeY();
// 
// 		const float DownsampledSizeX = View.RenderTargetSizeX >>1;/// FilterDownsampleFactor;
// 		const float DownsampledSizeY = View.RenderTargetSizeY >>1;/// FilterDownsampleFactor;
// 
// 		int i;
// 
// 		GToneMapingHistogramImageVertex.ReSet( View.RenderTargetSizeX, View.RenderTargetSizeY );
// 		RenderRects[0].X = RenderRects[0].Y = 1;
// 		for (i = 1; i <= RenderStepArraySize ; ++i)
// 		{
// 			RenderRects[i].X = RenderRects[i-1].X*3;
// 			RenderRects[i].Y = RenderRects[i-1].Y*3;
// 			if ( FilterBufferSizeX < RenderRects[i].X || FilterBufferSizeY < RenderRects[i].Y )
// 			{
// 				RenderRects[i].X = std::min( FilterBufferSizeX, RenderRects[i].X );
// 				RenderRects[i].Y = std::min( FilterBufferSizeY, RenderRects[i].Y );
// 				++i;
// 				RenderRects[i].X = DownsampledSizeX;
// 				RenderRects[i].Y = DownsampledSizeY;
// 				break;
// 			}
// 		}
// 
// 		UINT RenderStep = RenderStepArraySize;
// 
// 		FIntPoint Origin(0, 0);
// 		
// 		RHISetDepthState(TStaticDepthState<FALSE,CF_Always>::GetRHI());
// 		RHISetRasterizerState(TStaticRasterizerState<FM_Solid,CM_None>::GetRHI());
// 		RHISetBlendState(TStaticBlendState<>::GetRHI());
// 
 		TShaderMapRef<HDRVertexShader> VS(GetGlobalShaderMap());
 		HDRVertexShader* pVS = *VS;
// 
// 		FSamplerStateRHIParamRef TextureFilterLinearClamp = TStaticSamplerState<SF_Bilinear,AM_Clamp,AM_Clamp,AM_Clamp>::GetRHI();
// 		FSamplerStateRHIParamRef TextureFilterPointClamp = TStaticSamplerState<SF_Point,AM_Clamp,AM_Clamp,AM_Clamp>::GetRHI();
// 
// 		static FGlobalBoundShaderState RT2DownSampleShaderState;
// 
// 		// 1. Render to the downsampled filter buffer.
// 		{
// 			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("RenderLuminance - Original2DownSample"));
// 
// 			TShaderMapRef<FRT2DownSamplePixelShader> PSRT2DownSample(GetGlobalShaderMap());
// 			FRT2DownSamplePixelShader* pPSRT2DownSample = *PSRT2DownSample;
// 
// 			//PSRT2DownSample->
// 
// 			float DestRectX = RenderRects[i].X;
// 			float DestRectY = RenderRects[i].Y;
// 
// 			float SrcRectX = View.RenderTargetSizeX;//GSceneRenderTargets.GetBufferSizeX();
// 			float SrcRectY = View.RenderTargetSizeY;//GSceneRenderTargets.GetBufferSizeY();
// 
// 			FVector2D invtex( 1.f/GSceneRenderTargets.GetBufferSizeX(), 1.f/GSceneRenderTargets.GetBufferSizeY() );
// 
// 			// linear, render it
// 
// 			SetGlobalBoundShaderState(RT2DownSampleShaderState, 
// 				GFilterVertexDeclaration.VertexDeclarationRHI, 
// 				pVS, pPSRT2DownSample, sizeof(FFilterVertex));
// 
// 			//RHISetRenderTarget( GSceneRenderTargets.GetSilhouetteColorSurface(),
// 			//	GSceneRenderTargets.GetSceneDepthSurface() );
// 			//RHISetViewport(0,0,0.0f,DestRectX, DestRectY,1.0f);
// 			//GSceneRenderTargets.BeginRenderingSilhouettes();
// 			GSceneRenderTargets.BeginRenderingDownSampleForLuminanceColor();
// 
// 			RHIClear(TRUE,FLinearColor(0,0,0,0),FALSE,0,FALSE,0);
// 
// 			SetTextureParameterDirectly(
// 				pPSRT2DownSample->GetPixelShader(),
// 				pPSRT2DownSample->GetSrcTextureParam(),
// 				TextureFilterPointClamp,
// 				GSceneRenderTargets.GetSceneColorTexture());
// 			SetPixelShaderValue( pPSRT2DownSample->GetPixelShader(), pPSRT2DownSample->GetInvTexParam(), invtex );
// 
// 			DrawDenormalizedQuad(
// 				0, 0,										// DestRect Pos
// 				DestRectX, DestRectY,						// DestRect Size
// 				0, 0,										// SrcRect Pos
// 				SrcRectX, SrcRectY,							// SrcRect Size
// 				DestRectX, DestRectY,		// Dest Size
// 				GSceneRenderTargets.GetBufferSizeX(), GSceneRenderTargets.GetBufferSizeY()	// Src Size
// 				);
// 
// 			GSceneRenderTargets.FinishRenderingDownSampleForLuminanceColor();
// 			//GSceneRenderTargets.FinishRenderingSilhouettes();
// 			//RHICopyToResolveTarget(GSceneRenderTargets.GetSilhouetteColorSurface(), FALSE, FResolveParams());
// 		}
// 
// 		// 2. downsample 3*3 recursive
// 		ESceneRenderTargetTypes SrcRT = MAX_SCENE_RENDERTARGETS;
// 		ESceneRenderTargetTypes DestRT = MAX_SCENE_RENDERTARGETS;
// 
// 		{
// 			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("RenderLuminance - DownSampleCalculateLuminancePSwenkon"));
// 
// 			TShaderMapRef<FDownSampleCalculateLumPixelShader> PSDownSample(GetGlobalShaderMap());
// 			FDownSampleCalculateLumPixelShader* pPSDownSample = *PSDownSample;
// 
// 			static FGlobalBoundShaderState DownSampleShaderState;
// 
// 			FVector4 UVOffsets[9];			
// 
// 
// 			float DestRectX = RenderRects[i - 1 ].X;
// 			float DestRectY = RenderRects[i - 1 ].Y;
// 			float DestTexX = GSceneRenderTargets.GetFilterBufferSizeX();
// 			float DestTexY = GSceneRenderTargets.GetFilterBufferSizeY();
// 			//check(DestRectX <= DestTexX && DestRectY <= DestTexY);
// 							//if (n > 0)
// 			//{
// 			float SrcRectX = RenderRects[i ].X;
// 			float SrcRectY = RenderRects[i ].Y;
// 			float SrcTexX = DownsampledSizeX;//GSceneRenderTargets.GetFilterBufferSizeX();
// 			float SrcTexY = DownsampledSizeY;//GSceneRenderTargets.GetFilterBufferSizeY();
// 				//check(SrcRectX <= SrcTexX && SrcRectY <= SrcTexY);
// 				//}
// 
// 			float fUOffsetRcq = 1.0f / SrcTexX - 0.00001f;
// 			float fVOffsetRcq = 1.0f / SrcTexY - 0.00001f;
// 			float fUMax = SrcRectX / SrcTexX;
// 			float fVMax = SrcRectY / SrcTexY;
// 
// 			UVOffsets[0].X = -fUOffsetRcq;
// 			UVOffsets[0].Y = -fVOffsetRcq;
// 			UVOffsets[1].X = 0;
// 			UVOffsets[1].Y = -fVOffsetRcq;
// 			UVOffsets[2].X = fUOffsetRcq;
// 			UVOffsets[2].Y = -fVOffsetRcq;
// 			UVOffsets[3].X = -fUOffsetRcq;
// 			UVOffsets[3].Y = 0;
// 			UVOffsets[4].X = 0;
// 			UVOffsets[4].Y = 0;
// 			UVOffsets[5].X = fUOffsetRcq;
// 			UVOffsets[5].Y = 0;
// 			UVOffsets[6].X = -fUOffsetRcq;
// 			UVOffsets[6].Y = fVOffsetRcq;
// 			UVOffsets[7].X = 0;
// 			UVOffsets[7].Y = fVOffsetRcq;
// 			UVOffsets[8].X = fUOffsetRcq;
// 			UVOffsets[8].Y = fVOffsetRcq;
// 			GSceneRenderTargets.BeginRenderingLuminanceColor();
// 			//RHISetRenderTarget(GSceneRenderTargets.GetSurfaceByIndex(DestRT), FSurfaceRHIRef());
// 				//RHISetViewport(0,0,0.0f,DestRectX,DestRectY,1.0f);
// 
// 			SetGlobalBoundShaderState(DownSampleShaderState, 
// 				GFilterVertexDeclaration.VertexDeclarationRHI, 
// 				pVS, pPSDownSample, sizeof(FFilterVertex));
// 				pPSDownSample->SetParameters(UVOffsets);
// 
// #if !PS3
// 				// Clear the buffer to black.
// 			RHIClear(TRUE,FLinearColor(0,0,0,0),FALSE,0,FALSE,0);
// #endif
// 
// 				// point filter
// 
// 				SetTextureParameterDirectly(
// 					pPSDownSample->GetPixelShader(),
// 					pPSDownSample->GetSrcTextureParam(),
// 					TextureFilterPointClamp,
// 					GSceneRenderTargets.GetDownSampleForLuminanceTexture());
// 
// 			
// 
// 
// 				DrawDenormalizedQuad(
// 					0, 0,										// DestRect Pos
// 					DestRectX, DestRectY,						// DestRect Size
// 					0, 0,										// SrcRect Pos
// 					SrcRectX, SrcRectY,							// SrcRect Size
// 					DestTexX, DestTexY,							// Dest Size
// 					SrcTexX, SrcTexY							// Src Size
// 					);
// 
// 
// 				GSceneRenderTargets.FinishRenderingLuminanceColor();
// 			
// 		}
// 
// 		{
// 			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("RenderLuminance - DownSample"));
// 			
// 			TShaderMapRef<FDownSamplePixelShader> PSDownSample(GetGlobalShaderMap());
// 			FDownSamplePixelShader* pPSDownSample = *PSDownSample;
// 
// 			static FGlobalBoundShaderState DownSampleShaderState;
// 
// 			FVector4 UVOffsets[9];			
// 
// 			for (int n = 1 ; n < i; ++n)
// 			{
// 				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("RenderLuminance - SubSample"));
// 
// 				float DestRectX = RenderRects[i - n - 1 ].X;
// 				float DestRectY = RenderRects[i - n - 1 ].Y;
// 				float DestTexX = GSceneRenderTargets.GetFilterBufferSizeX();
// 				float DestTexY = GSceneRenderTargets.GetFilterBufferSizeY();
// 				//check(DestRectX <= DestTexX && DestRectY <= DestTexY);
// 
// 				//if (n > 0)
// 				//{
// 
// 				float SrcRectX = RenderRects[i - n].X;
// 				float SrcRectY = RenderRects[i - n].Y;
// 				float SrcTexX = GSceneRenderTargets.GetFilterBufferSizeX();
// 				float SrcTexY = GSceneRenderTargets.GetFilterBufferSizeY();
// 				//check(SrcRectX <= SrcTexX && SrcRectY <= SrcTexY);
// 				//}
// 
// 				float fUOffsetRcq = 1.0f / SrcTexX - 0.00001f;
// 				float fVOffsetRcq = 1.0f / SrcTexY - 0.00001f;
// 
// 				float fUMax = SrcRectX / SrcTexX;
// 				float fVMax = SrcRectY / SrcTexY;
// 
// 				UVOffsets[0].X = -fUOffsetRcq;
// 				UVOffsets[0].Y = -fVOffsetRcq;
// 				UVOffsets[1].X = 0;
// 				UVOffsets[1].Y = -fVOffsetRcq;
// 				UVOffsets[2].X = fUOffsetRcq;
// 				UVOffsets[2].Y = -fVOffsetRcq;
// 				UVOffsets[3].X = -fUOffsetRcq;
// 				UVOffsets[3].Y = 0;
// 				UVOffsets[4].X = 0;
// 				UVOffsets[4].Y = 0;
// 				UVOffsets[5].X = fUOffsetRcq;
// 				UVOffsets[5].Y = 0;
// 				UVOffsets[6].X = -fUOffsetRcq;
// 				UVOffsets[6].Y = fVOffsetRcq;
// 				UVOffsets[7].X = 0;
// 				UVOffsets[7].Y = fVOffsetRcq;
// 				UVOffsets[8].X = fUOffsetRcq;
// 				UVOffsets[8].Y = fVOffsetRcq;
// 
// 				GSceneRenderTargets.BeginRenderingLuminanceColor();
// 				//RHISetRenderTarget(GSceneRenderTargets.GetSurfaceByIndex(DestRT), FSurfaceRHIRef());
// 				//RHISetViewport(0,0,0.0f,DestRectX,DestRectY,1.0f);
// 
// 				SetGlobalBoundShaderState(DownSampleShaderState, 
// 					GFilterVertexDeclaration.VertexDeclarationRHI, 
// 					pVS, pPSDownSample, sizeof(FFilterVertex));
// 
// 				pPSDownSample->SetParameters(UVOffsets);
// 
// #if !PS3
// 				// Clear the buffer to black.
// 				RHIClear(TRUE,FLinearColor(0,0,0,0),FALSE,0,FALSE,0);
// #endif
// 
// 				// point filter
// 			
// 				SetTextureParameterDirectly(
// 					pPSDownSample->GetPixelShader(),
// 					pPSDownSample->GetSrcTextureParam(),
// 					TextureFilterPointClamp,
// 					GSceneRenderTargets.GetLuminanceTexture());
// 				
// 				
// 
// 				DrawDenormalizedQuad(
// 					0, 0,										// DestRect Pos
// 					DestRectX, DestRectY,						// DestRect Size
// 					0, 0,										// SrcRect Pos
// 					SrcRectX, SrcRectY,							// SrcRect Size
// 					DestTexX, DestTexY,							// Dest Size
// 					SrcTexX, SrcTexY							// Src Size
// 					);
// 
// 
// 				GSceneRenderTargets.FinishRenderingLuminanceColor();
// 				//RHICopyToResolveTarget(
// 				//	GSceneRenderTargets.GetSurfaceByIndex(DestRT), FALSE, 
// 				//	FResolveParams(FResolveRect(),CubeFace_PosX,
// 				//	GSceneRenderTargets.GetTextureByIndex(DestRT)));
// 			}
// 		}
// 
// 		{
// 			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("RenderLuminance - CalculateLuminance"));
// 
// 
// 			float DestTexX = GSceneRenderTargets.GetFilterBufferSizeX();
// 			float DestTexY = GSceneRenderTargets.GetFilterBufferSizeY();
// 
// 
// 	
// 
// 			TShaderMapRef<FCalculateLuminanceShader> PSCalculateLuminance(GetGlobalShaderMap());
// 			FCalculateLuminanceShader* pPSCalculateLuminance = *PSCalculateLuminance;
// 
// 			GSceneRenderTargets.BeginRenderingLuminanceColor();
// 
// 			RHISetBlendState( TStaticBlendState<BO_Add,BF_One,BF_Zero,BO_Add,BF_One,BF_Zero,CF_Always,1>::GetRHI() );
// 			RHISetDepthState( TStaticDepthState<FALSE,CF_Always>::GetRHI() );
// 			RHISetColorWriteMask( CW_RGBA );
// 
// 			static FGlobalBoundShaderState BoundShaderCalculateLuminance;
// 			SetGlobalBoundShaderState( BoundShaderCalculateLuminance, GFilterVertexDeclaration.VertexDeclarationRHI, pVS, pPSCalculateLuminance, sizeof(FFilterVertex) );
// 
// #if !PS3
// 			// Clear the buffer to black.
// 			RHIClear(TRUE,FLinearColor(0,0,0,0),FALSE,0,FALSE,0);
// #endif
// 
// 			// point filter
// 			SetTextureParameterDirectly(
// 				pPSCalculateLuminance->GetPixelShader(),
// 				pPSCalculateLuminance->GetSrcTextureParam(),
// 				TextureFilterPointClamp,
// 				GSceneRenderTargets.GetLuminanceTexture());
// 
// 			DrawDenormalizedQuad(
// 				0, 0,										// DestRect Pos
// 				DestTexX, DestTexX,						// DestRect Size
// 				0, 0,										// SrcRect Pos
// 				DestTexX, DestTexX,							// SrcRect Size
// 				DestTexX, DestTexX,							// Dest Size
// 				DestTexX, DestTexX							// Src Size
// 				);
// 			
// 
// 			GSceneRenderTargets.FinishRenderingLuminanceColor();
// 		}
// 
// 		{
// 			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("RenderLuminance - ToneMapingHistogramImage"));
// 
// 			GSceneRenderTargets.BeginRenderingToneMapingHistogramImage();
// 			//GSceneRenderTargets.BeginRenderingSilhouettes();
// #if !PS3
// 			// Clear the buffer to black.
// 			RHIClear(TRUE,FLinearColor(0,0,0,0),FALSE,0,FALSE,0);
// #endif
// 
// 			RHISetBlendState( TStaticBlendState<BO_Add,BF_One,BF_One,BO_Add,BF_One,BF_Zero,CF_Always,1>::GetRHI() );
// 			RHISetDepthState( TStaticDepthState<FALSE,CF_Always>::GetRHI() );
// 			//RHISetColorWriteMask( CW_RED );
// 
// 			TShaderMapRef<FToneMapingHistogramImageVertexShader> ToneMapingHIVS(GetGlobalShaderMap());
// 			TShaderMapRef<FToneMapingHistogramImagePixelShader>  ToneMapingHIPS(GetGlobalShaderMap());
// 
// 			static FGlobalBoundShaderState BoundShaderToneMapingHistogramImage;
// 			SetGlobalBoundShaderState(BoundShaderToneMapingHistogramImage, 
// 				GToneMapingHistogramImageVertexDeclaration.VertexDeclarationRHI, 
// 				((FToneMapingHistogramImageVertexShader*)*ToneMapingHIVS), ((FToneMapingHistogramImagePixelShader*)*ToneMapingHIPS), sizeof(FToneMapingHistogramImageVertex));
// 
// 
// 			SetVertexShaderTextureParameter(
// 				((FToneMapingHistogramImageVertexShader*)*ToneMapingHIVS)->GetVertexShader(),
// 				((FToneMapingHistogramImageVertexShader*)*ToneMapingHIVS)->GetSrcTextureParam(),
// 				//TextureFilterPointClamp,
// 				GSceneRenderTargets.GetDownSampleForLuminanceTexture());
// 				//GSceneRenderTargets.GetSceneColorTexture());
// 
// 			SetVertexShaderTextureParameter(
// 				((FToneMapingHistogramImageVertexShader*)*ToneMapingHIVS)->GetVertexShader(),
// 				((FToneMapingHistogramImageVertexShader*)*ToneMapingHIVS)->GetLuminanceTextureParam(),
// 				//TextureFilterPointClamp,
// 				GSceneRenderTargets.GetLuminanceTexture());
// 
// 			float SrcRectX = View.RenderTargetSizeX >> 1;//View.RenderTargetSizeX;//GSceneRenderTargets.GetBufferSizeX();
// 			float SrcRectY = View.RenderTargetSizeY >> 1;//View.RenderTargetSizeY;//GSceneRenderTargets.GetBufferSizeY();
// 
// 			float DestTexX = DownsampledSizeX;//.GetFilterBufferSizeX();
// 			float DestTexY = DownsampledSizeY;//GSceneRenderTargets.GetFilterBufferSizeY();
// 
// 			FVector4   vBufferWidthHeight( SrcRectX, SrcRectY, SrcRectX/DownsampledSizeX/*GSceneRenderTargets.GetFilterBufferSizeX()*//SrcRectX,
// 				SrcRectY/DownsampledSizeY/*GSceneRenderTargets.GetFilterBufferSizeX()*//SrcRectY );
// 			FVector2D  vInvLuminanceBuffer( 1.f/DestTexX, 1.f/DestTexY );
// 
// 			SetVertexShaderValue( ((FToneMapingHistogramImageVertexShader*)*ToneMapingHIVS)->GetVertexShader(), ((FToneMapingHistogramImageVertexShader*)*ToneMapingHIVS)->GetBufferWidthHeightParam(), vBufferWidthHeight );
// 
// 			SetVertexShaderValue( ((FToneMapingHistogramImageVertexShader*)*ToneMapingHIVS)->GetVertexShader(), ((FToneMapingHistogramImageVertexShader*)*ToneMapingHIVS)->GetInvLuminanceBufferWidthHeightParam(), vInvLuminanceBuffer );
// 
// 			RHISetStreamSource( 0, GToneMapingHistogramImageVertex.VertexBufferRHI, sizeof( FToneMapingHistogramImageVertex ), 0, 0, 0, 0 );
// 
// 			RHIDrawPrimitive( PT_PointSprite, 0, SrcRectX * SrcRectY );
// 
// 			//GSceneRenderTargets.FinishRenderingSilhouettes();
// 			GSceneRenderTargets.FinishRenderingToneMapingHistogramImage();
// 
// 			//RHISetColorWriteMask( CW_RGB );
// 		}
// 
// 		{
// 			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("RenderLuminance - CalculateLuminanceParameterPS"));
// 			
// 			float DestTexX = RenderRects[i].X;//GSceneRenderTargets.GetFilterBufferSizeX();
// 			float DestTexY = RenderRects[i].Y;//GSceneRenderTargets.GetFilterBufferSizeY();
// 
// 			UINT PixelNum = DestTexX * DestTexY;
// 
// 			FVector4  vPercentParameter( PixelNum * 0.01f, PixelNum * 0.5f, PixelNum * 0.99f, 1.f );
// 			FVector2D vInvSrcLuminanceTex( 1.f/DestTexX, 1.f/DestTexY );
// 			FVector4  vResult;
// 			UINT      uCurPixel = 0;
// 			INT       uCurSlot  = 0;
// 		
// 			FSurfaceRHIRef   surface = GSceneRenderTargets.GetToneMapingHistogramImageColorSurface();
// 			FSurfaceRHIRef   LuminanceGvgsurface = GSceneRenderTargets.GetLuminanceColorSurface();
// 			
// 			TArray<FFloat16Color> GvgData;
// 			RHIReadSurfaceFloatData( LuminanceGvgsurface, 0 , 0 , 0, 0, GvgData, CubeFace_PosX );
// 
// 			TArray<float> data;
// 			RHIReadR32FSurfaceOriginalData( surface, 0, 0, 1023, 0, data, CubeFace_PosX );
// 			int num = 0;//data.Num();
// 
// 			for ( int i = 0 ; i < data.Num() ; ++ i )
// 			{
// 				num += data( i );
// 			}
// 
// 			for(  ; uCurSlot < data.Num() ; ++uCurSlot )
// 			{
// 				uCurPixel += data( uCurSlot );
// 				if ( uCurPixel > vPercentParameter.X )
// 				{
// 					vResult.X = uCurSlot;
// 					++uCurSlot;
// 					break;
// 				}
// 			}
// 
// 			for(  ; uCurSlot < data.Num() ; ++uCurSlot )
// 			{
// 				uCurPixel += data( uCurSlot );
// 				if ( uCurPixel > vPercentParameter.Y )
// 				{
// 					vResult.Y = uCurSlot;
// 					++uCurSlot;
// 					break;
// 				}
// 			}
// 
// 			for(  ; uCurSlot < data.Num() ; ++uCurSlot )
// 			{
// 				uCurPixel += data( uCurSlot );
// 				if ( uCurPixel > vPercentParameter.Z )
// 				{
// 					vResult.Z = uCurSlot;
// 					++uCurSlot;
// 					break;
// 				}
// 			}
// 
// 			float slotStup =( GvgData(0).G - GvgData(0).B )/ 1024; 
// 			LuminanceParameter.X = vResult.X * slotStup + GvgData(0).B;
// 			LuminanceParameter.Y = vResult.Y * slotStup + GvgData(0).B;
// 			LuminanceParameter.Z = vResult.Z * slotStup + GvgData(0).B;
// 			TCHAR str[1024] = {0};
// 			
// 			swprintf( str, 1024, L"LuminanceParameter : %d, %f, %f ,%f, %f, %f\n", num, LuminanceParameter.X, LuminanceParameter.Y, LuminanceParameter.Z, (FLOAT)GvgData(0).R, (FLOAT)GvgData(0).G );
// 			OutputDebugString( str );
// 		}
		{
			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("RenderLuminance - CalculateAdaptedLumPS"));

			GSceneRenderTargets.BeginRenderingAdaptedLuminance();

			//for( int i = 0 ; i < 4 ; ++i )
			//{
			//	int x = ( i << 1) & 2;
			//	int y = i & 2;
			//	int f = x + y;
			//}
#if !PS3
			// Clear the buffer to black.
			RHIClear(TRUE,FLinearColor(0,0,0,0),FALSE,0,FALSE,0);
#endif

			RHISetBlendState( TStaticBlendState<BO_Add,BF_One,BF_Zero,BO_Add,BF_One,BF_Zero,CF_Always,1>::GetRHI() );
			RHISetDepthState( TStaticDepthState<FALSE,CF_Always>::GetRHI() );
			//RHISetColorWriteMask( CW_RED );
			TShaderMapRef<FCalculateAdaptedLumPixelShader>  CalculateAdaptedLumHIPS(GetGlobalShaderMap());

			static FGlobalBoundShaderState BoundShaderToneMapingHistogramImage;
			SetGlobalBoundShaderState(BoundShaderToneMapingHistogramImage, 
				GFilterVertexDeclaration.VertexDeclarationRHI, 
				pVS, ((FCalculateAdaptedLumPixelShader*)*CalculateAdaptedLumHIPS), sizeof(FFilterVertex));

			//now only use pp Parameter
			/*if ( GIsEditor && !bUseMatineeLuminanceParameter )
			{
				LuminanceParameter.X = NewTonemapingParameter.X;
				LuminanceParameter.Y = NewTonemapingParameter.Y;
				LuminanceParameter.Z = NewTonemapingParameter.Z;
				LuminanceParameter.W = NewTonemapingParameter.W;
			}
 			else */
				LuminanceParameter = View.LuminanceParameter;
			/*if(GWorld && GWorld->GetWorldInfo()) // oscar@sndas
 			{
 				LuminanceParameter.X = GWorld->GetWorldInfo()->NewLimitLuminance;
 				LuminanceParameter.Y = GWorld->GetWorldInfo()->NewMiddleLuminance;
				LuminanceParameter.Z = GWorld->GetWorldInfo()->NewWhiteLuminance;
 				LuminanceParameter.W = GWorld->GetWorldInfo()->NewAvgLuminanceParameter;
			}
			else
			{
				LuminanceParameter.X = 0.0f;
				LuminanceParameter.Y = 0.5f;
				LuminanceParameter.Z = 1.0f;
				LuminanceParameter.W = 0.5f;
			}*/
			
			SetPixelShaderValue( ((FCalculateAdaptedLumPixelShader*)*CalculateAdaptedLumHIPS)->GetPixelShader(), 
				((FCalculateAdaptedLumPixelShader*)*CalculateAdaptedLumHIPS)->GetLuminanceParameter(), LuminanceParameter );

			DrawDenormalizedQuad(
				0, 0,										// DestRect Pos
				1, 1,						// DestRect Size
				0, 0,										// SrcRect Pos
				1, 1,							// SrcRect Size
				1, 1,							// Dest Size
				1, 1							// Src Size
				);
			
			GSceneRenderTargets.FinishRenderingAdaptedLuminance();
#if !FINAL_RELEASE
			// Opt-in readback of the actual exposure inputs; never runs in normal play.
			static const UBOOL bTraceHDR = ParseParam(appCmdLine(), TEXT("TraceHDR"));
			static INT TraceHDRFrame = 0;
			if (bTraceHDR && TraceHDRFrame <= 1800 && TraceHDRFrame++ % 600 == 0)
			{
				TArray<FFloat16Color> ExposurePixels;
				RHIReadSurfaceFloatData(GSceneRenderTargets.GetAdaptedLuminanceSurface(), 0, 0, 0, 0, ExposurePixels, CubeFace_PosX);
				if (ExposurePixels.Num())
				{
					const FLinearColor Exposure(ExposurePixels(0));
					debugf(TEXT("HDRTrace: frame=%d authored=(%g,%g,%g,%g) texture=(%g,%g,%g,%g)"),
						TraceHDRFrame - 1, LuminanceParameter.X, LuminanceParameter.Y, LuminanceParameter.Z, LuminanceParameter.W,
						Exposure.R, Exposure.G, Exposure.B, Exposure.A);
				}
			}
#endif
		}
		return &(GSceneRenderTargets.GetTextureByIndex(AdaptedLuminance));
		
		// move to UberPostProcessBlendPixelShader.usf
		//// 3. tone mapping for the original scene color
		//{
		//	SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("RenderLuminance - ToneMapping"));

		//	TShaderMapRef<FToneMappingPixelShader> PSToneMapping(GetGlobalShaderMap());
		//	FToneMappingPixelShader* pPSToneMapping = *PSToneMapping;

		//	GSceneRenderTargets.BeginRenderingSceneColor();

		//	static FGlobalBoundShaderState ToneMappingShaderState;

		//	SetGlobalBoundShaderState(ToneMappingShaderState, 
		//		GFilterVertexDeclaration.VertexDeclarationRHI, 
		//		pVS, pPSToneMapping, sizeof(FFilterVertex));

		//	pPSToneMapping->SetParameters(1.0f, 1.0f);

		//	SetTextureParameterDirectly(
		//		pPSToneMapping->GetPixelShader(),
		//		pPSToneMapping->GetSrcTextureParam(),
		//		TextureFilterPointClamp,
		//		GSceneRenderTargets.GetSceneColorTexture());

		//	SetTextureParameterDirectly(
		//		pPSToneMapping->GetPixelShader(),
		//		pPSToneMapping->GetLuminanceTextureParam(),
		//		TextureFilterPointClamp,
		//		GSceneRenderTargets.GetTextureByIndex(DestRT));

		//	float SrcRectX = GSceneRenderTargets.GetBufferSizeX();
		//	float SrcRectY = GSceneRenderTargets.GetBufferSizeY();

		//	RHIClear(TRUE,FLinearColor(0,0,0,0),FALSE,0,FALSE,0);

		//	DrawDenormalizedQuad(
		//		0, 0,										// DestRect Pos
		//		SrcRectX, SrcRectY,							// DestRect Size
		//		0, 0,										// SrcRect Pos
		//		SrcRectX, SrcRectY,							// SrcRect Size
		//		SrcRectX, SrcRectY,							// Dest Size
		//		SrcRectX, SrcRectY							// Src Size
		//		);

		//	GSceneRenderTargets.FinishRenderingSceneColor();
		//}
	}
	//shanling@snda 

	// generate quarter resolution bloom content
	void RenderBloom(FViewInfo& View, FLOAT EffectSizeScale, UBOOL bSeparateTranslucency)
	{
		SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Bloom"));

		// full resolution (this can be much bigger than the viewport needed size, half and quarter is computed from that size)
		const UINT BufferSizeX = GSceneRenderTargets.GetBufferSizeX();
		const UINT BufferSizeY = GSceneRenderTargets.GetBufferSizeY();
		// quarter resolution +2 pixel border
		const UINT FilterBufferSizeX = GSceneRenderTargets.GetFilterBufferSizeX();
		const UINT FilterBufferSizeY = GSceneRenderTargets.GetFilterBufferSizeY();
		const UINT FilterDownsampleFactor = GSceneRenderTargets.GetFilterDownsampleFactor();
		// quarter resolution of the view without border
		const UINT DownsampledSizeX = View.RenderTargetSizeX / FilterDownsampleFactor;
		const UINT DownsampledSizeY = View.RenderTargetSizeY / FilterDownsampleFactor;
		FVector2D SampleMaskMin(0 / (FLOAT)BufferSizeX, 0 / (FLOAT)BufferSizeY);
		FVector2D SampleMaskMaxA((0 + View.SizeX - 1) / (FLOAT)BufferSizeX, (0 + View.SizeY - 1) / (FLOAT)BufferSizeY);
		FVector2D SampleMaskMaxB((0 + View.SizeX / 2 - 1) / (FLOAT)BufferSizeX, (0 + View.SizeY / 2 - 1) / (FLOAT)BufferSizeY);
		FVector2D SampleMaskMaxC((0 + View.SizeX / 4 - 1) / (FLOAT)BufferSizeX, (0 + View.SizeY / 4 - 1) / (FLOAT)BufferSizeY);

		// the color was range compressed to fit in the range so we need to decompress it
		const FLOAT MAX_SCENE_COLOR = 4.0f;

		// create source for bloom in quarter resolution in #1
		RenderGatherPass(View, EGD_Bloom, SRTI_FilterColor1, MAX_SCENE_COLOR, BloomQuality, bSeparateTranslucency);

		FIntPoint Origin(1, 1);

		if(BloomType == 0)
		{
			// one Gaussian bell curve (limited radius but linear performance with radius and look)
			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Blur 1 Gaussian 1 resolution"));

			// blur #1
			GaussianBlurFilterBuffer(View,View.SizeX, DownsampledSizeX + AntiLeakBorder, DownsampledSizeY + AntiLeakBorder, BloomKernelSizeMedium * EffectSizeScale, 1.0f, SRTI_FilterColor1, SampleMaskMin, SampleMaskMaxA);
		}
		else if(BloomType == 1)
		{
			// 2 Gaussian bell curves combined
			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Blur 2 Gaussians 2 resolutions"));

			// downsample #1 to #2
			{	
				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Downsample to low res"));

				DrawDownsampledTexture(
					GSceneRenderTargets.GetFilterColorSurface(SRTI_FilterColor2),
					GSceneRenderTargets.GetFilterColorTexture(SRTI_FilterColor2),
					GSceneRenderTargets.GetFilterColorTexture(SRTI_FilterColor1),
					Origin,
					FIntRect(Origin, Origin + FIntPoint(DownsampledSizeX, DownsampledSizeY)),
					FIntPoint(FilterBufferSizeX, FilterBufferSizeY),
					FIntPoint(FilterBufferSizeX, FilterBufferSizeY));
			}

			// now:
			//   #1: 1:1 res 
			//   #2: 1:2 res

			// multiple times Gaussian blur is like adding radius so we blur less as be blur again
			FLOAT Pass1KernelSize = BloomKernelSizeMedium - BloomKernelSizeSmall;

			// blur #2
			// adjust SampleMaskMin, SampleMaskMax ?				
			// * 0.5f on kernel size as this is 1/2 resolution
			{
				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Blur low res"));

				GaussianBlurFilterBuffer(View,View.SizeX, DownsampledSizeX / 2 + AntiLeakBorder, DownsampledSizeY / 2 + AntiLeakBorder, Pass1KernelSize * EffectSizeScale * 0.5f, 1.0f, SRTI_FilterColor2, SampleMaskMin, SampleMaskMaxB);
			}

			FLOAT BloomWeightMedium = 1.0f - BloomWeightSmall;

			checkSlow(BloomWeightMedium >= 0.0f && BloomWeightMedium <= 1.0f);

			// #1 = #1 * .. + #2 * ..
			{
				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Combine"));

				CombineFilterBuffer(DownsampledSizeX + AntiLeakBorder, DownsampledSizeY + AntiLeakBorder, SRTI_FilterColor1, BloomWeightSmall, SRTI_FilterColor2, BloomWeightMedium);
			}

			// blur #1
			{
				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Blur result"));

				GaussianBlurFilterBuffer(View,View.SizeX, DownsampledSizeX + AntiLeakBorder, DownsampledSizeY + AntiLeakBorder, BloomKernelSizeSmall * EffectSizeScale, 1.0f, SRTI_FilterColor1, SampleMaskMin, SampleMaskMaxA);
			}
		}
		else if(BloomType == 2)
		{				
			// 3 Gaussian bell curves combined
			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Blur 3 Gaussians 3 resolutions"));

			{	
				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Downsample to medium res"));

				// downsample #1 to #2
				DrawDownsampledTexture(
					GSceneRenderTargets.GetFilterColorSurface(SRTI_FilterColor2),
					GSceneRenderTargets.GetFilterColorTexture(SRTI_FilterColor2),
					GSceneRenderTargets.GetFilterColorTexture(SRTI_FilterColor1),
					Origin,
					FIntRect(Origin, Origin + FIntPoint(DownsampledSizeX, DownsampledSizeY)),
					FIntPoint(FilterBufferSizeX, FilterBufferSizeY),
					FIntPoint(FilterBufferSizeX, FilterBufferSizeY));
			}

			// now:
			//   #0: 1:4 res
			//   #1: 1:1 res 
			//   #2: 1:2 res

			// multiple times Gaussian blur is like adding radius so we blur less as be blur again
			FLOAT Pass1KernelSize = BloomKernelSizeMedium - BloomKernelSizeSmall;

			{
				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Blur medium res"));

				// blur #2
				// adjust SampleMaskMin, SampleMaskMax ?				
				// * 0.5f on kernel size as this is 1/2 resolution
				// ming@snda - fix 
				GaussianBlurFilterBuffer(View,View.SizeX, DownsampledSizeX / 2 + AntiLeakBorder, DownsampledSizeY / 2 + AntiLeakBorder, Pass1KernelSize * EffectSizeScale * 0.5f, 1.0f, SRTI_FilterColor2, SampleMaskMin, SampleMaskMaxB);
			}

			// downsample #2 to #0
			{
				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Downsample to low res"));

				DrawDownsampledTexture(
					GSceneRenderTargets.GetFilterColorSurface(SRTI_FilterColor0),
					GSceneRenderTargets.GetFilterColorTexture(SRTI_FilterColor0),
					GSceneRenderTargets.GetFilterColorTexture(SRTI_FilterColor2),
					Origin,
					FIntRect(Origin, Origin + FIntPoint(DownsampledSizeX / 2, DownsampledSizeY / 2)),
					FIntPoint(FilterBufferSizeX, FilterBufferSizeY),
					FIntPoint(FilterBufferSizeX, FilterBufferSizeY));
			}

			// multiple times Gaussian blur is like adding radius so we blur less as be blur again
			FLOAT Pass2KernelSize = BloomKernelSizeLarge - BloomKernelSizeSmall;

			// blur #0
			// adjust SampleMaskMin, SampleMaskMax ?				
			// * 0.25f on kernel size as this is 1/4 resolution
			{
				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Blur high res"));

				GaussianBlurFilterBuffer(View,View.SizeX, DownsampledSizeX / 4 + AntiLeakBorder, DownsampledSizeY / 4 + AntiLeakBorder, Pass2KernelSize * EffectSizeScale * 0.25f, 1.0f, SRTI_FilterColor0, SampleMaskMin, SampleMaskMaxC);
			}

			FLOAT BloomWeightMedium = 1.0f - BloomWeightSmall - BloomWeightLarge;

			checkSlow(BloomWeightMedium >= 0.0f && BloomWeightMedium <= 1.0f);

			// #1 = #1 * .. + #2 *.. + #0 * ..
			{
				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Combine"));

				CombineFilterBuffer(DownsampledSizeX + AntiLeakBorder, DownsampledSizeY + AntiLeakBorder, SRTI_FilterColor1, BloomWeightSmall, SRTI_FilterColor2, BloomWeightMedium, SRTI_FilterColor0, BloomWeightLarge);
			}

			// blur #1
			{
				SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Blur result"));

				GaussianBlurFilterBuffer(View,View.SizeX, DownsampledSizeX + AntiLeakBorder, DownsampledSizeY + AntiLeakBorder, BloomKernelSizeSmall * EffectSizeScale, 1.0f, SRTI_FilterColor1, SampleMaskMin, SampleMaskMaxA);
			}
		}
		else if(BloomType == 3)
		{
			// one Gaussian bell curve, multi pass (unlimited radius, but performance can suffer quickly, for backwards compatibility)
			// radius can only be chosen divisible by 4 (e.g. 0,4,8,12,..)
			SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Blur 1 Gaussian multipass if needed"));

			// blur #1
			const INT BloomKernelTexels = appTrunc(BloomKernelSizeMedium / FilterDownsampleFactor);
			// Determine how many blur passes will be needed based on the blur kernels size, where each blur pass doubles the effective blur size
			const INT NumPasses = appCeilLogTwo((BloomKernelTexels + MAX_FILTER_SAMPLES - 1) / MAX_FILTER_SAMPLES) + 1;

			for (INT BlurPassIndex = 0; BlurPassIndex < NumPasses; BlurPassIndex++)
			{
				//@todo - when doing multiple passes, allow blurring on increments less than (1 << NumPasses) * MAX_FILTER_SAMPLES
				INT PassBlurKernel = NumPasses == 1 ? BloomKernelTexels : MAX_FILTER_SAMPLES;
				// Scale up by the next power of two
				FLOAT PassBlurScale = 1 << BlurPassIndex;
				GaussianBlurFilterBuffer(View,View.SizeX, DownsampledSizeX + AntiLeakBorder, DownsampledSizeY + AntiLeakBorder, PassBlurKernel * EffectSizeScale * FilterDownsampleFactor, PassBlurScale, SRTI_FilterColor1, SampleMaskMin, SampleMaskMaxA);
			}
		}

		// #1 now has the bloomed scene content
	}

	/**
	 * Selects the proper pixel shader and renders the full-res variation.
	 * @param View - current view.
	 * @param ColorGradingRHI - the color grading LUT.
	 */
	template<UINT DepthOfFieldFullResMode, UINT TemplTonemapperType, UBOOL bSeparateTranslucency, UBOOL bImageGrain, UBOOL bTemporalAA>
	void RenderVariationFullResTempl(FViewInfo& View, const FTextureRHIRef& ColorGradingRHI, const FTexture2DRHIRef* pToneMappingLuminanceTex = NULL, float Gamma = 2.2f)
	{
		UINT a = DepthOfFieldFullResMode;
		UINT b = TemplTonemapperType;
		UBOOL c = bSeparateTranslucency;
		UBOOL d = bImageGrain;
		UBOOL e = bTemporalAA;
		static FGlobalBoundShaderState UberBlendBoundShaderState;
		TShaderMapRef<FUberPostProcessVertexShader> BlendVertexShader(GetGlobalShaderMap());
		TShaderMapRef<TUberPostProcessBlendPixelShader<DepthOfFieldFullResMode,TemplTonemapperType,bImageGrain,bSeparateTranslucency,bTemporalAA> > BlendPixelShader(GetGlobalShaderMap());
		SetGlobalBoundShaderState(UberBlendBoundShaderState, GFilterVertexDeclaration.VertexDeclarationRHI, *BlendVertexShader, *BlendPixelShader, sizeof(FFilterVertex), 0, EGST_UberPostProcess);
		RenderVariationFullRes(View, ColorGradingRHI, *BlendVertexShader, *BlendPixelShader, pToneMappingLuminanceTex, Gamma);
	}

	/**
	 * Render the full-res variation.
	 * @param View - current view.
	 * @param ColorGradingRHI - the color grading LUT.
	 * @param BlendVertexShader - the vertex shader with which to render.
	 * @param BlendPixelShader - the pixel shader with which to render.
	 */
	void RenderVariationFullRes(
		FViewInfo& View,
		const FTextureRHIRef& ColorGradingRHI,
		FUberPostProcessVertexShader* BlendVertexShader,
		FUberPostProcessBlendPixelShaderBase* BlendPixelShader,
		const FTexture2DRHIRef* LuminanceTextureRHI = NULL,
		float Gamma = 2.2f)
	{
		// full resolution (this can be much bigger than the viewport needed size, half and quarter is computed from that size)
		const UINT BufferSizeX = GSceneRenderTargets.GetBufferSizeX();
		const UINT BufferSizeY = GSceneRenderTargets.GetBufferSizeY();
		// quarter resolution +2 pixel border
		const UINT FilterBufferSizeX = GSceneRenderTargets.GetFilterBufferSizeX();
		const UINT FilterBufferSizeY = GSceneRenderTargets.GetFilterBufferSizeY();
		const UINT FilterDownsampleFactor = GSceneRenderTargets.GetFilterDownsampleFactor();
		// half resolution
		const UINT HalfSizeX = BufferSizeX / 2;
		const UINT HalfSizeY = BufferSizeY / 2;
		// quarter resolution
		const UINT QuarterSizeX = BufferSizeX / FilterDownsampleFactor;
		const UINT QuarterSizeY = BufferSizeY / FilterDownsampleFactor;

		// The combined (uber) post-processing shader does depth of field, bloom, motion blur, material colorization and gamma correction
		// all together.
		//
		// It takes an HDR (64-bit) input and produces a low dynamic range (32-bit) output.  If it is the final post processing
		// shader in the post processing chain then it can render directly into the view's render target.  Otherwise it has to render into
		// a 32-bit render target.
		//
		// Note: Any post-processing shader that follows the uber shader needs to be able to handle an LDR input.  The shader can can check
		//       for this by looking at the bUseLDRSceneColor flag in the FViewInfo structure.  Also the final shader following the uber
		//       shader needs to write into the view's render target (or needs to write the result out to a 64-bit render-target).

		// Set the DOF and bloom parameters
		BlendPixelShader->DOFParameters.SetPS(BlendPixelShader, View.DepthOfFieldParams);
		BlendPixelShader->PostProcessParameters.SetPS(BlendPixelShader, BlurKernelSize, DOFOcclusionTweak);

		if (GWorld && GWorld->GetWorldInfo() && LuminanceTextureRHI)	// added by zhaojun@snda
		{
			SetPixelShaderValue(BlendPixelShader->GetPixelShader(), 
				BlendPixelShader->LuminanceThresholdParameter, GWorld->GetWorldInfo()->LuminanceThreshold);

			SetTextureParameterDirectly(
				BlendPixelShader->GetPixelShader(),
				BlendPixelShader->LuminanceTextureParameter,
				TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
				*LuminanceTextureRHI);
		}

		SetPixelShaderValue(BlendPixelShader->GetPixelShader(), BlendPixelShader->GammaCoefficientParameter, Gamma);

		// Set the color LUT texture parameter
		SetTextureParameterDirectly(
			BlendPixelShader->GetPixelShader(),
			BlendPixelShader->ColorGradingLUTTextureParameter,
			TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
			ColorGradingRHI);

		{
			// we clamp the half res lookup in the view rectangle to fix Matinee cinema border and split screen border artifacts

			// contract view by half a texel in half res (1 texel in full res)
			FLOAT Border = 1.0f; 
			FVector2D SampleMaskMin(View.RenderTargetX + Border, View.RenderTargetY + Border);
			FVector2D SampleMaskMax(View.RenderTargetX + View.RenderTargetSizeX - 1 - Border, View.RenderTargetY + View.RenderTargetSizeY - 1 - Border);

			FVector4 MaskRect(
				SampleMaskMin.X / BufferSizeX,
				SampleMaskMin.Y / BufferSizeY,
				SampleMaskMax.X / BufferSizeX,
				SampleMaskMax.Y / BufferSizeY);

			SetPixelShaderValues(
				BlendPixelShader->GetPixelShader(),
				BlendPixelShader->HalfResMaskRectParameter,
				&MaskRect,
				1);
		}

		// Setup the scene texture
		BlendPixelShader->SceneTextureParameters.Set(&View, BlendPixelShader, SF_Point);

		SetPixelShaderValue(
			BlendPixelShader->GetPixelShader(), 
			BlendPixelShader->BloomTintAndScreenBlendThresholdParameter, 
			// Bloom scale is applied after the blurring to avoid clamping
			FVector4(BloomTint.R * BloomScale, BloomTint.G * BloomScale, BloomTint.B * BloomScale, BloomScreenBlendThreshold));

#if WITH_MOBILE_RHI
		if (GUsingMobileRHI)
		{
//			SetTextureParameterDirectly(BlendPixelShader->GetPixelShader(), BlendPixelShader->LowResPostProcessBuffer, TStaticSamplerState<SF_Bilinear>::GetRHI(), GSceneRenderTargets.GetHalfResPostProcessTexture() );
		}
		else
#endif
		{
			SetTextureParameterDirectly(BlendPixelShader->GetPixelShader(), BlendPixelShader->LowResPostProcessBuffer, TStaticSamplerState<SF_Bilinear>::GetRHI(), GSceneRenderTargets.GetHalfResPostProcessTexture() );
		}

		// noise texture
		if(BlendPixelShader->NoiseTextureParameter.IsBound())
		{
			UTexture2D* NoiseTexture = GEngine->ImageGrainNoiseTexture;
			
			if(!NoiseTexture)
			{
				// in case there is no noise texture (ini setting points to not existing texture)
				SceneImageGrainScale = 0;
				NoiseTexture = GEngine->ScreenDoorNoiseTexture;
			}

			SetTextureParameter(
				BlendPixelShader->GetPixelShader(),
				BlendPixelShader->NoiseTextureParameter,
				TStaticSamplerState<SF_Point,AM_Wrap,AM_Wrap,AM_Wrap>::GetRHI(),
				NoiseTexture->Resource->TextureRHI );
		}

		// set ImageAdjustments1Parameter
		{
			const bool AnimateNoise = true;

			// chose to work well with the 64x64 texture and 
			// to give pleasant change over time
			const char RandomValues[] = 
			{
				8,59,
				43,48,
				20,63,
				43,19,
				6,26,
				8,46,
				29,31,
				30,63,
				44,34,
				62,36,
				49,63,
				36,56,
				35,30,
				41,63,
				10,54,
				58,4
			};

			static UINT NewRand[2] = {0, 0};

			if(AnimateNoise)
			{
				static UINT State = 0;

				State = (State + 1) % 16;

				// tweaked with the 6x64 texture to not have obvious movement in the noise
				NewRand[0] = RandomValues[State * 2 + 0];
				NewRand[1] = RandomValues[State * 2 + 1];
			}

			SetPixelShaderValue(
				BlendPixelShader->GetPixelShader(), 
				BlendPixelShader->ImageAdjustments1Parameter,
				FVector4(NewRand[0], NewRand[1], 0, SceneImageGrainScale));
		}

		// set ImageAdjustments2Parameter and ImageAdjustments3Parameter
		{
			// Adjusted by some constant slightly bigger than 1 to allow the values to reach 1
			// Max to avoid division by zero
			const FLOAT TonemapperScaleClamped = Max(0.000001f, TonemapperScale);

			// 0.22f is the value where the sigmoid curve has the slope of 1 where it meets the linear curve
			FLOAT A = 0.22f / TonemapperScaleClamped;
			FLOAT B = 1.0f;

			// adjust scale to get y=1 at x=TonemapperRange
			{
				// compute y for x=TonemapperRange
				FLOAT y = TonemapperAFunc(TonemapperRange, A, B);

				B /= y;
			}

			FLOAT LinearSteepness = 1.0f * TonemapperScaleClamped;

			FLOAT FunctionSplitPos = InverseTonemapperADerivedFunc(LinearSteepness, A, B);

			SetPixelShaderValue(
				BlendPixelShader->GetPixelShader(), 
				BlendPixelShader->ImageAdjustments2Parameter,
				FVector4(A, B, FunctionSplitPos, LinearSteepness));

			SetPixelShaderValue(
				BlendPixelShader->GetPixelShader(), 
				BlendPixelShader->ImageAdjustments3Parameter,
				FVector4(Clamp(TonemapperToeFactor, 0.0f, 1.0f), 0, 0, 0));
		}

		if(DepthOfFieldType == DOFType_BokehDOF)
		{
			SetTextureParameter(
				BlendPixelShader->GetPixelShader(),
				BlendPixelShader->BokehDOFLayerTextureParameter, 
				TStaticSamplerState<SF_Bilinear>::GetRHI(),
				GSceneRenderTargets.GetBokehDOFTexture() );
		}

		// Set the motion blur parameters.
		BlendPixelShader->MotionBlurShaderParameters.Set(BlendPixelShader->GetPixelShader(),View,MotionBlurParams);

		// We need to adjust the UV coordinate calculation for the scene color texture when rendering directly to
		// the view's render target (to support the editor).

		// without div by 8 adjustment
		UINT TargetSizeX = View.Family->RenderTarget->GetSizeX();
		UINT TargetSizeY = View.Family->RenderTarget->GetSizeY();
		FLOAT ScaleX = TargetSizeX / static_cast<FLOAT>(BufferSizeX);
		FLOAT ScaleY = TargetSizeY / static_cast<FLOAT>(BufferSizeY);

		// For split-screen and 4:3 views we also need to take into account the viewport correctly; however, the
		// DOFAndBloomBlendVertex shader computes the UV coordinates for the SceneColor texture directly from the
		// screen coordinates that are used to render and since the view-port may not be located at 0,0 we need
		// to adjust for that by modifying the UV offset and scale.

		// used to scale to half res, shift to view in left top corner
		SetVertexShaderValue(
			BlendVertexShader->GetVertexShader(),
			BlendVertexShader->SceneCoordinate2ScaleBiasParameter,
			FVector4(
			0.5f / HalfSizeX,
			0.5f / HalfSizeY,
			View.RenderTargetY / (FLOAT)BufferSizeY,
			View.RenderTargetX / (FLOAT)BufferSizeX)
			);

		// used to scale to quarter res (which is left top screen +1 border texel)
		{
			SetVertexShaderValue(
				BlendVertexShader->GetVertexShader(),
				BlendVertexShader->SceneCoordinate3ScaleBiasParameter,
				FVector4(
				0.25f / FilterBufferSizeX,
				0.25f / FilterBufferSizeY,
				1.0f / FilterBufferSizeY,
				1.0f / FilterBufferSizeX)
				);
		}

		if( FinalEffectInGroup
			&& View.Family->bResolveScene 
			&& !GSystemSettings.NeedsUpscale() )
		{	
			// compute pixel position from screen pos, unify D3D9 half texel shift (done like this for cleaner code), flip y
			SetVertexShaderValue(
				BlendVertexShader->GetVertexShader(),
				BlendVertexShader->SceneCoordinate1ScaleBiasParameter,
				FVector4(
				0.5f * TargetSizeX,
				-0.5f * TargetSizeY,
				0.5f * TargetSizeY + GPixelCenterOffset - View.Y,
				0.5f * TargetSizeX + GPixelCenterOffset - View.X));

			// Draw a quad mapping the blurred pixels in the filter buffer to the scene color buffer.
			DrawDenormalizedQuad(
				View.X, View.Y, View.SizeX, View.SizeY,
				View.RenderTargetX, View.RenderTargetY, View.RenderTargetSizeX, View.RenderTargetSizeY,
				TargetSizeX, TargetSizeY,
				BufferSizeX, BufferSizeY);
		}
		else
		{
			// compute pixel position from screen pos, unify D3D9 half texel shift (done like this for cleaner code), flip y
			SetVertexShaderValue(
				BlendVertexShader->GetVertexShader(),
				BlendVertexShader->SceneCoordinate1ScaleBiasParameter,
				FVector4(
				0.5f * BufferSizeX,
				-0.5f * BufferSizeY,
				0.5f * BufferSizeY + GPixelCenterOffset - View.RenderTargetY,
				0.5f * BufferSizeX + GPixelCenterOffset - View.RenderTargetX));

			// Draw a quad mapping the blurred pixels in the filter buffer to the scene color buffer.
			DrawDenormalizedQuad(
				View.RenderTargetX, View.RenderTargetY, View.RenderTargetSizeX,View.RenderTargetSizeY,
				View.RenderTargetX, View.RenderTargetY, View.RenderTargetSizeX, View.RenderTargetSizeY,
				BufferSizeX, BufferSizeY,
				BufferSizeX, BufferSizeY);

			FResolveRect ResolveRect;
			ResolveRect.X1 = View.RenderTargetX;
			ResolveRect.Y1 = View.RenderTargetY;
			ResolveRect.X2 = View.RenderTargetX + View.RenderTargetSizeX;
			ResolveRect.Y2 = View.RenderTargetY + View.RenderTargetSizeY;

			if( View.Family->bResolveScene )
			{
				// Resolve the scene color LDR buffer.
				GSceneRenderTargets.FinishRenderingSceneColorLDR(TRUE,ResolveRect);
			}
			else
			{
				// Resolve the scene color HDR buffer.
				GSceneRenderTargets.FinishRenderingSceneColor(TRUE);
			}
		}

		if( View.Family->bResolveScene )
		{
			// Indicate that from now on the scene color is in an LDR surface.
			View.bUseLDRSceneColor = TRUE; 
		}
	}

	UBOOL Render(const FScene* Scene, UINT InDepthPriorityGroup, FViewInfo& View, const FMatrix& CanvasTransform, FSceneColorLDRInfo& LDRInfo)
	{
		SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("UberPostProcessing"));

		check(SDPG_PostProcess == InDepthPriorityGroup);

		if(PostProcessAA.IsEnabled(View))
		{
			// is rendered after all other post processing
			PostProcessAA.SetDeferredObject();
		}

		// [ming@snda
		if( !GSystemSettings.GetAllowBloom() && !GSystemSettings.GetAllowDepthOfField() && !GSystemSettings.GetAllowMotionBlur() )
			return TRUE;
		// ming@snda]

		GSceneRenderTargets.ResolveFullResTransluceny();

		FLOAT EffectSizeScale = 1.0f;
		if(bScaleEffectsWithViewSize)
		{
			// we scale by with because FOV is defined horizontally
			// 1280x720 is the reference resolution
			EffectSizeScale = View.SizeX / 1280.0f;
		}
		
		//shanling@sndo
		if ( ( GIsEditor && bUseLuminanceClamp ) || ( !GIsEditor && GSystemSettings.bUseDOFLuminanceClamp ) ) 
			DownSampleSceneAndDepthForDOF<1>( View );
		else
			DownSampleSceneAndDepthForDOF<0>( View );

		DownSampleSceneAndDepth(View);

		

		check(FALSE==View.bUseLDRSceneColor);

		// blend final LUT (3D color lookup texture) for color grading
		FTextureRHIRef ColorGradingRHI = ColorGradingBlenderRTCopy.ResolveLUT(View, ColorTransform);

		// setup flags -------------------------------------------

		UBOOL bUseDoFBlurBuffer = FALSE;

#if XBOX
		if( View.bRenderedToDoFBlurBuffer && (View.Family->ShowFlags & SHOW_TranslucencyDoF) )
		{
			bUseDoFBlurBuffer = TRUE;
		}
#endif
		// Should we do MotionBlur for this view?
		UBOOL bLocalMotionBlur = bMotionBlur;

		// it depends on view show flags
		// Disable motion blur when not using realtime update, since it requires continuous frames.
		if(
			( ( View.Family->ShowFlags & SHOW_MotionBlur ) == 0 )
			|| ( !View.Family->bRealtimeUpdate )
			|| ( !View.Family->ShouldPostProcess() )
		)
		{
			bLocalMotionBlur = FALSE;
		}

		UBOOL bUseSoftEdgeMotionBlur = MotionBlurSoftEdgeKernelSize > 0;

		if(!bLocalMotionBlur)
		{
			// soft edge motion blur without motion blur is useless
			bUseSoftEdgeMotionBlur = FALSE;
		}

		EDOFType LocalDepthOfFieldType = DepthOfFieldType;

		// depending on view flags or if the radius is too small (optimization) we don't render depth of field
		if(
			( ( View.Family->ShowFlags & SHOW_DepthOfField ) == 0 )
			|| ( !View.Family->ShouldPostProcess() )
			|| ( BlurKernelSize < 0.1f )
		)
		{
			// switch Depth of Field off
			LocalDepthOfFieldType = DOFType_MAX;
		}

		// DepthOfFieldHalfResMode: 0:no DOF (e.g. ReferenceDOF in full res), 1:SimpleDOf, 2:ReferenceDOF in half res
		UINT DepthOfFieldHalfResMode = 0;
		{
			if(LocalDepthOfFieldType == DOFType_SimpleDOF)
			{
				DepthOfFieldHalfResMode = 1;
			}
			// if the showflag is on we override the DepthOfField mode
			if(
				( View.Family->ShowFlags & SHOW_VisualizeDOFLayers )
				&& ( View.Family->ShouldPostProcess() )
			)
			{
				DepthOfFieldHalfResMode = 2;
			}
		}

		checkSlow(IsInRenderingThread());
		UBOOL bSeparateTranslucency = GSystemSettings.bAllowSeparateTranslucency;

		// chose the right shader depending on flags -------------

		// half resolution pass -----------------

#if WITH_MOBILE_RHI
		if (GUsingMobileRHI)
		{
			// don't support DoF on mobile yet
		}
		else
#endif
		{
#define VARIATION(A, B, C, D) \
			if(A == bUseDoFBlurBuffer && B == bUseSoftEdgeMotionBlur && C == DepthOfFieldHalfResMode && D == bLocalMotionBlur) \
			{ \
				RenderVariationHalfRes<A, B, C, D>(View, LocalDepthOfFieldType, EffectSizeScale, bSeparateTranslucency); \
			} else
			//
			VARIATION(0,0,0,0) VARIATION(0,0,0,1) VARIATION(0,0,1,0) VARIATION(0,0,1,1)
			VARIATION(0,0,2,0) VARIATION(0,0,2,1)
			VARIATION(0,1,0,0) VARIATION(0,1,0,1) VARIATION(0,1,1,0) VARIATION(0,1,1,1)
			VARIATION(0,1,2,0) VARIATION(0,1,2,1)
			VARIATION(1,0,0,0) VARIATION(1,0,0,1) VARIATION(1,0,1,0) VARIATION(1,0,1,1)
			VARIATION(1,0,2,0) VARIATION(1,0,2,1)
			VARIATION(1,1,0,0) VARIATION(1,1,0,1) VARIATION(1,1,1,0) VARIATION(1,1,1,1)
			VARIATION(1,1,2,0) VARIATION(1,1,2,1) 
			// terminator, should never be reached
			check(0);
			//
#undef VARIATION
		}

		if (GSystemSettings.GetAllowBloom())
		{
			RenderBloom(View, EffectSizeScale, bSeparateTranslucency);
		}

		UINT ToneMapperTypeOriginal = TonemapperType;

		//if (!GIsEditor)	// hdr only in game, zhaojun@snda
		{
			//shanling@snda
			if ( GSystemSettings.GetRenderHDR() && TonemapperType == 0 /*&& GPixelFormats[PF_R32F].Supported*/ && ( !GIsEditor || bUseNewToneMaping )  )	// hdr force tonemapper type 1
			{
				TonemapperType = 1;	// use the tone-mapping zhaojun@snda
				// force close the hdr in cb
				//TonemapperType = 0;	// use the tone-mapping zhaojun@snda
			}
		}

		const FTexture2DRHIRef* pLuminanceTex = NULL;

		if (TonemapperType == 1)
		{
			pLuminanceTex = RenderLuminance(View, EffectSizeScale, bSeparateTranslucency);
		}

		float gamma = 2.2f;

		// if the scene wasn't meant to be resolved to LDR then continue rendering to HDR
		if( !View.Family->bResolveScene )			
		{
			// Using 64-bit (HDR) surface
			GSceneRenderTargets.BeginRenderingSceneColor();
		}
		else
		{
			// Normally, we're going to specify RTUsage_FullOverwrite and ping-pong the RT between the surface and its resolve-target.
			// But if we're drawing to secondary views (e.g. splitscreen), we must make sure that the final result is drawn to the same
			// memory as view0, so that it will contain the final fullscreen image after all postprocess.
			// To do this, we need to detect if there are an odd number of effects that draw to LDR (on the secondary view), since an
			// even number of ping-pongs will place the final image in the same buffer as view0.
			// On PS3 SceneColorLDR, LightAttenuation and BackBuffer refers to the same memory.
			DWORD UsageFlags = RTUsage_FullOverwrite;
			if ( LDRInfo.bAdjustPingPong && (LDRInfo.NumPingPongsRemaining & 1) )
			{
				UsageFlags |= RTUsage_DontSwapBuffer;
			}

			//if this is the final effect then render directly to the view's render target
			//unless an upscale is needed, in which case render to LDR scene color
			if (FinalEffectInGroup
				&& !GSystemSettings.NeedsUpscale())
			{
				// fix rt resolve bug
				// ming@snda
				if( View.Family->RenderTarget->GetRenderTargetSurface() != GSceneRenderTargets.GetSceneColorSurface() )
				{
					RHISetRenderTarget(View.Family->RenderTarget->GetRenderTargetSurface(),FSurfaceRHIRef());	
				}
				else
				{
					// Render to the final render target
					GSceneRenderTargets.BeginRenderingBackBuffer( UsageFlags );
				}
			}
			else
			{
				GSceneRenderTargets.BeginRenderingSceneColorLDR( UsageFlags );
			}
		}

		//

		UBOOL DepthOfFieldFullResMode = 0;
		
		if(LocalDepthOfFieldType == DOFType_ReferenceDOF)
		{
			DepthOfFieldFullResMode = 1;
		}
		else if(LocalDepthOfFieldType == DOFType_BokehDOF)
		{
			DepthOfFieldFullResMode = 2;
		}

		// allow console command to override
		UBOOL bLocalImageGrain = bEnableSceneEffect && SceneImageGrainScale > 0.0f;
		
		// Doing image grain for this view depends on show flags
		if(
			( ( View.Family->ShowFlags & SHOW_ImageGrain ) == 0 )
			|| ( !View.Family->ShouldPostProcess() )
		)
		{
			bLocalImageGrain = FALSE;
		}

		const UBOOL bTemporalAA = 
		   GSystemSettings.bAllowTemporalAA
		&& (View.Family->ShowFlags & SHOW_TemporalAA)
		&& View.RenderingOverrides.bAllowTemporalAA;

		// full resolution pass -----------------
#define VARIATION1(A)		VARIATION2(A,0)			VARIATION2(A,1)			VARIATION2(A,2)
#define VARIATION2(A,B)		VARIATION3(A,B,0)		VARIATION3(A,B,1)
#define VARIATION3(A,B,C)	VARIATION4(A,B,C,0)		VARIATION4(A,B,C,1)
#define VARIATION4(A,B,C,D)	VARIATION5(A,B,C,D,0)	VARIATION5(A,B,C,D,1)
#define VARIATION5(A, B, C, D, E) \
		if(A == DepthOfFieldFullResMode && B == TonemapperType && C == bSeparateTranslucency && D == bLocalImageGrain && E == bTemporalAA) \
		{ \
			RenderVariationFullResTempl<A, B, C, D, E>(View, ColorGradingRHI, pLuminanceTex, gamma); \
		} else
		VARIATION1(0) VARIATION1(1) VARIATION1(2)

		// terminator, should never be reached
		check(0);
		//
#undef VARIATION1
#undef VARIATION2
#undef VARIATION3
#undef VARIATION4
#undef VARIATION5

		TonemapperType = ToneMapperTypeOriginal;	//zhaojun@snda

		return TRUE;
	}


	/**
	 * Informs FSceneRenderer what to do during pre-pass.
	 * @param MotionBlurParameters	- The parameters for the motion blur effect are returned in this struct.
	 * @return Motion blur needs to have velocities written during pre-pass.
	 */
	virtual UBOOL RequiresVelocities( FMotionBlurParams& OutMotionBlurParams ) const
	{
		// when used for SceneCapture actor we get a call from the game thread
		// when used for normal post processing the call comes from the render thread

		if ( bMotionBlur )
		{
			OutMotionBlurParams = MotionBlurParams;

			const AWorldInfo* WorldInfo = GWorld->GetWorldInfo();

			OutMotionBlurParams.bPlayersOnly = WorldInfo->bPlayersOnly;
		}
		return bMotionBlur;
	}

	/**
	 * Tells FSceneRenderer whether to store the previous frame's transforms.
	 */
	virtual UBOOL RequiresPreviousTransforms(const FViewInfo& View) const
	{
		return bMotionBlur;
	}

	/**
	 * Whether the effect may potentially render to SceneColorLDR or not.
	 * @return TRUE if the effect may potentially render to SceneColorLDR, otherwise FALSE.
	 */
	virtual UBOOL MayRenderSceneColorLDR() const
	{
		return TRUE;
	}

	/**
	 * Whether the effect will setup a deferred post-process anti-aliasing effect.
	 */
	virtual UBOOL HasPostProcessAA(const FViewInfo& View) const
	{
		return PostProcessAA.IsEnabled(View);
	}

	void CheckForChanges( const FLUTBlender& PreviousLUTBlender )
	{
		ColorGradingBlenderRTCopy.CheckForChanges( PreviousLUTBlender );
	}

	const FLUTBlender& GetLUTBlender() const
	{
		return ColorGradingBlenderRTCopy;
	}
	//shanling@snda
	void SertDOFParameter( bool bUse, float Parameter )
	{
		bUseLuminanceClamp = bUse;
		LuminanceClampParameter = Parameter;
	}

	void SetNewTonemapingParameter( const FVector4 & Parameter )
	{
		NewTonemapingParameter = Parameter;
	}
	void SetUseNewToneMapingEnable( bool bUse )
	{
		bUseNewToneMaping = bUse;
	}

	void SetUseMatineeLuminanceParameter( bool bUse )
	{
		bUseMatineeLuminanceParameter = bUse;
	}

	template< UINT UseLuminanceClamp > 
	void DownSampleSceneAndDepthForDOF( FViewInfo& View )
	{
		SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("DownSampleSceneAndDepthForDOF"));

		const UINT BufferSizeX = GSceneRenderTargets.GetBufferSizeX();
		const UINT BufferSizeY = GSceneRenderTargets.GetBufferSizeY();

		// if this pointer is set, the downsampled z is put into the alpha channel
		const FSceneView* DownsampleZView = &View;

		if(GUsingMobileRHI)
		{
			// on mobile the rendertarget is ARGB8, the depth is not available
			// to z doesn't need to be downsampled
			DownsampleZView = 0;
		}


	FIntRect DstRect;
	FIntPoint DstLeftTop = FIntPoint(View.RenderTargetX / 2, View.RenderTargetY / 2);
	FIntRect  SrcRect    = FIntRect((INT)(View.RenderTargetX), (INT)(View.RenderTargetY), (INT)(View.RenderTargetX + View.RenderTargetSizeX), (INT)(View.RenderTargetY + View.RenderTargetSizeY));
	UINT      DownsampleFactor = 2;
	FIntPoint DstTextureSize = FIntPoint(BufferSizeX / 2, BufferSizeY / 2);
	FIntPoint SrcTextureSize = FIntPoint(BufferSizeX, BufferSizeY);
	// should we fill the destination, or only half the source size?
	DstRect = FIntRect( DstLeftTop, DstLeftTop + FIntPoint(SrcRect.Width() / DownsampleFactor, SrcRect.Height() / DownsampleFactor));
	

	{ // oscar@snda
		SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("No Depth Tests, No Backface Culling")); // oscar@snda
		// No depth tests, no backface culling.
		RHISetDepthState(TStaticDepthState<FALSE,CF_Always>::GetRHI());
		RHISetRasterizerState(TStaticRasterizerState<FM_Solid,CM_None>::GetRHI());
		RHISetBlendState(TStaticBlendState<>::GetRHI());
	} // oscar@snda
	

	FSamplerStateRHIParamRef SamplerState = TStaticSamplerState<SF_Point,AM_Clamp,AM_Clamp,AM_Clamp>::GetRHI();
	UINT SampleCount1D =  DownsampleFactor;

	// Setup the downsamples.
	FVector2D DownsampleOffsets[MAX_FILTER_SAMPLES];
	FLinearColor DownsampleWeights[MAX_FILTER_SAMPLES];
	
	FLOAT InvSrcSizeX = 1.0f / (FLOAT)SrcTextureSize.X;
	FLOAT InvSrcSizeY = 1.0f / (FLOAT)SrcTextureSize.Y;

	{
		FLOAT OffsetX = - 0.5f * InvSrcSizeX;
		FLOAT OffsetY = - 0.5f * InvSrcSizeY;
		for(UINT SampleY = 0; SampleY < SampleCount1D; ++SampleY)
		{
			for(UINT SampleX = 0; SampleX < SampleCount1D; ++SampleX)
			{
				const INT SampleIndex = SampleY * SampleCount1D + SampleX;
				DownsampleOffsets[SampleIndex] = FVector2D(
					(FLOAT)SampleX * InvSrcSizeX + OffsetX,
					(FLOAT)SampleY * InvSrcSizeY + OffsetY
					);
				DownsampleWeights[SampleIndex] = FLinearColor::White / Square(SampleCount1D);
			}
		}
	}

	// Downsample the scene color buffer.

	// Set the filter color surface as the render target
	GSceneRenderTargets.BeginRenderingDOFDownSample();
	RHISetViewport(0, 0, 0.0f, DstTextureSize.X, DstTextureSize.Y, 1.0f);

	// still needed otherwise colors can leak into the other splitscreen view
	// Also serves the same purpose as RHIDiscardRenderBuffer(), which is needed on mobile.
#if !PS3 && !NGP
#if WITH_MOBILE_RHI
	// On a tiled renderer, it's important for performance to clear a surface
	// after binding, if the intension is to overwrite all the pixels
	if (GMobileTiledRenderer)
#endif
	{
	RHIClear(TRUE,FLinearColor(0,0,0,0),FALSE,0,FALSE,0);
	}
#endif

	if(DownsampleZView)
	{
		SCOPED_DRAW_EVENT(Event)(DEC_SCENE_ITEMS,TEXT("Setup Vertex/Pixel Shader")); // oscar@snda
		// down sample 4:1 with scene (average color, depth in alpha and velocity bit)
		TShaderMapRef<FDofDownSampleShader<UseLuminanceClamp>> PixelShader(GetGlobalShaderMap());
		PixelShader->SetParameters( DownsampleZView, SamplerState, GSceneRenderTargets.GetEffectiveSceneColorTexture(), LuminanceClampParameter );

		if( SampleCount1D == 2 )
		{
			SCOPED_DRAW_EVENT(EventV)(DEC_SCENE_ITEMS,TEXT("Setup DownsampleOffsets")); // oscar@snda
			static FGlobalBoundShaderState BoundShaderState;
			TShaderMapRef<TFilterVertexShader<4> > VertexShader(GetGlobalShaderMap());
			VertexShader->SetParameters(DownsampleOffsets);
			SetGlobalBoundShaderState(BoundShaderState, GFilterVertexDeclaration.VertexDeclarationRHI, *VertexShader, *PixelShader, sizeof(FFilterVertex));
		}
		else
		{
			SCOPED_DRAW_EVENT(EventV)(DEC_SCENE_ITEMS,TEXT("Setup DownsampleOffsets")); // oscar@snda
			static FGlobalBoundShaderState BoundShaderState;
			TShaderMapRef<TFilterVertexShader<16> > VertexShader(GetGlobalShaderMap());
			VertexShader->SetParameters(DownsampleOffsets);
			SetGlobalBoundShaderState(BoundShaderState, GFilterVertexDeclaration.VertexDeclarationRHI, *VertexShader, *PixelShader, sizeof(FFilterVertex));
		}

		RHIReduceTextureCachePenalty(PixelShader->GetPixelShader());
	}
	else
	{
		//SetFilterShaders(SamplerState, GSceneRenderTargets.GetEffectiveSceneColorTexture(), DownsampleOffsets, DownsampleWeights, SampleCount1D * SampleCount1D);
	}

	DrawDenormalizedQuad(
		DstRect.Min.X, DstRect.Min.Y, DstRect.Width(), DstRect.Height(),
		SrcRect.Min.X, SrcRect.Min.Y, SrcRect.Width(), SrcRect.Height(),
		DstTextureSize.X, DstTextureSize.Y,
		SrcTextureSize.X, SrcTextureSize.Y
		);

	GSceneRenderTargets.FinishRenderingDOFDownSample();
	}
	//shanling@snda
protected:
	ColorTransformMaterialProperties ColorTransform;
	FMotionBlurParams MotionBlurParams;
	FLUTBlender ColorGradingBlenderRTCopy;
	FLOAT TonemapperScale;
	FLOAT SceneImageGrainScale;
	FLOAT MotionBlurSoftEdgeKernelSize;
	FLOAT TonemapperToeFactor;
	FLOAT TonemapperRange;
	UINT ColorGradingCVar;
	UINT TonemapperType;
	INT BloomType;
	INT BloomQuality;
	FLOAT BloomWeightSmall;
	FLOAT BloomWeightLarge; 
	FLOAT BloomKernelSizeSmall;
	FLOAT BloomKernelSizeMedium;
	FLOAT BloomKernelSizeLarge; 
	UBOOL bMotionBlur;
	UBOOL bScaleEffectsWithViewSize;
	UBOOL bEnableSceneEffect;
	FPostProcessAA PostProcessAA;

	//shanling@snda
	FVector4 LuminanceParameter;
	FVector4 NewTonemapingParameter;
	bool     bUseNewToneMaping;
	bool     bUseMatineeLuminanceParameter;
	bool     bUseLuminanceClamp;
	float    LuminanceClampParameter;
	//shenlinag@snda
	// to prevent bad colors content to leak in (in split screen or when not using the full render target)
	const static UINT AntiLeakBorder = 2;
};

/**
 * Creates a proxy to represent the render info for a post process effect
 * @param WorldSettings - The world's post process settings for the view.
 * @return The proxy object.
 */
FPostProcessSceneProxy* UUberPostProcessEffect::CreateSceneProxy(const FPostProcessSettings* WorldSettings)
{
	if ( GUsingMobileRHI && !GMobileAllowPostProcess )
	{
		return NULL;
	}

	UBOOL bLocalMotionBlur = FALSE;

	// Disable motion blur for tiled screenshots; the way motion blur is handled is
	// incompatible with tiled rendering and a single view.
	// Probably we could workaround with multiple views if it is necessary.
	extern UBOOL GIsTiledScreenshot;
	extern INT GGameScreenshotCounter;

	if ( (WorldSettings == NULL || WorldSettings->bEnableMotionBlur) && GSystemSettings.GetAllowMotionBlur() && !GIsTiledScreenshot && (GGameScreenshotCounter == 0) )
	{
		bLocalMotionBlur = TRUE;
	}

	UINT LocalTonemapperType = TonemapperType;

#if !FINAL_RELEASE
	{
		// console command can overwrite
		static IConsoleVariable* CVar = GConsoleManager->FindConsoleVariable(TEXT("TonemapperType")); 

		INT Value = CVar->GetInt();

		if(Value == 0
		|| Value == 1
		|| Value == 2)
		{
			LocalTonemapperType = Value;
		}
	}
#endif // !FINAL_RELEASE
	//shanling@snda
	FUberPostProcessSceneProxy *pProxy = new FUberPostProcessSceneProxy(this, WorldSettings, GColorGrading, LocalTonemapperType, bLocalMotionBlur, bEnableImageGrain);
	pProxy->SetNewTonemapingParameter( FVector4(NewLimitLuminance, NewMiddleLuminance, NewWhiteLuminance, NewAvgLuminanceParameter) );
	pProxy->SetUseNewToneMapingEnable( bUseNewToneMaping );
	pProxy->SetUseMatineeLuminanceParameter( bUseMatineeLuminanceParameter );
	pProxy->SertDOFParameter( bUseLuminanceClamp, LuminanceClampParameter );
	return pProxy;
}

// [ming@snda
/**
 * @param View - current view
 * @return TRUE if the effect should be rendered
 */
UBOOL UUberPostProcessEffect::IsShown(const FSceneView* View) const
{
	return (GSystemSettings.GetAllowBloom() || GSystemSettings.GetAllowDepthOfField() || GSystemSettings.GetAllowMotionBlur() || GSystemSettings.GetAllowPostProcessAA())
		&& UPostProcessEffect::IsShown( View );
}
// ming@snda]
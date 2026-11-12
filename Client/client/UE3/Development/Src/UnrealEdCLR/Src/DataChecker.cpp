#include "UnrealEdCLR.h"
#include "DataChecker.h"
#include "ShaderManager.h"


DataChecker::DataChecker()
{
	nMaxTriangleCount1		= 100;
	nMaxTextureWidth1		= 1024;
	nMaxTextureHeight1		= 1024;
	nMaxVertexShader1		= 50;
	nMaxBasepassShaderWithLightmap1	= 50;
	nMaxPointLightShader1			= 50;
	nMaxSamplerUsage1				= 4;

	nMaxTriangleCount2		= 100;
	nMaxTextureWidth2		= 1024;
	nMaxTextureHeight2		= 1024;
	nMaxVertexShader2		= 50;
	nMaxBasepassShaderWithLightmap2	= 50;
	nMaxPointLightShader2			= 50;
	nMaxSamplerUsage2				= 4;

	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxTriangleCount1"	), nMaxTriangleCount1, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxTextureWidth1"	), nMaxTextureWidth1, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxTextureHeight1"	), nMaxTextureHeight1, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxVertexShader1"	), nMaxVertexShader1, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxBasepassShaderWithLightmap1"	), nMaxBasepassShaderWithLightmap1, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxPointLightShader1"	), nMaxPointLightShader1, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxSamplerUsage1"	), nMaxSamplerUsage1, GEditorIni);

	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxTriangleCount2"	), nMaxTriangleCount2, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxTextureWidth2"	), nMaxTextureWidth2, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxTextureHeight2"	), nMaxTextureHeight2, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxVertexShader2"	), nMaxVertexShader2, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxBasepassShaderWithLightmap2"	), nMaxBasepassShaderWithLightmap2, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxPointLightShader2"	), nMaxPointLightShader2, GEditorIni);
	GConfig->GetInt(TEXT("UnrealEd.DataChecker"), TEXT("MaxSamplerUsage2"	), nMaxSamplerUsage2, GEditorIni);

	Init();
}

DataChecker::~DataChecker()
{

}

bool DataChecker::IsValid(UObject* DataAsset, FString &Error, int nLevel)
{
	bool checkPass = true;

	if(DataAsset->IsA(UStaticMesh::StaticClass()))
	{
		++nStaticMeshCount;
		UStaticMesh *StaticMesh = Cast<UStaticMesh>(DataAsset);
		if(StaticMesh)
		{
			FStaticMeshRenderData *CurRenderData = &(StaticMesh->GetSourceData());
			int nCount = CurRenderData->GetTriangleCount();

			FBoxSphereBounds Bounds = StaticMesh->Bounds;
			float V = (Bounds.GetBox().Max.X - Bounds.GetBox().Min.X)*(Bounds.GetBox().Max.Y - Bounds.GetBox().Min.Y)*(Bounds.GetBox().Max.Z -Bounds.GetBox().Min.Z);
			
			if( StaticMesh->LightMapCoordinateIndex > 0 && StaticMesh->LODModels.Num() > 0 )
			{
				const FStaticMeshRenderData& RenderData = StaticMesh->LODModels(0);
				if( StaticMesh->LightMapCoordinateIndex >= (INT)RenderData.VertexBuffer.GetNumTexCoords() )
				{
					GWarn->MapCheck_Add(MCTYPE_WARNING, StaticMesh,
						*Localize(TEXT("Lightmass"), TEXT("LightmassError_BadLightMapCoordinateIndex"), TEXT("UnrealEd")),
						TEXT(""));

					checkPass = false;
				}
			}

			//check uv
			if(StaticMesh->LODModels.Num() > 0)
			{
				FStaticMeshRenderData* RenderData = &StaticMesh->LODModels(0);
				//for(int LightMapCoordinateIndex = 0; LightMapCoordinateIndex < 2; ++LightMapCoordinateIndex)
				{
					INT LightMapCoordinateIndex = StaticMesh->LightMapCoordinateIndex;

					if( RenderData && ( ( UINT )LightMapCoordinateIndex < RenderData->VertexBuffer.GetNumTexCoords() ) )
					{
						bool UVOverBound = false;
						bool UVOverlapped = false;
						//draw triangles
						WORD* Indices = (WORD*)RenderData->IndexBuffer.Indices.GetData();
						UINT NumIndices = RenderData->IndexBuffer.Indices.Num();
						for (UINT i = 0; i < NumIndices - 2; i += 3)
						{
							FVector2D UV1( RenderData->VertexBuffer.GetVertexUV( Indices[ i + 0 ], LightMapCoordinateIndex ) ); 
							FVector2D UV2( RenderData->VertexBuffer.GetVertexUV( Indices[ i + 1 ], LightMapCoordinateIndex ) ); 
							FVector2D UV3( RenderData->VertexBuffer.GetVertexUV( Indices[ i + 2 ], LightMapCoordinateIndex ) ); 
							// Draw lines in black unless the UVs are outside of the 0.0 - 1.0 range.  For out-of-bounds
							// UVs, we'll draw the line segment in red
							FLinearColor UV12LineColor = FLinearColor::Black;
							if( UV1.X < -0.01f || UV1.X > 1.01f ||
								UV2.X < -0.01f || UV2.X > 1.01f ||
								UV1.Y < -0.01f || UV1.Y > 1.01f ||
								UV2.Y < -0.01f || UV2.Y > 1.01f )
							{
								//UV12LineColor = FLinearColor( 0.6f, 0.0f, 0.0f );
								UVOverBound = true;
								break;
							}
							FLinearColor UV23LineColor = FLinearColor::Black;
							if( UV3.X < -0.01f || UV3.X > 1.01f ||
								UV2.X < -0.01f || UV2.X > 1.01f ||
								UV3.Y < -0.01f || UV3.Y > 1.01f ||
								UV2.Y < -0.01f || UV2.Y > 1.01f )
							{
								//UV23LineColor = FLinearColor( 0.6f, 0.0f, 0.0f );
								UVOverBound = true;
								break;
							}
							FLinearColor UV31LineColor = FLinearColor::Black;
							if( UV3.X < -0.01f || UV3.X > 1.01f ||
								UV1.X < -0.01f || UV1.X > 1.01f ||
								UV3.Y < -0.01f || UV3.Y > 1.01f ||
								UV1.Y < -0.01f || UV1.Y > 1.01f )
							{
								//UV31LineColor = FLinearColor( 0.6f, 0.0f, 0.0f );
								UVOverBound = true;
								break;
							}

							extern FLOAT Side(const FVector2D& p, const FVector2D& q, const FVector2D& a, const FVector2D& b);
							extern INT Intersecting(const FVector2D& p0, const FVector2D& p1, const FVector2D& t0, const FVector2D& t1, const FVector2D& t2);

							if(LightMapCoordinateIndex != 0)
							{
								// check if has overlapped UV
								for (UINT j = 0; j < NumIndices - 2; j +=3 )
								{
									if(j != i)
									{
										FVector2D TUV1( RenderData->VertexBuffer.GetVertexUV( Indices[ j + 0 ], LightMapCoordinateIndex ) ); 
										FVector2D TUV2( RenderData->VertexBuffer.GetVertexUV( Indices[ j + 1 ], LightMapCoordinateIndex ) ); 
										FVector2D TUV3( RenderData->VertexBuffer.GetVertexUV( Indices[ j + 2 ], LightMapCoordinateIndex ) );

										INT intRes1 = Intersecting(UV1, UV2, TUV1, TUV2, TUV3);
										INT intRes2 = Intersecting(UV2, UV3, TUV1, TUV2, TUV3);
										INT intRes3 = Intersecting(UV3, UV1, TUV1, TUV2, TUV3);
										if((intRes1 == 3) || (intRes2 == 3) || (intRes3 == 3))
										{
											UVOverlapped = TRUE;
										}
										
										// oscar@snda there are cases of full overlap
										if(!UVOverlapped)
										{
											if((UV1 == TUV1) && (UV2 == TUV2) && (UV3 == TUV3))
											{
												UVOverlapped = TRUE;
											}
										}
										// oscar@snda
									}
								}
							}
						}

						if(UVOverBound)
						{
							Error = FString::Printf(TEXT("%s:%d"), *Localize(TEXT("Lightmass"), TEXT("LightmassError_ObjectWrappedUVs"), TEXT("UnrealEd")),
								LightMapCoordinateIndex);
							GWarn->MapCheck_Add(MCTYPE_WARNING, StaticMesh,	*Error,	TEXT(""));
							checkPass = false;
						}

						if(UVOverlapped)
						{	
							Error = FString::Printf(TEXT("%s:%d"), *Localize(TEXT("Lightmass"), TEXT("LightmassError_ObjectOverlappedUVs"), TEXT("UnrealEd")),
								LightMapCoordinateIndex);

							GWarn->MapCheck_Add(MCTYPE_WARNING, StaticMesh,	*Error,	TEXT(""));
							checkPass = false;
						}
					}
				}
			}

			// check collision mesh
			if(StaticMesh->BodySetup && StaticMesh->BodySetup->AggGeom.ConvexElems.Num() > 20)
			{
				Error = FString::Printf(TEXT("Collision Mesh Faces:%d"), StaticMesh->BodySetup->AggGeom.ConvexElems.Num());
				GWarn->MapCheck_Add(MCTYPE_WARNING, StaticMesh,	*Error,	TEXT(""));
				checkPass = false;
			}

			float fValue = nCount/V*1000;
			if(nLevel == LEVEL1)
			{
				if(fValue > nMaxTriangleCount1)
				{
					Error = FString::Printf(TEXT("TriangleCount:%d"), nCount);
					GWarn->MapCheck_Add(MCTYPE_WARNING, StaticMesh,	*Error,	TEXT(""));
					checkPass = false;
				}
			}
			else
			{
				if(fValue > nMaxTriangleCount2)
				{
					Error = FString::Printf(TEXT("TriangleCount:%d"), nCount);
					GWarn->MapCheck_Add(MCTYPE_WARNING, StaticMesh,	*Error,	TEXT(""));
					checkPass = false;
				}
			}

			if(!checkPass)
			{
				++nStaticMeshErrorCount;
			}
		}
	}
	else if(DataAsset->IsA(UMaterial::StaticClass()))
	{
		++nMaterialCount;
		UMaterial *Material = Cast<UMaterial>(DataAsset);
		if(Material)
		{
			int nShaderNum[3] = {0,0,0};
			FMaterialResource *Resource = Material->GetMaterialResource();
			const FMaterialShaderMap* MaterialShaderMap = Resource->GetShaderMap();

			if (MaterialShaderMap && MaterialShaderMap->IsCompilationFinalized())
			{
				const FMeshMaterialShaderMap* MeshShaderMap = MaterialShaderMap->GetMeshShaderMap(&FLocalVertexFactory::StaticType);
				if (MeshShaderMap)
				{
					//distortion requires an extra pass
					TArray<FString> ShaderTypeNames;
					new (ShaderTypeNames)  FString(TEXT("TBasePassVertexShaderFNoLightMapPolicyFNoDensityPolicy"));
					if(Material->bUsedWithStaticLighting)
					{
						new (ShaderTypeNames) FString(TEXT("TBasePassPixelShaderFDirectionalLightMapTexturePolicyNoSkyLight"));
						new (ShaderTypeNames) FString(TEXT("TLightPixelShaderFPointLightPolicyFNoStaticShadowingPolicy"));
					}

					for(int i = 0; i < ShaderTypeNames.Num(); ++i)
					{
						FShaderType* ShaderType = FindShaderTypeByName(*ShaderTypeNames(i));
						if(ShaderType)
						{
							const FShader* Shader = MeshShaderMap->GetShader(ShaderType);
							if (Shader)
							{
								//if the shader was found, add it to the output arrays
								nShaderNum[i] = Shader->GetNumInstructions();
							}
						}
					}
				}
			}

			const INT SamplersUsed = Resource->GetSamplerUsage();
			if(nLevel == LEVEL1)
			{
				if(nShaderNum[0] > nMaxVertexShader1)
				{
					Error = FString::Printf(TEXT("VertexShader:%d"), nShaderNum[0]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(nShaderNum[1] > nMaxBasepassShaderWithLightmap1)
				{
					Error = FString::Printf(TEXT("BasepassShaderWithLightmap:%d"), nShaderNum[1]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(nShaderNum[2] > nMaxPointLightShader1)
				{
					Error = FString::Printf(TEXT("PointLightShader:%d"), nShaderNum[2]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(SamplersUsed > nMaxSamplerUsage1)
				{
					Error = FString::Printf(TEXT("SamplerUsage:%d"), SamplersUsed);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}	

			}
			else
			{
				if(nShaderNum[0] > nMaxVertexShader2)
				{
					Error = FString::Printf(TEXT("VertexShader:%d"), nShaderNum[0]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(nShaderNum[1] > nMaxBasepassShaderWithLightmap2)
				{
					Error = FString::Printf(TEXT("BasepassShaderWithLightmap:%d"), nShaderNum[1]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(nShaderNum[2] > nMaxPointLightShader2)
				{
					Error = FString::Printf(TEXT("PointLightShader:%d"), nShaderNum[2]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(SamplersUsed > nMaxSamplerUsage2)
				{
					Error = FString::Printf(TEXT("SamplerUsage:%d"), SamplersUsed);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}	
			}

			if(!checkPass)
			{
				++nMaterialErrorCount;
			}
		}
	}
	else if(DataAsset->IsA(UMaterialInstance::StaticClass()))
	{
		++nMaterialInstanceCount;
		UMaterial *Material = Cast<UMaterial>(DataAsset);
		if(Material)
		{
			int nShaderNum[3] = {0,0,0};
			FMaterialResource *Resource = Material->GetMaterialResource();
			const FMaterialShaderMap* MaterialShaderMap = Resource->GetShaderMap();

			if (MaterialShaderMap && MaterialShaderMap->IsCompilationFinalized())
			{
				const FMeshMaterialShaderMap* MeshShaderMap = MaterialShaderMap->GetMeshShaderMap(&FLocalVertexFactory::StaticType);
				if (MeshShaderMap)
				{
					//distortion requires an extra pass
					TArray<FString> ShaderTypeNames;
					new (ShaderTypeNames)  FString(TEXT("TBasePassVertexShaderFNoLightMapPolicyFNoDensityPolicy"));
					if(Material->bUsedWithStaticLighting)
					{
						new (ShaderTypeNames) FString(TEXT("TBasePassPixelShaderFDirectionalLightMapTexturePolicyNoSkyLight"));
						new (ShaderTypeNames) FString(TEXT("TLightPixelShaderFPointLightPolicyFNoStaticShadowingPolicy"));
					}

					for(int i = 0; i < ShaderTypeNames.Num(); ++i)
					{
						FShaderType* ShaderType = FindShaderTypeByName(*ShaderTypeNames(i));
						if(ShaderType)
						{
							const FShader* Shader = MeshShaderMap->GetShader(ShaderType);
							if (Shader)
							{
								//if the shader was found, add it to the output arrays
								nShaderNum[i] = Shader->GetNumInstructions();
							}
						}
					}
				}
			}

			const INT SamplersUsed = Resource->GetSamplerUsage();
			if(nLevel == LEVEL1)
			{
				if(nShaderNum[0] > nMaxVertexShader1)
				{
					Error = FString::Printf(TEXT("VertexShader:%d"), nShaderNum[0]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(nShaderNum[1] > nMaxBasepassShaderWithLightmap1)
				{
					Error = FString::Printf(TEXT("BasepassShaderWithLightmap:%d"), nShaderNum[1]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(nShaderNum[2] > nMaxPointLightShader1)
				{
					Error = FString::Printf(TEXT("PointLightShader:%d"), nShaderNum[2]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(SamplersUsed > nMaxSamplerUsage1)
				{
					Error = FString::Printf(TEXT("SamplerUsage:%d"), SamplersUsed);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}	

			}
			else
			{
				if(nShaderNum[0] > nMaxVertexShader2)
				{
					Error = FString::Printf(TEXT("VertexShader:%d"), nShaderNum[0]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(nShaderNum[1] > nMaxBasepassShaderWithLightmap2)
				{
					Error = FString::Printf(TEXT("BasepassShaderWithLightmap:%d"), nShaderNum[1]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(nShaderNum[2] > nMaxPointLightShader2)
				{
					Error = FString::Printf(TEXT("PointLightShader:%d"), nShaderNum[2]);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(SamplersUsed > nMaxSamplerUsage2)
				{
					Error = FString::Printf(TEXT("SamplerUsage:%d"), SamplersUsed);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Material,	*Error,	TEXT(""));
					checkPass = false;
				}	
			}

			if(!checkPass)
			{
				++nMaterialInstanceErrorCount;
			}
		}
	}
	else if(DataAsset->IsA(UTexture2D::StaticClass()))
	{
		++nTextureCount;
		UTexture2D *Texture = Cast<UTexture2D>(DataAsset);
		if(Texture)
		{
			int nWidth = Texture->SizeX;
			int nHeight = Texture->SizeY;
			if(nLevel == LEVEL1)
			{
				if(nWidth > nMaxTextureWidth1)
				{
					Error = FString::Printf(TEXT("TextureWidth:%d"), nWidth);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Texture,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(nHeight > nMaxTextureHeight1)
				{
					Error = FString::Printf(TEXT("TextureHeight:%d"), nHeight);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Texture,	*Error,	TEXT(""));
					checkPass = false;
				}

			}
			else
			{
				if(nWidth > nMaxTextureWidth2)
				{
					Error = FString::Printf(TEXT("TextureWidth:%d"), nWidth);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Texture,	*Error,	TEXT(""));
					checkPass = false;
				}
				
				if(nHeight > nMaxTextureHeight2)
				{
					Error = FString::Printf(TEXT("TextureHeight:%d"), nHeight);
					GWarn->MapCheck_Add(MCTYPE_WARNING, Texture,	*Error,	TEXT(""));
					checkPass = false;
				}

			}

			if(!checkPass)
			{
				++nTextureErrorCount;
			}
		}
	}

	return checkPass;
}

void DataChecker::Init()
{
	nStaticMeshCount		= 0;
	nStaticMeshErrorCount	= 0;
	nMaterialCount			= 0;
	nMaterialErrorCount		= 0;
	nMaterialInstanceCount	= 0;
	nMaterialInstanceErrorCount	= 0;
	nTextureCount			= 0;
	nTextureErrorCount		= 0;
}

FString DataChecker::GetAllErrorMsg()
{
	FString AllError;
	if(nStaticMeshCount > 0)
	{
		if(nStaticMeshErrorCount > 0)
		{
			AllError += FString::Printf(TEXT("StaticMesh count: %d, non-compliant: %d, %d percent  \r\n\r\n"),
				nStaticMeshCount, nStaticMeshErrorCount, 100*nStaticMeshErrorCount/nStaticMeshCount);
		}
		else
		{
			AllError += FString::Printf(TEXT("StaticMesh count: %d, all compliant!\r\n\r\n"), nStaticMeshCount);
		}
	}

	if(nMaterialCount > 0)
	{
		if(nMaterialErrorCount > 0)
		{
			AllError += FString::Printf(TEXT("Material count: %d, non-compliant: %d, %d percent \r\n\r\n "),
				nMaterialCount, nMaterialErrorCount, 100*nMaterialErrorCount/nMaterialCount);
		}
		else
		{
			AllError += FString::Printf(TEXT("Material count: %d, all compliant!\r\n\r\n"), nMaterialCount);
		}
	}

	if(nMaterialInstanceCount > 0)
	{
		AllError += FString::Printf(TEXT("MaterialInstance count: %d, non-compliant: %d, %d percent of Materials \r\n\r\n "),
				nMaterialInstanceCount, nMaterialInstanceErrorCount, nMaterialCount > 0 ? 100*nMaterialInstanceCount/nMaterialCount : 100);		
	}

	if(nTextureCount > 0)
	{
		if(nTextureErrorCount > 0)
		{
			AllError += FString::Printf(TEXT("Texture count: %d, non-compliant: %d, %d percent \r\n\r\n"),
				nTextureCount, nTextureErrorCount, 100*nTextureErrorCount/nTextureCount);
		}
		else
		{
			AllError += FString::Printf(TEXT("Texture count: %d, all compliant!\r\n\r\n"), nTextureCount);
		}
	}
	
	return AllError;
}

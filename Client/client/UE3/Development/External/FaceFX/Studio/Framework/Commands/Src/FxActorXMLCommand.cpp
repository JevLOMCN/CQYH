//------------------------------------------------------------------------------
// A command for importing or exporting FxActor data in XML format.
//
// Owner: Doug Perkowski
//
// Copyright (c) 2002-2010 OC3 Entertainment, Inc.
//------------------------------------------------------------------------------
#include "stdwx.h"

#include "FxActorXMLCommand.h"
#include "FxUnrealSupport.h"

#ifdef __UNREAL__
     #include "XWindow.h"
#else
	#include "wx/filename.h"
#endif // __UNREAL__

//@todo This is included to get access to FxMsg() etc.  We might want to remove 
//      this later on if we actually implement the plug-in architecture.
#include "FxConsole.h"
#include "FxActor.h"
#include "FxSDK.h"
#include "FxCurrentTimeNode.h"
#include "FxMorphTargetNode.h"
#include "FxSessionProxy.h"
#include "FxAnimUserData.h"
#include "FxStudioSession.h"
#include "FxStudioApp.h"
#include "FxXMLSupport.h"

namespace OC3Ent
{
namespace Face
{


FX_IMPLEMENT_CLASS(FxActorXMLCommand, 0, FxCommand);

FxActorXMLCommand::FxActorXMLCommand()
{
	_isUndoable = FxFalse;
}

FxActorXMLCommand::~FxActorXMLCommand()
{
}

FxCommandSyntax FxActorXMLCommand::CreateSyntax( void )
{
	FxCommandSyntax newSyntax;
	newSyntax.AddArgDesc(FxCommandArgumentDesc("-f", "-file", CAT_String));
	newSyntax.AddArgDesc(FxCommandArgumentDesc("-i", "-import", CAT_Flag));
	return newSyntax;
}

// Forward declarations.
FxName _PhonemeToName( FxPhoneme phon );
FxPhoneme _NameToPhoneme( FxName name );
FxBool _WriteActorToXML (const FxString& xmlpath, FxActor* pActor);
FxBool _ReadActorFromXML (const FxString& xmlpath, FxActor* pActor);
TiXmlElement* _ExportNode(FxFaceGraphNode* pNode);
TiXmlElement* _ExportBone(const FxBone& bone);
FxBool _ImportBone( TiXmlNode* boneNode, FxBone& bone);
void _ConvertSpaceSeparatedString( FxString &string, FxArray<FxReal>& array);

FxCommandError FxActorXMLCommand::Execute( const FxCommandArgumentList& argList )
{

	FxSessionProxy::GetSelectedAnimGroup(_cachedAnimGroup);
	FxSessionProxy::GetSelectedAnim(_cachedAnim);
	FxSessionProxy::GetSelectedAnimCurves(_cachedCurves);
	FxSessionProxy::GetSelectedKeys(_cachedCurveIndices, _cachedKeyIndices);

	FxString filename;
	if( argList.GetArgument("-file", filename) )
	{
		FxActor* pActor = NULL;
		FxSessionProxy::GetActor(&pActor);
		if(!pActor)
		{
			FxSessionProxy::CreateActor();
			FxSessionProxy::GetActor(&pActor);
			_shouldFlushUndo = FxTrue;
		}

		FxBool bImport = argList.GetArgument("-import");
		if( bImport )
		{
			_shouldFlushUndo = FxTrue;	
			FxMsg("Loading %s...", filename.GetData());
			if( _ReadActorFromXML(filename, pActor) )
			{
				FxMsg("FxActorXML command complete: Imported <b>%s</b> to actor.", filename.GetData());
			}
			else
			{
				FxError("<b>ERROR:</b> could not import file!");
			}
			FxSessionProxy::ActorDataChanged();
		}
		else
		{
			if( _WriteActorToXML(filename, pActor) )
			{
				FxMsg("FxActorXML command complete: Exported actor to <b>%s</b>.", filename.GetData());
			}
			else
			{
				FxError("<b>ERROR:</b> could not export actor!");
			}
			FxSessionProxy::SetSelectedAnimGroup(_cachedAnimGroup);
			FxSessionProxy::SetSelectedAnim(_cachedAnim);
			FxSessionProxy::SetSelectedAnimCurves(_cachedCurves);
			FxSessionProxy::SetSelectedKeys(_cachedCurveIndices, _cachedKeyIndices);

		}
		return CE_Success;
	}
	else
	{
		FxError("<b>ERROR:</b> invalid arguments (must specify -file)!");
		return CE_Failure;
	}
}

FxName _PhonemeToName( FxPhoneme phon )
{
	return FxName(FxGetPhonemeInfo(phon).talkback.mb_str(wxConvLibc));
}
FxPhoneme _NameToPhoneme( FxName name )
{
	FxSize i = 0;
	for( i = 0; i < NUM_PHONS; ++i)
	{
		if( name ==  _PhonemeToName((FxPhoneme)i) )
		{
			break;
		}
	}
	return (FxPhoneme)i;
}

TiXmlElement* _ExportBone(const FxBone& bone)
{
	TiXmlElement *pBoneElement = new TiXmlElement( "bone" );
	pBoneElement->SetAttribute( "name", bone.GetNameAsCstr() );
	FxString transform;
	transform << bone.GetPos().x << " ";
	transform << bone.GetPos().y << " ";
	transform << bone.GetPos().z << " ";
	transform << bone.GetRot().w << " ";
	transform << bone.GetRot().x << " ";
	transform << bone.GetRot().y << " ";
	transform << bone.GetRot().z << " ";
	transform << bone.GetScale().x << " ";
	transform << bone.GetScale().y << " ";
	transform << bone.GetScale().z;
	TiXmlText text( transform.GetData());
	pBoneElement->InsertEndChild(text);
	return pBoneElement;
}


FxBool _ImportBone( TiXmlNode* boneNode, FxBone& bone)
{
	if(boneNode)
	{
		TiXmlElement* boneElement = boneNode->ToElement();
		if(boneElement)
		{
			FxString boneName(boneElement->Attribute("name"));
			if(boneName.Length() > 0)
			{
				bone.SetName(boneName.GetData());
				FxString transform(boneElement->GetText());
				FxArray<FxReal> trans;
				_ConvertSpaceSeparatedString(transform, trans);
				if(trans.Length() != 10)
				{
					FxWarn("Expected 10 values in for transform of bone <b>%s</b>.  Found %d. Could not import bone.", boneName.GetData(), (FxInt32)trans.Length());
				}
				else
				{
					FxVec3 pos = FxVec3(trans[0], trans[1], trans[2]);
					FxQuat quat = FxQuat(trans[3], trans[4], trans[5], trans[6]);
					FxVec3 scale = FxVec3(trans[7],trans[8], trans[9]);
					bone.SetPos(pos);
					bone.SetRot(quat);
					bone.SetScale(scale);
					return FxTrue;
				}
			}
		}
	}
	return FxFalse;
}
	
void _ConvertSpaceSeparatedString( FxString &string, FxArray<FxReal>& array)
{
	string.StripWhitespace();
	string += ' ';
	FxSize last_index = 0;
	FxSize new_index = string.FindFirst(' ', last_index);
	while(	new_index != FxInvalidIndex && 
			new_index > last_index			)
	{
		FxString value = string.Substring(last_index, new_index - last_index);
		array.PushBack((FxReal) FxAtof(value.GetData()));
		last_index = new_index + 1;
		new_index = string.FindFirst(' ', last_index);
	}
}
FxBool _WriteActorToXML (const FxString& xmlpath, FxActor* pActor)
{
#ifdef NO_SAVE_VERSION
	return FxFalse;
#endif
	if(pActor)
	{
		void* pVoidSession = NULL;
		FxStudioSession* pSession = NULL;
		FxSessionProxy::GetSession(&pVoidSession);
		if( pVoidSession )
		{
			pSession = reinterpret_cast<FxStudioSession*>(pVoidSession);
		}
		else
		{
			FxError("<b>ERROR:</b> Null Session!");
			return FxFalse;
		}

		TiXmlDocument xmlDoc;
		TiXmlElement *pActorElement = new TiXmlElement( "actor" );
		xmlDoc.LinkEndChild(pActorElement);

		pActorElement->SetAttribute( "name", pActor->GetNameAsCstr() );
		pActorElement->SetAttribute( "version",  (FxInt32)FxSDKGetVersion());
		pActorElement->SetAttribute( "path", pSession->GetActorFilePath().GetData() );
		
		TiXmlElement *pFaceGraphElement = NULL; 

		FxFaceGraph& faceGraph = pActor->GetDecompiledFaceGraph();
		if(pActor->GetMasterBoneList().GetNumRefBones() > 0)
		{
			pFaceGraphElement = new TiXmlElement( "face_graph" );
			pActorElement->LinkEndChild(pFaceGraphElement);
			TiXmlElement *pRefBonesElement = new TiXmlElement( "bones" );
			pFaceGraphElement->LinkEndChild(pRefBonesElement);
			for(FxSize i = 0; i < pActor->GetMasterBoneList().GetNumRefBones(); ++i)
			{
				TiXmlElement *pBoneElement = _ExportBone(pActor->GetMasterBoneList().GetRefBone(i));
				if(pBoneElement)
				{
					pRefBonesElement->LinkEndChild(pBoneElement);
				}
			}
		}
		if(faceGraph.GetNumNodes() > 0)
		{
			if(!pFaceGraphElement)
			{
				pFaceGraphElement = new TiXmlElement( "face_graph" );
				pActorElement->LinkEndChild(pFaceGraphElement);
			}

			TiXmlElement *pFaceGraphNodesElement = new TiXmlElement( "nodes" );
			pFaceGraphElement->LinkEndChild(pFaceGraphNodesElement);


			const FxCompiledFaceGraph& cFaceGraph = pActor->GetCompiledFaceGraph();
			for(FxSize i = 0; i < cFaceGraph.nodes.Length(); ++i)
			{
				FxCompiledFaceGraphNode cNode = cFaceGraph.nodes[i];
				TiXmlElement *pFaceGraphNodeElement =  new TiXmlElement( "node" );

				pFaceGraphNodeElement->SetAttribute("type", FxNodeTypeLookupTable[cNode.nodeType].nodeClass);
				pFaceGraphNodesElement->LinkEndChild(pFaceGraphNodeElement);

				if(  cNode.nodeType == NT_BonePose )
				{
					FxBonePoseNode* pBonePoseNode = FxCast<FxBonePoseNode>(faceGraph.FindNode(cNode.name));
					if(pBonePoseNode)
					{
						TiXmlElement *pBonesElement = new TiXmlElement( "bones" );
						pFaceGraphNodeElement->LinkEndChild(pBonesElement);
						for(FxSize i = 0; i < pBonePoseNode->GetNumBones(); ++i)
						{
							TiXmlElement *pBoneElement = _ExportBone(pBonePoseNode->GetBone(i));
							if(pBoneElement)
							{
								pBonesElement->LinkEndChild(pBoneElement);
							}
						}
					}
				}

				pFaceGraphNodeElement->SetAttribute("name", cNode.name.GetAsCstr());
				pFaceGraphNodeElement->SetDoubleAttribute("min", cNode.nodeMin);
				pFaceGraphNodeElement->SetDoubleAttribute("max", cNode.nodeMax);
				FxInputOp op = cNode.inputOperation;
				switch(op)
				{
					case IO_Sum:
						pFaceGraphNodeElement->SetAttribute("input_op", "sum");
						break;
					case IO_Multiply:
						pFaceGraphNodeElement->SetAttribute("input_op", "multiply");
						break;
					case IO_Max:
						pFaceGraphNodeElement->SetAttribute("input_op", "max");
						break;
					case IO_Min:
						pFaceGraphNodeElement->SetAttribute("input_op", "min");
						break;
					case IO_Invalid:
						break;
				}

				if(cNode.userProperties.Length() > 0)
				{
					TiXmlElement *pNodePropertiesElement = new TiXmlElement( "properties" );
					pFaceGraphNodeElement->LinkEndChild(pNodePropertiesElement);
					for(FxSize i = 0; i < cNode.userProperties.Length(); ++i)
					{
						TiXmlElement *pNodePropertyElement = new TiXmlElement( "property" );
						pNodePropertyElement->SetAttribute("name", cNode.userProperties[i].GetNameAsCstr());

						FxFaceGraphNodeUserPropertyType type = 	cNode.userProperties[i].GetPropertyType();
						pNodePropertyElement->SetAttribute("property_enum",  (FxInt32)type);

						switch (type)
						{
							case UPT_Integer:
								pNodePropertyElement->SetAttribute("value",  cNode.userProperties[i].GetIntegerProperty());
								break;
							case UPT_Bool:
								pNodePropertyElement->SetAttribute("value",  cNode.userProperties[i].GetBoolProperty());
								break;
							case UPT_Real:
								pNodePropertyElement->SetDoubleAttribute("value",  cNode.userProperties[i].GetRealProperty());
								break;
							case UPT_String:
								pNodePropertyElement->SetAttribute("value",  cNode.userProperties[i].GetStringProperty().GetData());
								break;
							case UPT_Choice:
								pNodePropertyElement->SetAttribute("value",  cNode.userProperties[i].GetChoiceProperty().GetData());
								break;
						}
						pNodePropertiesElement->LinkEndChild(pNodePropertyElement);
					}
				}
			}

			// Go through again for links
			TiXmlElement *pFaceGraphLinksElement = NULL;
			FxFaceGraph::Iterator curr = faceGraph.Begin(FxFaceGraphNode::StaticClass());
			FxFaceGraph::Iterator end  = faceGraph.End(FxFaceGraphNode::StaticClass());
			while( curr != end )
			{
				FxFaceGraphNode* pNode = curr.GetNode();
				if(pNode)
				{
					for(FxSize i = 0; i < pNode->GetNumInputLinks(); ++i)
					{
						if( NULL == pFaceGraphLinksElement )
						{
							pFaceGraphLinksElement = new TiXmlElement( "links" );
							pFaceGraphElement->LinkEndChild(pFaceGraphLinksElement);
						}
						FxFaceGraphNodeLink link = pNode->GetInputLink(i);
						TiXmlElement *pLinkElement = new TiXmlElement( "link" );

						pLinkElement->SetAttribute("function", link.GetLinkFnName().GetAsString().GetData());
						pLinkElement->SetAttribute("from", link.GetNode()->GetNameAsCstr());
						pLinkElement->SetAttribute("to", pNode->GetNameAsCstr());
						
						pFaceGraphLinksElement->LinkEndChild(pLinkElement);

						TiXmlElement *pLinkParametersElement = new TiXmlElement( "parameters" );
						pLinkParametersElement->SetAttribute("num_params", (FxInt32)link.GetLinkFnParams().parameters.Length());
						FxString linkParams;
						
						for(FxSize j = 0; j < link.GetLinkFnParams().parameters.Length(); ++j)
						{
							linkParams << link.GetLinkFnParams().parameters[j] << " ";				
						}
						TiXmlText text( linkParams.GetData());
						pLinkParametersElement->InsertEndChild(text);
						pLinkElement->LinkEndChild(pLinkParametersElement);
					}
				}
				++curr;
			}
		}

		TiXmlElement *pMappingElement = new TiXmlElement( "mapping" );
		pActorElement->LinkEndChild(pMappingElement);
		FxPhonemeMap mapping;
		FxSessionProxy::GetPhonemeMap(mapping);

		for(FxSize i = 0; i < mapping.GetNumMappingEntries(); ++i)
		{
			FxPhonToNameMap entry = mapping.GetMappingEntry(i);
			
			TiXmlElement *pEntryElement = new TiXmlElement( "entry" );
			FxName phonemeName = _PhonemeToName(entry.phoneme);
			pEntryElement->SetAttribute( "phoneme",  phonemeName.GetAsCstr());
			pEntryElement->SetAttribute( "target",  entry.name.GetAsCstr());
			pEntryElement->SetDoubleAttribute( "amount",  entry.amount );
			pMappingElement->LinkEndChild(pEntryElement);
		}

		TiXmlElement *pAnimGroupsElement = new TiXmlElement( "animation_groups" );
		pActorElement->LinkEndChild(pAnimGroupsElement);
		for(FxSize i = 0; i < pActor->GetNumAnimGroups(); ++i)
		{
			FxAnimGroup group = pActor->GetAnimGroup(i);
			TiXmlElement *pAnimGroupElement = new TiXmlElement( "animation_group" );
			pAnimGroupElement->SetAttribute( "name", group.GetNameAsCstr() );
			pAnimGroupsElement->LinkEndChild(pAnimGroupElement);
			FxSessionProxy::SetSelectedAnimGroup(group.GetNameAsString());
			for(FxSize j = 0; j < group.GetNumAnims(); ++j)
			{
				FxName animName = group.GetAnim(j).GetName();
				// Select the animation so that it's user data is loaded.
				FxSessionProxy::SetSelectedAnim(animName.GetAsString());

				FxAnim* pAnim = NULL;
				FxSessionProxy::GetAnim(group.GetNameAsString(), animName.GetAsString(), &pAnim);
				FxAssert(pAnim);
				
				TiXmlElement *pAnimationElement = new TiXmlElement( "animation" );
				pAnimationElement->SetAttribute( "name", pAnim->GetNameAsCstr() );
			
				FxAnimUserData* pUserData = NULL;
				if( pAnim->GetUserData())
				{
					pUserData = reinterpret_cast<FxAnimUserData*>(pAnim->GetUserData());

					pAnimationElement->SetAttribute( "language", pUserData->GetLanguage().GetData() );
					pAnimationElement->SetAttribute( "coarticulation_config", pUserData->GetCoarticulationConfig().GetData() );
					pAnimationElement->SetAttribute( "gesture_config", pUserData->GetGestureConfig().GetData() );

					if(!pUserData->ShouldContainAnalysisResults())
					{
						pAnimationElement->SetAttribute( "analysis_results", "off" );
					}
					if(!pUserData->ShouldContainSpeechGestures())
					{
						pAnimationElement->SetAttribute( "speech_gestures", "off" );
					}

					// UE3 specific Stuff
					if(pAnim->GetSoundCuePath().Length()> 0)
					{
						pAnimationElement->SetAttribute( "audio_path", pAnim->GetSoundCuePath().GetData() );
					}
					else
					{
						wxString audioPath(pUserData->GetAudioPath().GetData(), wxConvLibc);                                                        
						wxFileName audioFilePath(audioPath);
						if( audioFilePath.FileExists() )
						{
							pAnimationElement->SetAttribute( "audio_path_full", audioFilePath.GetFullPath().mb_str(wxConvUTF8) );
							audioFilePath.MakeRelativeTo();
							pAnimationElement->SetAttribute( "audio_path", audioFilePath.GetFullPath().mb_str(wxConvUTF8) );

						}	
						else 
						{
							// Try finding the file relative to the actor path
							wxString wxActorPath(pSession->GetActorFilePath().GetData(), wxConvLibc );    
							wxFileName wxFileNameActor(wxActorPath);
							wxFileName relativeToActorPath = wxFileNameActor.GetPath() + wxFileName::GetPathSeparator() + audioPath;
							if(relativeToActorPath.FileExists())
							{
								pAnimationElement->SetAttribute( "audio_path_full", relativeToActorPath.GetFullPath().mb_str(wxConvUTF8) );
								relativeToActorPath.MakeRelativeTo();
								pAnimationElement->SetAttribute( "audio_path", relativeToActorPath.GetFullPath().mb_str(wxConvUTF8) );
							}
							else
							{
								// Might as well writ out what we have, even if it doesn't point to a file.
								pAnimationElement->SetAttribute( "audio_path", audioPath.mb_str(wxConvUTF8) );
							}
						}
					}
					if(pAnim->GetSoundNodeWave().Length() > 0)
					{
						pAnimationElement->SetAttribute( "audio_name", pAnim->GetSoundNodeWave().GetData() );
					}
					if(pAnim->GetSoundCueIndex() != -1)
					{
						pAnimationElement->SetAttribute( "audio_index", (FxInt32)pAnim->GetSoundCueIndex() );
					}

					FxPhonWordList *pPhonWordList = pUserData->GetPhonemeWordListPointer();
					if(pPhonWordList)
					{
						FxSize numPhons = pPhonWordList->GetNumPhonemes();

						TiXmlElement *pPhonemesElement = new TiXmlElement( "phonemes" );
						pAnimationElement->LinkEndChild(pPhonemesElement);

						for( FxSize k = 0; k < numPhons; ++k )
						{
							TiXmlElement *pPhonemeElement = new TiXmlElement( "phoneme" );
							FxName phonemeName = _PhonemeToName((FxPhoneme)pPhonWordList->GetPhonemeEnum(k));				
							pPhonemeElement->SetAttribute( "phoneme",  phonemeName.GetAsCstr());
							pPhonemeElement->SetDoubleAttribute( "start",  pPhonWordList->GetPhonemeStartTime(k));
							pPhonemeElement->SetDoubleAttribute( "end",  pPhonWordList->GetPhonemeEndTime(k));
							pPhonemesElement->LinkEndChild(pPhonemeElement);							
						}

						FxSize numWords = pPhonWordList->GetNumWords();
						TiXmlElement *pWordsElement = new TiXmlElement( "words" );
						pAnimationElement->LinkEndChild(pWordsElement);
						for( FxSize k = 0; k < numWords; ++k )
						{
							TiXmlElement *pWordElement = new TiXmlElement( "word" );
							pWordElement->SetDoubleAttribute( "start",  pPhonWordList->GetWordStartTime(k));
							pWordElement->SetDoubleAttribute( "end",  pPhonWordList->GetWordEndTime(k));
							pWordsElement->LinkEndChild(pWordElement);
							wxString str = pPhonWordList->GetWordText(k);
							TiXmlText text( str.mb_str(wxConvUTF8));
							pWordElement->InsertEndChild(text);
						}
					}
					TiXmlElement *pAnalysisTextElement = new TiXmlElement( "analysis_text" );
					pAnimationElement->LinkEndChild(pAnalysisTextElement);
					// Using wxString to convert from wchar to UTF8
					wxString str = pUserData->GetAnalysisText().GetData();
					TiXmlText text( str.mb_str(wxConvUTF8));
					pAnalysisTextElement->InsertEndChild(text);				
				}
				
				pAnimGroupElement->LinkEndChild(pAnimationElement);

				TiXmlElement *pCurvesElement = new TiXmlElement( "curves" );
				pAnimationElement->LinkEndChild(pCurvesElement);

				for(FxSize k = 0; k < pAnim->GetNumAnimCurves(); k++)
				{
					FxAnimCurve curve = pAnim->GetAnimCurve(k);
					TiXmlElement *pCurveElement = new TiXmlElement( "curve" );
					pCurveElement->SetAttribute( "name", curve.GetNameAsCstr() );
					pCurveElement->SetAttribute( "num_keys", curve.GetNumKeys() );
					if(pUserData)
					{
						if( pUserData->IsCurveOwnedByAnalysis(curve.GetName()) )
						{
							pCurveElement->SetAttribute( "owner", "analysis" );
						}
						else
						{
							pCurveElement->SetAttribute( "owner", "user" );
						}
					}
					pCurvesElement->LinkEndChild(pCurveElement);

					if(curve.GetNumKeys() > 0)
					{
						FxString keyData;
				
						for(FxSize l = 0; l < curve.GetNumKeys(); l++)
						{
							FxAnimKey key = curve.GetKey(l);
							keyData << key.GetTime()	<< " ";
							keyData << key.GetValue()	<< " ";
							keyData << key.GetSlopeIn() << " ";
							keyData << key.GetSlopeOut()<< " ";
						}
						TiXmlText text( keyData.GetData());
						pCurveElement->InsertEndChild(text);
					}
				}
			}
		}
		if(xmlDoc.SaveFile( xmlpath.GetData()))
		{
			return FxTrue;
		}
	}
	return FxFalse;
}

FxBool _ReadActorFromXML (const FxString& xmlpath, FxActor* pActor)
{
	if(pActor)
	{
		void* pVoidSession = NULL;
		FxStudioSession* pSession = NULL;
		FxSessionProxy::GetSession(&pVoidSession);
		if( pVoidSession )
		{
			pSession = reinterpret_cast<FxStudioSession*>(pVoidSession);
		}
		else
		{
			FxError("<b>ERROR:</b> Null Session!");
			return FxFalse;
		}
		TiXmlDocument doc( xmlpath.GetData() );
		if ( doc.LoadFile() )
		{
			//TiXmlNode* node = 0;
			TiXmlNode* actorNode = 0;
			TiXmlElement* actorElement = 0;
			TiXmlNode* mappingNode = 0;
			TiXmlElement* entryElement = 0;
			TiXmlNode* entryNode = 0;
			TiXmlNode* animationGroupsNode = 0;
			TiXmlNode* animationGroupNode = 0;
			TiXmlElement* animationGroupElement = 0;
			TiXmlElement* animationElement = 0;
			TiXmlNode* animNode = 0;
			TiXmlNode* phonemesNode = 0;
			TiXmlNode* phonemeNode = 0;
			TiXmlElement* phonemeElement =0;
			TiXmlNode* wordsNode = 0;
			TiXmlNode* wordNode = 0;
			TiXmlElement* wordElement =0;
			TiXmlNode* analysisTextNode = 0;
			TiXmlElement* analysisTextElement = 0;
			TiXmlNode* curvesNode = 0;
			TiXmlNode* curveNode = 0;
			TiXmlElement* curveElement = 0;
			
			//Face Graph
			TiXmlNode* faceGraphNode = 0;
			TiXmlNode* refBonesNode = 0;
			TiXmlNode* nodesNode = 0;
			TiXmlNode* nodeNode = 0;
			TiXmlElement* nodeElement = 0;
			TiXmlNode* propertiesNode = 0;
			TiXmlNode* propertyNode = 0;
			TiXmlElement* propertyElement = 0;
			TiXmlNode* linksNode = 0;
			TiXmlNode* linkNode = 0;
			TiXmlElement* linkElement = 0;
			TiXmlElement* parametersElement = 0;
			TiXmlNode* parametersNode = 0;
			TiXmlNode* bonesNode = 0;
			TiXmlNode* boneNode = 0;

			actorNode = doc.FirstChild( "actor" );
			if( actorNode )
			{
				actorElement = actorNode->ToElement();
				FxString actorName(actorElement->Attribute( "name" ));
				if( actorName.Length() > 0 )
				{
					pActor->SetName(actorName.GetData());
				}
			
				FxString actorPath( actorElement->Attribute("path"));


				faceGraphNode = actorNode->FirstChild( "face_graph" );
				if(faceGraphNode)
				{
					FxFaceGraph faceGraph = pActor->GetDecompiledFaceGraph();
					if(faceGraph.GetNumNodes() > 0)
					{
						FxWarn("Ignoring face_graph section due to existing nodes.  Use templates to merge face graphs.");
					}
					else
					{
						refBonesNode = faceGraphNode->FirstChild("bones");
						if(refBonesNode)
						{
							FxMsg("Importing reference bones.");
							boneNode = refBonesNode->IterateChildren( "bone",  0 );
							// If we are trashing a valid refpose, warn the user.  But there
							// is no need to trash the MBL if there are no refbones in the XML file.
							if( boneNode && pActor->GetMasterBoneList().GetNumRefBones() > 0 ) 
							{
								pActor->GetMasterBoneList().Clear();
								FxWarn("Overwriting the actor's existing reference pose.  The XML file contains reference bones.");
							}
							while( boneNode )
							{
								FxBone bone;
								if(_ImportBone(boneNode, bone))
								{
									pActor->GetMasterBoneList().AddRefBone(bone);
								}
								boneNode = refBonesNode->IterateChildren( "bone",  boneNode );
							}
							pActor->SetShouldClientRelink(FxTrue);
						}
						nodesNode = faceGraphNode->FirstChild("nodes");
						if(nodesNode)
						{
							nodeNode = nodesNode->IterateChildren( "node",  0 );
							while( nodeNode )
							{
								nodeElement = nodeNode->ToElement();
								if(nodeElement)
								{
									FxString nodeName(nodeElement->Attribute("name"));
									if(nodeName.Length() > 0)
									{
										FxString nodeType(nodeElement->Attribute("type"));
										nodeType = nodeType.ToLower();
										FxFaceGraphNode* pNode = NULL;

										if( nodeType == FxString(FxNodeTypeLookupTable[NT_BonePose].nodeClass).ToLower())
										{
											FxMsg("Importing bone pose <b>%s</b>.", nodeName.GetData());

											FxBonePoseNode* pBonePoseNode = new FxBonePoseNode();
											bonesNode = nodeNode->FirstChild( "bones" );
											if(bonesNode)
											{
												boneNode =  bonesNode->FirstChild( "bone" );
												while(boneNode)
												{
													FxBone bone;
													if(_ImportBone(boneNode, bone))
													{
														pBonePoseNode->AddBone(bone);
													}
													boneNode = bonesNode->IterateChildren( "bone",  boneNode );
												}
												pNode = FxCast<FxFaceGraphNode>(pBonePoseNode);
											}
										}
										else if( nodeType == FxString(FxNodeTypeLookupTable[NT_Combiner].nodeClass).ToLower())
										{
											pNode = new FxCombinerNode();
										}
										else if( nodeType  == FxString(FxNodeTypeLookupTable[NT_Delta].nodeClass).ToLower())
										{
											pNode = new FxDeltaNode();
										}
										else if( nodeType  == FxString(FxNodeTypeLookupTable[NT_CurrentTime].nodeClass).ToLower())
										{
											pNode = new FxCurrentTimeNode();
										}
										else if( nodeType  == FxString(FxNodeTypeLookupTable[NT_MaterialParameterUE3].nodeClass).ToLower())
										{
											pNode = new FUnrealFaceFXMaterialParameterNode();
										}
										else if( nodeType == FxString(FxNodeTypeLookupTable[NT_MorphTargetUE3].nodeClass).ToLower())
										{
											pNode = new FUnrealFaceFXMorphNode();
										}
										else if( nodeType  == FxString(FxNodeTypeLookupTable[NT_MorphTarget].nodeClass).ToLower())
										{
											pNode = new FxMorphTargetNode();
										}
										else
										{
											FxWarn("Unrecognized node type for node <b>%s</b>.  Setting as FxCombiner.", nodeName.GetData());
											pNode = new FxCombinerNode();
										}
										if( pNode )
										{
											pNode->SetName(nodeName.GetData());
											FxDReal minimum = 0.0f;
											FxDReal maximum = 1.0f; 
											if ( TIXML_WRONG_TYPE == nodeElement->QueryDoubleAttribute("min", &minimum) )
											{
												FxWarn("Invalid min parameter for node <b>%s</b>. Using Default.", nodeName.GetData());
											}
											if ( TIXML_WRONG_TYPE == nodeElement->QueryDoubleAttribute("max", &maximum)  )													
											{
												FxWarn("Invalid max parameter for node <b>%s</b>. Using Default.", nodeName.GetData());
											}
											pNode->SetMin((FxReal)minimum);
											pNode->SetMax((FxReal)maximum);
											if(pNode->GetMax() < pNode->GetMin() )
											{
												pNode->SetMin(pNode->GetMax());
												FxWarn("Min greater than max for node <b>%s</b>.  Setting Min equal to Max.", nodeName.GetData());
											}
											FxString inputOp(nodeElement->Attribute("input_op"));
											inputOp = inputOp.ToLower();
											if( inputOp == FxString("multiply") )
											{
												pNode->SetNodeOperation(IO_Multiply);
											}
											else if( inputOp == FxString("max") )
											{
												pNode->SetNodeOperation(IO_Max);
											}
											else if( inputOp == FxString("min") )
											{
												pNode->SetNodeOperation(IO_Min);
											}	
											else if( inputOp == FxString("sum") ||
													 inputOp == FxString("")		)
											{
												pNode->SetNodeOperation(IO_Sum);
											}
											else
											{
												FxWarn("Unrecognized input operation for node <b>%s</b>.  Using sum.", nodeName.GetData());
												pNode->SetNodeOperation(IO_Sum);
											}

											if(pNode->GetNumUserProperties() > 0)
											{
												propertiesNode = nodeNode->FirstChild("properties");
												if(propertiesNode)
												{
													propertyNode = propertiesNode->IterateChildren("property", 0);
													while( propertyNode )
													{
														FxBool propertySuccess = FxFalse;

														propertyElement = propertyNode->ToElement();
														if(propertyElement)
														{
															FxName propName(propertyElement->Attribute("name"));
															if(FxInvalidIndex != pNode->FindUserProperty(propName))
															{
																// Make sure the node type supports this user property.
																FxSize propIndex = pNode->FindUserProperty(propName);
																if( FxInvalidIndex != propIndex)
																{
																	FxInt32 type = -1;
																	if ( TIXML_SUCCESS == propertyElement->QueryIntAttribute("property_enum", &type) )
																	{
																		FxFaceGraphNodeUserProperty propI(UPT_Integer);
																		FxFaceGraphNodeUserProperty propB(UPT_Bool);
																		FxFaceGraphNodeUserProperty propR(UPT_Real);
																		FxFaceGraphNodeUserProperty propS(UPT_String);
																		FxFaceGraphNodeUserProperty propC(UPT_Choice);

																		FxInt32 valueI = -1;
																		FxDReal valueR = 0.0f;
																		FxString valueS;
							
																		switch(type)
																		{
																		case UPT_Integer:
																			if ( TIXML_SUCCESS == propertyElement->QueryIntAttribute("value", &valueI) )
																			{
																				propI.SetIntegerProperty(valueI);
																				propI.SetName(propName);
																				pNode->ReplaceUserProperty(propI);
																				propertySuccess = FxTrue;
																			}
																			break;
																		case UPT_Bool:
																			if ( TIXML_SUCCESS == propertyElement->QueryIntAttribute("value", &valueI) )
																			{
																				propB.SetName(propName);
																				if(valueI == 0 )
																				{
																					propB.SetBoolProperty(FxFalse);
																					pNode->ReplaceUserProperty(propB);
																					propertySuccess = FxTrue;

																				}
																				if( valueI == 1)
																				{
																					propB.SetBoolProperty(FxFalse);
																					pNode->ReplaceUserProperty(propB);
																					propertySuccess = FxTrue;
																				}
																			}
																			break;
																		case UPT_Real:
																			if ( TIXML_SUCCESS == propertyElement->QueryDoubleAttribute("value", &valueR) )
																			{
																				propR.SetRealProperty(FxReal(valueR));
																				propR.SetName(propName);
																				pNode->ReplaceUserProperty(propR);
																				propertySuccess = FxTrue;
																			}
																			break;
																		case UPT_String:
																			propS.SetName(propName);
																			propS.SetStringProperty( FxString(propertyElement->Attribute("value")));
																			pNode->ReplaceUserProperty(propS);
																			propertySuccess = FxTrue;
																			break;
																		case UPT_Choice:
																			propC.SetName(propName);
																			propC.SetStringProperty(FxString(propertyElement->Attribute("value")));
																			pNode->ReplaceUserProperty(propC);
																			propertySuccess = FxTrue;
																			break;
																		}
																	}
																}
															}
														}
														if(!propertySuccess)
														{
															FxWarn("Error importing user property for node <b>%s</b>.", nodeName.GetData());
														}
														propertyNode = propertiesNode->IterateChildren("property", propertyNode);
													}
												}
											}
											FxSessionProxy::AddFaceGraphNode( pNode );
										}
										else
										{
											FxWarn("Could not add node <b>%s</b>.", nodeName.GetData());
										}
									}
								}
								nodeNode = nodesNode->IterateChildren("node",  nodeNode );
							}
						}
						linksNode = faceGraphNode->FirstChild("links");
						if( linksNode )
						{
							FxFaceGraph faceGraph = pActor->GetDecompiledFaceGraph();
							linkNode = linksNode->IterateChildren( "link", 0 );
							while( linkNode )
							{
								linkElement = linkNode->ToElement();
								if(linkElement)
								{
									if( !linkElement->Attribute("to") || 
										!linkElement->Attribute("from") ||
										!linkElement->Attribute("function") )
									{
										FxWarn("Ignoring link node without to, from, and function attributes.");
										break;
									}
									
									FxString toString = linkElement->Attribute("to");
									FxString fromString = linkElement->Attribute("from");
									FxFaceGraphNode* toNode = faceGraph.FindNode(toString.GetData());
									FxFaceGraphNode* fromNode = faceGraph.FindNode(fromString.GetData());
									FxString linkFn(linkElement->Attribute("function"));
									const FxLinkFn* pLinkFn = FxLinkFn::FindLinkFunction(linkFn.GetData());
				
									if( toNode && fromNode && pLinkFn )
									{
										FxLinkFnParameters params;
										parametersNode = linkNode->FirstChild("parameters");
										if( parametersNode )
										{
											parametersElement = parametersNode->ToElement();
											if( parametersElement )
											{
												FxString parameterString(parametersElement->GetText());
												FxArray<FxReal> parameterArray;
												_ConvertSpaceSeparatedString(parameterString, parameterArray);
													
												FxInt32 numParams = 0;
												FxInt32 result = parametersElement->QueryIntAttribute("num_params", &numParams);
												if(	result == TIXML_SUCCESS						 &&
													parameterArray.Length() == (FxSize)numParams &&
													pLinkFn->GetNumParams() == (FxSize)numParams	)
												{
													params.parameters = parameterArray;
												}
												else
												{
													FxWarn("Error importing parameters in link function from <b>%s</b> to <b>%s</b>.  Using default values.", fromString.GetData(), toString.GetData()); 
												}
											}
											FxSessionProxy::AddFaceGraphNodeLink(fromNode, toNode, pLinkFn, params);
										}	
									}
								}
								linkNode = linksNode->IterateChildren( "link", linkNode );
							}
						}
						pSession->OnActorInternalDataChanged(NULL, ADCF_FaceGraphChanged);
						FxSessionProxy::CompileFaceGraph();
						FxSessionProxy::LayoutFaceGraph();
					}
				}

				mappingNode = actorNode->FirstChild( "mapping" );
				if( mappingNode )
				{
					FxPhonemeMap map;
					
					entryNode = mappingNode->IterateChildren( "entry", 0 );
					while( entryNode )
					{
						entryElement = entryNode->ToElement();
						if( entryElement )
						{
							FxName phon(entryElement->Attribute("phoneme"));
							FxName target(entryElement->Attribute("target"));
							FxDReal amount = 0.0f;
							entryElement->QueryDoubleAttribute("amount", &amount);
							FxPhoneme phoneme = _NameToPhoneme(phon);
							if( phoneme <= PHON_LAST )
							{
								map.SetMappingAmount(phoneme, target, (FxReal)amount);
							}
						}
						entryNode = mappingNode->IterateChildren( entryNode );
					}
					FxSessionProxy::SetPhonemeMap(map);
				}
				animationGroupsNode = actorNode->FirstChild( "animation_groups" );
				if(animationGroupsNode)
				{
					animationGroupNode = animationGroupsNode->IterateChildren( "animation_group",  0 );
					while( animationGroupNode )
					{
						animationGroupElement = animationGroupNode->ToElement();
						if( animationGroupElement )
						{
							FxString groupName(animationGroupElement->Attribute( "name" ));
							if(groupName.Length() > 0)
							{
								// Just try adding the group, if it fails, it's already there.
								FxSessionProxy::CreateAnimGroup(groupName);
								FxSessionProxy::SetSelectedAnimGroup(groupName);

								animNode = animationGroupNode->IterateChildren( "animation", 0 );
								while( animNode )
								{
									animationElement = animNode->ToElement();
									if( animationElement )
									{
										FxString animName(animationElement->Attribute( "name" ));
										if(animName.Length() > 0)
										{
											FxAnim anim;
											anim.SetName(animName.GetData());
											FxMsg("Adding animation <b>%s</b> in group <b>%s</b>.", animName.GetData(), groupName.GetData());

											FxAnimUserData* pUserData = new FxAnimUserData();
											FxPhonWordList *pPhonWordList = new FxPhonWordList();
											pUserData->SetNames(groupName, animName);
											pUserData->SetRandomSeed(static_cast<FxInt32>(time(0)));

											FxString animName(animationElement->Attribute( "name" ));

											FxString coarticulationConfig(animationElement->Attribute( "coarticulation_config" ));
											if(coarticulationConfig.Length() > 0)
											{
												pUserData->SetCoarticulationConfig(coarticulationConfig);
											}
											FxString gestureConfig(animationElement->Attribute( "gesture_config" ));
											if(gestureConfig.Length() > 0)
											{
												pUserData->SetGestureConfig(coarticulationConfig);
											}
											pUserData->SetLanguage(FxString(animationElement->Attribute( "language" )));
											if(FxString(animationElement->Attribute( "analysis_results" ))  == "off")
											{
												pUserData->SetShouldContainAnalysisResults(FxFalse);
											}
											if(FxString(animationElement->Attribute( "speech_gestures" ))  == "off")
											{
												pUserData->SetShouldContainSpeechGestures(FxFalse);
											}
											// Use the full path to the audio if the file exists. 
											wxString audioPathStr(animationElement->Attribute( "audio_path_full" ), wxConvUTF8);
											wxFileName audioPathFileName(audioPathStr);
											if( !audioPathFileName.FileExists() )
											{
												audioPathStr = wxString(animationElement->Attribute( "audio_path" ), wxConvUTF8);
											}
											// Audio path stored as an FxString so unicode paths not possible.
											pUserData->SetAudioPath(FxString(audioPathStr.mb_str(wxConvLibc)));

											// UE3 Stuff
											anim.SetSoundCuePath(animationElement->Attribute( "audio_path" ));
											anim.SetSoundNodeWave(animationElement->Attribute( "audio_name" ));

											FxInt32 UE3SoundCueIndex = 0;
											animationElement->Attribute("sound_cue_index", &UE3SoundCueIndex);
											anim.SetSoundCueIndex(UE3SoundCueIndex);

											analysisTextNode = animNode->FirstChild( "analysis_text" );
											if(analysisTextNode)
											{
												analysisTextElement = analysisTextNode->ToElement();
												if( analysisTextElement )
												{
													wxString analysisText(analysisTextElement->GetText(), wxConvUTF8);
													pUserData->SetAnalysisText(FxWString(analysisText.wc_str()));
												}
											}
											phonemesNode = animNode->FirstChild( "phonemes" );
											if( phonemesNode )
											{
												phonemeNode = phonemesNode->IterateChildren( "phoneme", 0 );
												
												FxDReal end_last = 0.0f;
												while( phonemeNode )
												{
													phonemeElement = phonemeNode->ToElement();
													if( phonemeElement )
													{
														FxDReal start = 0.0f;
														FxDReal end = 0.0f;													
														if (	TIXML_SUCCESS == phonemeElement->QueryDoubleAttribute("start", &start) &&
																TIXML_SUCCESS == phonemeElement->QueryDoubleAttribute("end", &end) &&
																end > start )															
														{
															// Make sure the start time of a phoneme is equal to the end time of the prior phoneme. 
															if(	!(pPhonWordList->GetNumPhonemes() > 0 && 
																start != end_last ) )
															{
																end_last = end;
																FxName phon(phonemeElement->Attribute("phoneme"));
																FxPhoneme phoneme = _NameToPhoneme(phon);
																pPhonWordList->AppendPhoneme(phoneme, (FxReal)start, (FxReal)end);
															}
														}
													}
													phonemeNode = phonemesNode->IterateChildren( "phoneme", phonemeNode );
												}
												// Words are meaningless without phonenes, so don't include words without phonemes.
												wordsNode = animNode->FirstChild( "words" );
												if( wordsNode )
												{
													wordNode = wordsNode->IterateChildren( "word", 0 );
													while( wordNode )
													{
														wordElement = wordNode->ToElement();
														if( wordElement )
														{
															FxDReal start = 0.0f;
															FxDReal end = 0.0f;
															if (	TIXML_SUCCESS == wordElement->QueryDoubleAttribute("end", &end) &&
																	TIXML_SUCCESS == wordElement->QueryDoubleAttribute("start", &start)	)																	
															{
																wxString str(wordElement->GetText(), wxConvUTF8);
																pPhonWordList->GroupToWord(str, (FxReal)start, (FxReal)end);
															}
														}
														wordNode = wordsNode->IterateChildren( "word", wordNode );
													}
												}
											}
											curvesNode = animNode->FirstChild( "curves" );
											if( curvesNode )
											{
												curveNode = curvesNode->IterateChildren( "curve", 0 );
												while( curveNode )
												{
													curveElement = curveNode->ToElement();
													FxName curveName(curveElement->Attribute( "name" ));
													
													// The default is owned by user.  So check if the curve is owned by analyis.
													if(FxString(curveElement->Attribute( "owner" ))  == "analysis")
													{
														pUserData->SetCurveOwnedByAnalysis(curveName, FxTrue);
													}
													FxBool bValidCurveFormat = FxTrue;
													FxInt32 numKeysInt = 0;
													if(curveElement->QueryIntAttribute("num_keys", &numKeysInt))
													{
														bValidCurveFormat = FxFalse; 
													}
													
													FxSize numKeys = (FxSize)numKeysInt;
													FxArray<FxReal> keyValues;
													FxString key_data(curveElement->GetText()); 
													_ConvertSpaceSeparatedString(key_data, keyValues);
													if(	curveName == FxName::NullName || 
														numKeysInt < 0	||
														keyValues.Length() != numKeys * 4	)
													{
														bValidCurveFormat = FxFalse; 
													}
													if( bValidCurveFormat )
													{
														FxAnimCurve curve;
														curve.SetName(curveName);

														for(FxSize i = 0; i < numKeys; ++i)
														{
															FxAnimKey key;
															key.SetTime(keyValues[i*4 + 0]);
															key.SetValue(keyValues[i*4 + 1]);
															key.SetSlopeIn(keyValues[i*4 + 2]);
															key.SetSlopeOut(keyValues[i*4 + 3]);
															curve.InsertKey(key);
														}
														// Remove the curve if it exists. Duplicates not allowed.
														anim.RemoveAnimCurve( curveName );
														anim.AddAnimCurve( curve );
													}
													else
													{
														FxWarn("Invalid curve format in curve <b>%s</b>.", curveName.GetAsString());
													}
													curveNode = curvesNode->IterateChildren( "curve", curveNode );
												}
											}
	
											// Remove the animation if it exists.  Duplicates not allowed.
											FxSessionProxy::PreRemoveAnim(groupName,animName);
											FxSessionProxy::RemoveAnim(groupName, animName);
											if(FxSessionProxy::AddAnim(groupName, anim))
											{
												FxAnim* pNewAnim = NULL;
												if( FxSessionProxy::GetAnim(groupName, animName, &pNewAnim ))
												{
													if(pNewAnim)
													{
#ifdef __UNREAL__
														UFaceFXAsset* pFaceFXAsset = 0;
														FxSessionProxy::GetFaceFXAsset(&pFaceFXAsset);
														if( pFaceFXAsset )
														{
															pFaceFXAsset->ReferencedSoundCues.Empty();
															pFaceFXAsset->FixupReferencedSoundCues();
														}
#endif // __UNREAL__
														FxSessionProxy::SetSelectedAnim(animName);
														FxAnimUserData* pNewAnimUserData = reinterpret_cast<FxAnimUserData*>(pNewAnim->GetUserData());
				
														(*pNewAnimUserData) = (*pUserData);
														pNewAnimUserData->SetPhonemeWordList(pPhonWordList);
#ifndef __UNREAL__
														if(pUserData->GetAudioPath().Length() == 0)
														{
															FxWarn("Audio file path is empty.");
														}
														else
														{
															// Now add the audio.  The path can be stored as 1) relative to studio, relative to the actor, or as a full path.
															wxString qualifiedAudioPath;
															wxString relativeAudioPath(pUserData->GetAudioPath().GetData(), wxConvLibc);														
															wxFileName audioFilePath(relativeAudioPath);
															
															wxString wxActorPath(actorPath.GetData(),wxConvLibc );														
															wxFileName wxFileNameActor(wxActorPath);
															wxFileName relativeToActorPath = wxFileNameActor.GetPath() + wxFileName::GetPathSeparator() + relativeAudioPath;
															if(relativeToActorPath.FileExists())
															{
																qualifiedAudioPath = relativeToActorPath.GetFullPath();															
															}
															else if(audioFilePath.FileExists())
															{
																qualifiedAudioPath = audioFilePath.GetFullPath();															
															}
															else
															{
																FxWarn("Audio file path <b>%s</b> is neither a complete path, nor relative from FaceFX Studio, nor relative from the current actor path. No audio will be loaded.", pNewAnimUserData->GetAudioPath().GetData());
															}														
															FxString filePath(qualifiedAudioPath.mb_str(wxConvLibc));
															if(filePath.Length() > 0)
															{
																if( FxFileExists(filePath) )
																{
																	FxAudioFile theAudioFile(filePath);
																	if( theAudioFile.Load() )
																	{
																		FxAudio* pAudio = new FxAudio(theAudioFile);
																		pAudio->SetName(animName.GetData());
																		pNewAnimUserData->SetAudio(pAudio);
																		delete pAudio;
																	}
																	else
																	{
																		FxWarn("Audio file <b>%s</b> is not 16-bit with a sample rates greater than or equal to 16-kHz. No audio will be loaded.", pNewAnimUserData->GetAudioPath().GetData());
																	}
																}
															}
														}
#endif 
														// It is not safe to unload this user data because it is not a lazy-loaded
														// object from the original session.
														pNewAnimUserData->SetIsSafeToUnload(FxFalse);								
													}
												}
											}
											if( pPhonWordList )
											{
												delete pPhonWordList;
												pPhonWordList = NULL;
												pUserData->SetPhonemeWordList(NULL);
											}
											if( pUserData )
											{
												delete pUserData;
												pUserData = NULL;
											}
										}
									}
									animNode = animationGroupNode->IterateChildren( "animation", animNode );
								}
							}
						}
						animationGroupNode = animationGroupsNode->IterateChildren("animation_group",  animationGroupNode );
					}
				}

				// Use the session to set the animation as empty.  The last animation imported needs to be unselected
				// and reselected to view its user data.
				pSession->OnAnimChanged(NULL, FxAnimGroup::Default, NULL);
				return FxTrue;
			}
		}
	}
	return FxFalse;
}

} // namespace Face

} // namespace OC3Ent

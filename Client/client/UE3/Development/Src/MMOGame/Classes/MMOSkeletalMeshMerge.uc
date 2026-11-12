/*=============================================================================
	MMOSkeletalMeshMerge.uc: Helper class for creating merged avatars.
	ming@snda
=============================================================================*/

class MMOSkeletalMeshMerge extends SkeletalMesh
	native;

struct native BodyPart
{
	var SkeletalMesh		Mesh;
	var init array<Texture2D>	Textures;	// should be ordered by priority
	var init array<Texture2D>	Textures_N;	// normal
	var init array<Texture2D>	Textures_S;	// specular + alpha + emmisive
	var init array<Texture2D>	Textures_Base;	// because the color map can be changed, the S/N texture names are now taken from here

	// texture uv, mip0
	var int		OrigX;
	var int		OrigY;
	var int		SizeX;
	var int		SizeY;
};

var transient init array< BodyPart >	Parts;

var int	TargetTexSizeX;
var int	TargetTexSizeY;

//var Texture2DComposite	MergedTex;
var transient Texture2DDynamic	MergedTex_D;
var transient Texture2DDynamic	MergedTex_N;
var transient Texture2DDynamic	MergedTex_S;

// reset material if not correct
// in case the artists change the materials
// it's impossible to make the artists here understand the issue
var Material	NewMaterialTemplate;

var transient native pointer TexMergeUtil{class FTextureMergeUtil};

// true after merge is done
var transient bool	bDoneMerge;

// num of maximum mip levels
var int	NumMipLevels;


native final function Init( int partNum );

// get normal and specular textures according to diffuses
native final function bool PrepareTextures();

// check if all mips are streamed in
native final function bool IsSourceTexturesFullyLoaded();

native final function bool LastMergeCancelled();

native final function bool DoMerge();

native final function UpdateState();

// check whether merge operation has finished
native final function bool CheckFinish();

native final function PostMerge();

// cancel latest merging
native final function CancelMerge();

native final function bool Equals( const MMOSkeletalMeshMerge other ) const;

// deal with skin color changes
native final function SetSkinColor( linearcolor NewColor );

native final function SetSkinSpecular( linearcolor NewSpecular );

// Specular outline effect of models that the artists love...
native final function SetFakeSilhouetteColor( linearcolor NewFakeSilhouette );

defaultproperties
{
	TargetTexSizeX=512
	TargetTexSizeY=256
	NumMipLevels=1

	NewMaterialTemplate=Material'MMO_MaterialTemplate.Character.MT_Player'
	//NewMaterialTemplate=Material'MMO_MaterialTemplate.Character.MT_Opaque'
	//NewMaterialTemplate=Material'NewAvatarTest.NormalTestMat'
}
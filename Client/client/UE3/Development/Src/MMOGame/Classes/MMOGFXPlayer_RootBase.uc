class MMOGFXPlayer_RootBase extends MMOGFXPlayer
	abstract
	native(UI)
	config(UI);

/** Reference to _root of the movie's (udk_manager.swf) stage. */
var GFxObject RootMC;

/** Reference to the manager MovieClip (_root.manager) where views will be attached. */
var GFxObject ManagerMC;


/** Structure which defines a unique menu view to be loaded. */
struct native GFXUIViewInfo
{
	var name WidgetClass;

	/** Unique string. */
	var name ViewName;

    /** SWF content to be loaded. */
    var init string SWFName;

    /** Dependant views that should be loaded if this view is displayed. */
    var array<name> DependantViews;

	var bool PreLoad;
	var byte Priority;

	structdefaultproperties
	{
		// 0 1 2 3 4 0 1 2 3 4, 5 levels in total, higher ones are shown on top
		Priority=2
	}
};

/** Array of all menu views to be loaded, defined in DefaultUI.ini. */
var config array<GFXUIViewInfo>			ViewData;

/**
 * An array of names of views which have been attachMovie()'d and loadMovie()'d. Views
 * are loaded based on their DependantViews array, defined in Default.ini.
 */
var array<name>						LoadedViews;

// add by zhangrui05
var array<name>						LoadedMovies;

/** 
 *  Create a view using existing ViewInfo. 
 *
 *  @param InViewInfo, the data for the view which includes the SWFName and the name for the view.
 */
native final function bool LoadView(GFXUIViewInfo InViewInfo);

/** 
 *  Loads a view using the view name. Used for loading views based on the current screen.
 *  Views that should be loaded for each screen are defined in DefaultUI.ini.
 *
 *  @param InViewName		The name of the view to be loaded.
 */
native final function bool LoadViewByName( name InViewName );

/**  
 *  Checks whether a view has already been loaded using the view name. 
 *
 *  @param InViewName		The name of the view to check.
 */
native final function bool IsViewLoaded( name InViewName );

event CallTraceAllChild()
{
	ActionScriptVoid("root.traceAllChild");
}

event GFxObject CallGetDropIcon()
{
	return ActionScriptObject("root.getDropIcon");
}

event CallLoadMovie(string swf)
{
	ActionScriptVoid("root.UCLoadMovie");
}

event UCClearFocus()
{
	ActionScriptVoid("root.UCClearFocus");
}

event CallUCUpdateSubtitle(string htmlStr)
{
	ActionScriptVoid("root.UCUpdateSubtitle");
}

event CallUCShow(bool show, int scrW, int scrH)
{
	ActionScriptVoid("root.UCShow");
}

native function PostAdvance(float DeltaTime);


native function BindingWidgetsInfo();

native function bool WidgetInitialized(name WidgetName, name WidgetPath, GFxObject Widget);
native function bool WidgetUnloaded(name WidgetName, name WidgetPath, GFxObject Widget);


DefaultProperties
{
	//Priority=5
	MovieInfoStr="UI_Root.UI_Root"
	IsLockedDynamicLoad=true
	bCloseOnLevelChange=FALSE
	//bAllowFocus=FALSE
	Priority=5
}

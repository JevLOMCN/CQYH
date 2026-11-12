//
// KeyKey message handling state class
//
class KeyEventProcessDisableState extends KeyEventProcess
	native;

native function OnEnter();
native function OnLeave();

// 0~9
native function OnNumberKey(EKeyCode KeyValue, bool bRelease);

// a~z
native function OnCharacterKey(EKeyCode KeyValue, bool bRelease);

// blank ,  . / \ = - Enter
native function OnSymbolKey(EKeyCode KeyValue, bool bRelease);

// F1~ F12
native function OnFunctionKey(EKeyCode KeyValue, bool bRelease);

// CTRL, ALT, SHIFT
native function OnModifierKey(EKeyCode KeyValue, bool bRelease);

// HOME, END, PGUP, PGDN, Caps Lock, Scroll Lock, Num Lock, Left, Right, Up, Down
native function OnNavigationKey(EKeyCode KeyValue, bool bRelease);

// ESC, PRSCRN, PAUSE/BREAK, WINDOWS
native function OnSystemCommandKey(EKeyCode KeyValue, bool bRelease);

// Mouse Left Key, Mouse Right Key, MouseScrollUp, MouseScrollDown
native function OnMouseKey(EKeyCode KeyValue, bool bRelease);


DefaultProperties
{

}

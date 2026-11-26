param([Parameter(Mandatory = $true)][string]$Path)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

# Generated files use legacy source encodings. Preserve every original byte.
$encoding = [Text.Encoding]::GetEncoding(28591)
$filePath = [IO.Path]::GetFullPath($Path)
$original = $encoding.GetString([IO.File]::ReadAllBytes($filePath))
$text = $original.Replace("`r`n", "`n")
$targets = @{}
$functionPattern = '(?ms)^static int [^(]+\(lua_State\* tolua_S\)\s*\{.*?^\}'
$copyPattern = 'strncpy\(\(char\*\)\s*(self->\w+|\w+),\s*\(const char\*\)tolua_tostring\(tolua_S,2,0\),\s*[^;]+\);'

function Get-Owner([string]$body) {
    $match = [regex]::Match($body, '\b((?:\w+::)*\w+)\s*\*\s*self\s*=')
    if ($match.Success) { return $match.Groups[1].Value }
    return ''
}

# Identify fixed arrays from their generated strncpy setters, rather than treating
# every Lua C-string getter as an array (some getters return pointers).
foreach ($function in [regex]::Matches($text, $functionPattern)) {
    $owner = Get-Owner $function.Value
    foreach ($copy in [regex]::Matches($function.Value, $copyPattern)) {
        $targets[$owner + ':' + $copy.Groups[1].Value] = $true
    }
}

$text = [regex]::Replace($text, $functionPattern, [Text.RegularExpressions.MatchEvaluator]{
    param($function)
    $body = $function.Value
    $owner = Get-Owner $body
    $indexPattern = 'tolua_index = \(int\)tolua_tonumber\(tolua_S,2,0\);'
    if ([regex]::IsMatch($body, $indexPattern)) {
        $array = [regex]::Match($body, '(self->\w+)\[tolua_index\]')
        if (!$array.Success) { throw "Unrecognized generated array access in $filePath" }
        $guard = 'if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), ' +
            $array.Groups[1].Value + ', tolua_index)) return 0;'
        $body = [regex]::Replace($body, $indexPattern, $guard)
    }
    $body = [regex]::Replace($body, $copyPattern, [Text.RegularExpressions.MatchEvaluator]{
        param($copy)
        $target = $copy.Groups[1].Value
        $guard = if ($target.StartsWith('self->')) { "if (!self) return 0;`n " } else { '' }
        return $guard + 'ClientFixedString::Assign(' + $target + ', tolua_tostring(tolua_S,2,0));'
    })
    $body = [regex]::Replace($body,
        'tolua_pushstring\(tolua_S,\s*\(const char\*\)(self->\w+|\w+)\);',
        [Text.RegularExpressions.MatchEvaluator]{
            param($push)
            $target = $push.Groups[1].Value
            if (!$targets.ContainsKey($owner + ':' + $target)) { return $push.Value }
            $guard = if ($target.StartsWith('self->')) { "if (!self) return 0;`n " } else { '' }
            return $guard + 'lua_pushlstring(tolua_S, ' + $target + ', ClientFixedString::Length(' + $target + '));'
        })
    return $body
})

if ([regex]::IsMatch($text, '\bstrncpy\s*\(')) {
    throw "Unrecognized generated string setter in $filePath"
}
if ($text.Contains('ClientFixedString::') -and !$text.Contains('#include "../../../utility/fixed_string.h"')) {
    $include = '#include "tolua++.h"'
    if (!$text.Contains($include)) { throw "Missing tolua include in $filePath" }
    $text = $text.Replace($include, $include + "`n" + '#include "../../../utility/fixed_string.h"')
}
if ($text.Contains('ClientLuaArray::') -and !$text.Contains('#include "../../../utility/lua_array_index.h"')) {
    $include = '#include "tolua++.h"'
    if (!$text.Contains($include)) { throw "Missing tolua include in $filePath" }
    $text = $text.Replace($include, $include + "`n" + '#include "../../../utility/lua_array_index.h"')
}
$text = $text.Replace("`n", "`r`n")
if ($text -ne $original) {
    $temporary = $filePath + '.fixed-' + [Guid]::NewGuid().ToString('N') + '.tmp'
    try {
        [IO.File]::WriteAllBytes($temporary, $encoding.GetBytes($text))
        for ($attempt = 0; ; ++$attempt) {
            try {
                [IO.File]::Replace($temporary, $filePath, [System.Management.Automation.Language.NullString]::Value)
                break
            } catch [IO.IOException] {
                if ($attempt -ge 5) { throw }
                Start-Sleep -Milliseconds 100
            }
        }
    } finally {
        if ([IO.File]::Exists($temporary)) { [IO.File]::Delete($temporary) }
    }
}

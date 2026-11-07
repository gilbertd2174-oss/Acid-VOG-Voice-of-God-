param([ValidateSet('Release','Debug')][string]$Configuration='Release')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$artefacts=Join-Path $root "build/AcidVOG_artefacts/$Configuration"
$plugin=Join-Path $artefacts 'VST3/Acid VOG.vst3'
if(-not(Test-Path -LiteralPath $plugin)){throw 'VST3 build was not found.'}
$dist=Join-Path $root 'dist'
$package=Join-Path $dist 'Acid-VOG-Windows-x64'
if(Test-Path -LiteralPath $package){throw 'Previous package exists. Move it aside before packaging again.'}
New-Item -ItemType Directory -Path $package -Force|Out-Null
Copy-Item -LiteralPath $plugin -Destination $package -Recurse
$exe=Join-Path $artefacts 'Standalone/Acid VOG.exe'
if(Test-Path -LiteralPath $exe){Copy-Item -LiteralPath $exe -Destination $package}
foreach($name in @('README.md','INSTALL.md','LICENSE','THIRD_PARTY.md','VALIDATION.md')){Copy-Item -LiteralPath (Join-Path $root $name) -Destination $package}
Compress-Archive -LiteralPath $package -DestinationPath (Join-Path $dist 'Acid-VOG-Windows-x64.zip') -Force
Write-Host "Package ready: $dist"

$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$source = Join-Path $root 'upstream'
if (!(Test-Path $source)) {
    git clone --recursive https://github.com/horsicq/DIE-engine.git $source
    if ($LASTEXITCODE -ne 0) { throw 'Unable to clone DIE-engine.' }
} else {
    git -C $source submodule update --init --recursive
    if ($LASTEXITCODE -ne 0) { throw 'Unable to initialize dependencies.' }
}
git -C $source rev-parse HEAD

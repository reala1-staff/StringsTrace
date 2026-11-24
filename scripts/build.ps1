param([string]$QtPath)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
& "$PSScriptRoot/setup.ps1"
$source = Join-Path $root 'upstream'
$build = Join-Path $root 'build'
$dist = Join-Path $root 'dist'
$main = Join-Path $source 'src/gui/main_gui.cpp'
$text = [IO.File]::ReadAllText($main)
$anchor = '        mainWindow.show();'
if (!$text.Contains('StringsTrace theme')) {
    if (!$text.Contains($anchor)) { throw 'Upstream GUI entry point changed.' }
    $theme = [IO.File]::ReadAllText((Join-Path $root 'ui/theme.qss'))
    $patch = '        // StringsTrace theme' + "`n" + '        app.setStyleSheet(QString::fromUtf8(R"ST(' + $theme + ')ST"));' + "`n" + '        mainWindow.setWindowTitle("StringsTrace");' + "`n"
    [IO.File]::WriteAllText($main, $text.Replace($anchor, $patch + $anchor))
}
$args = @('-S', $source, '-B', $build, '-DDIE_BUILD_GUI=ON', '-DDIE_BUILD_LITE=OFF')
if ($QtPath) { $args += "-DCMAKE_PREFIX_PATH=$QtPath" }
cmake @args
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
cmake --build $build --config Release --parallel
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
cmake --install $build --config Release --prefix $dist
if ($LASTEXITCODE -ne 0) { throw 'Install failed.' }

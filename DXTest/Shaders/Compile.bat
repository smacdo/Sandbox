REM /Od /Zi is for debuggin

REM Simple texture shader
fxc.exe /E main /T vs_5_0 /Fo SimpleTextureVertexShader.cso SimpleTextureVertexShader.hlsl
fxc.exe /E main /T ps_5_0 /Fo SimpleTexturePixelShader.cso SimpleTexturePixelShader.hlsl

REM Light shader.
fxc.exe /Od /Zi /E main /T vs_5_0 /Fo SimpleLightVertexShader.cso SimpleLightVertexShader.hlsl
fxc.exe /Od /Zi /E main /T ps_5_0 /Fo SimpleLightPixelShader.cso SimpleLightPixelShader.hlsl

REM Font shader.
fxc.exe /Od /Zi /E main /T vs_5_0 /Fo FontVertexShader.cso FontVertexShader.hlsl
fxc.exe /Od /Zi /E main /T ps_5_0 /Fo FontPixelShader.cso FontPixelShader.hlsl
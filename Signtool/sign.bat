cd C:\Users\PC\Desktop\signer
signtool.exe sign /v /ac SectigoPublicCodeSigningCAR36.crt /f HG.pfx /p aa123123 "C:\Users\PC\source\repos\Dll1\x64\Debug\Dll1.dll"
move "C:\Users\PC\source\repos\Dll1\x64\Debug\Dll1.dll" "C:\SYNCED\Engine\Binaries\ThirdParty\CEF3\Win64\d3dcompiler_47.dll"
"C:\SYNCED\ArkGame\Binaries\Win64\SYNCED.exe" ArkGame --open_id=1926974186469552919 --token=0b27f35cefede7302228d66564fabf3aea3ea302 --channel_id=131 --region=616 --steam_id=0 -culture=en -language=en -locale=en -LoginPlatform=6 -NOSCREENMESSAGES
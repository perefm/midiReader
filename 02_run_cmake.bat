rmdir rtmidi_test_vs2022 /s /q
mkdir rtmidi_test_vs2022
cd rtmidi_test_vs2022
cmake.exe -DVCPKG_TARGET_TRIPLET=x64-windows-static -G "Visual Studio 17 2022" ..
cd..

pause
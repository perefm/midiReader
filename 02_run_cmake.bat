rmdir midiReader_vs2022 /s /q
mkdir midiReader_vs2022
cd midiReader_vs2022
cmake.exe -DVCPKG_TARGET_TRIPLET=x64-windows-static -G "Visual Studio 17 2022" ..
cd..

pause
rmdir midiReader_vs2026 /s /q
mkdir midiReader_vs2026
cd midiReader_vs2026
cmake.exe -DVCPKG_TARGET_TRIPLET=x64-windows-static -G "Visual Studio 18 2026" ..
cd..

pause
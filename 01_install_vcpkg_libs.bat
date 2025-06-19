rmdir vcpkg /s /q
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
powershell "dir *.ps* -recurse | Set-AuthenticodeSignature -Certificate (dir cert:\currentuser\my -code)"
call bootstrap-vcpkg.bat
vcpkg install --recurse rtmidi:x64-windows-static wolf-midi:x64-windows-static --triplet x64-windows --keep-going --clean-after-build
cd..
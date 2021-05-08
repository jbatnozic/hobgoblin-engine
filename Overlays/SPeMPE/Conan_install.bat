conan install . -if Build/SPeMPE-x64-Debug/ ^
    --profile=default ^
    --build=outdated ^
    -s build_type=Debug
    
conan install . -if Build/SPeMPE-x64-Release/ ^
    --profile=default ^
    --build=outdated ^
    -s build_type=Release
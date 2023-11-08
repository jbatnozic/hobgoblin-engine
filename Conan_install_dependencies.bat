
conan install . -of _Build/Hobgoblin-x64/ ^
    --profile=hobgoblin ^
    -s build_type=Debug ^
    --build=missing ^
    --build=outdated ^
    --build=cascade
    
conan install . -of _Build/Hobgoblin-x64/ ^
    --profile=hobgoblin ^
    -s build_type=Release ^
    --build=missing ^
    --build=outdated ^
    --build=cascade

REM Uncomment the following to also install RelWithDebInfo profile:
:: conan install . -of _Build/Hobgoblin-x64/ ^
::     --profile=hobgoblin ^
::     -s build_type=RelWithDebInfo ^
::     --build=missing ^
::     --build=outdated ^
::     --build=cascade

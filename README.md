# LoneShooter

A 2.5D Fps Game about a lone guy trying to survive waves of enemies controlled by
an other wordly god.`

**Notes:**
To compile you need mingw or msvc.

## Compile

```powershell
 g++ -o cmds/LoneShooter.exe cmds-src/LoneShooter/loneshooter.cpp -lgdi32 -lwinmm -mwindows -lole32 -loleaut32 -luuid -O2



```
For 32bit (Using msys2's 32bit mingw Compiler):

```powershell
 $env:Path = 'C:\msys64\mingw32\bin;' + $env:Path; g++ -o cmds/LoneShooter_32.exe src/loneshooter.cpp -lgdi32 -lwinmm -mwindows -lole32 -loleaut32 -luuid -O2 -static 2>&1

```

## Run

```powershell
./LoneShooter.exe
```
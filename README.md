# LOB-engine
Limit Order Book personnal project

## Compilation guide:

```
cmake -S . -B build
cmake --build build
./build/itchparse -f fichier
```

for sanitizers : 
```
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
```

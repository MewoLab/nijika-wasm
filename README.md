# Nijika WebAssembly

Nijika is a custom lightweight WebAssembly module capable of reading specific AssetBundles and DDS textures. It is the backend for [AquaNet2](https://github.com/MewoLab/AquaNet2)'s local asset viewer.

Nijika is NOT general-purpose. It can only handle AssetBundles containing a singular texture, and specific DDS compression types (as shown below). It can also ONLY handle up to 32 MB files by default.<br>
**Use at your own discretion.**

## Features

It can read...
 - Unity (specifically 2018.4 at the moment) Texture2Ds within AssetBundles
 - DDS files compressed with DXT1, DXT3, DXT5 or uncompressed RGB, RGBA, BGR, and BGRA.

It also decodes compression on CPU, not relying on specific GPU extensions that aren't available on some systems.

## Goals

- Unity 5 support
- Increase scope for reading AssetBundles?
- Github Workflow?
- Better documentation for building & Windows

## Building

### Requirements
- Emscripten
- CMake

### Building (Unix-likes)

1. Install dependencies

> :warning: This assumes you have Vcpkg installed & set up with CMake.<br>
> Feel free to add these 2 as submodules so this can be simplified.

```bash
vcpkg install 7zip:wasm32-emscripten
vcpkg install lz4:wasm32-emscripten
```

2. Pull latest submodules

```bash
git submodule update --init --remote --recursive
```

3. Build

> :warning: This hasn't been formally tested. I use CLion, sorry

```
mkdir library
cd library
emcmake .. -DVCPKG_CMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

## Usage

Nijika does not have a proper TypeScript interface.<br>
It is up to you to implement the interface, but I will provide general guidance (as shown below)

### Initialization

1. Instantiate the module and it's memory

```typescript
let memory: Uint8Array | undefined;
let wasm = WebAssembly.instantiateStreaming(
    fetch("module.wasm"), {
        env: {
            // Memory growth is occasionally used when files are too large
            emscripten_notify_memory_growth: () => {
                memory = new Uint8Array(
                    wasmModule!.instance.exports.memory.buffer
                );
            }
        }
    }
);
```

2. Get the memory address (this will not change, so hold onto it)

**Note: Do NOT call `init()` more than once.**

```
let memoryAddress = wasm?.instance.exports.init();
```

### I/O

3. Insert the file into memory

```typescript
let file: Blob = /*blob*/;
memory?.set(
    new Uint8Array(await file.arrayBuffer()),
    memoryAddress
);
```

4. Process it & extract it
```typescript
try {
/* 
*   DDS: 0,
*   AB (Unity 2018.4): 1
*/
    const type = 1;
    
    const size = wasm?.instance.exports.open(type);
    const array = memory?.slice(memoryAddress, memoryAddress + size);
    
    const file = new Blob([array], {type: "image/png"});
    // It's complete! `file` is your asset, converted into a PNG.
} catch(err) {
    console.log(`An exception occurred.`, err);
    // Probably want to reinstantiate WASM here
}
```
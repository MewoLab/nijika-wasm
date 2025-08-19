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

1. Pull latest submodules

```bash
git submodule update --init --remote --recursive
```

2. Build

> :warning: This hasn't been formally tested. I use CLion, sorry

```
mkdir library
cd library
emcmake .. -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
cmake --build .
```

## Usage

Nijika does not have a proper TypeScript interface.<br>
It is up to you to implement the interface, but I will provide general guidance (as shown below)

(Anyone who is interested in building a TypeScript interface is welcome to, I just haven't done it yet)

### Initialization

1. Instantiate the module and it's memory

```typescript
let memory: Uint8Array | undefined;
let wasm = WebAssembly.instantiateStreaming(
    fetch("module.wasm"), {
        env: {
            // Memory growth is occasionally used when files are too large
            emscripten_notify_memory_growth: () => {
                memory = new Uint8Array(wasmModule!.instance.exports.memory.buffer);
            }
        }
    }
);
memory = new Uint8Array(wasmModule!.instance.exports.memory.buffer);
```

### I/O

2. Copy the file into WASM memory

```typescript
let file: Blob = /*blob*/;
// Allocate space in memory for the file and copy the data
let input = wasm.instance.exports.malloc(file.size);
memory?.set(
    new Uint8Array(await file.arrayBuffer()),
    input
);
```

3. Get texture
```typescript
try {
    // For DDS, use `getDDS`
    // Retain the pointer that you allocated just a moment ago
    const success = wasm.instance.exports.getDDS(input);
    // Clean up the input
    wasm.instance.exports.free(input);

    // Access the buffer
    const size = wasm.instance.exports.fifoOutputSize();
    const pointer = wasm.instance.exports.fifoOutput();

    // Convert into a blob
    const file = new Blob([
        memory!.slice(pointer, pointer + size)
    ], {type: "image/png"});
    // It's complete! `file` is your asset, converted into a PNG.
} catch(err) {
    console.log(`An exception occurred.`, err);
    // Probably want to reinstantiate WASM here
}
```
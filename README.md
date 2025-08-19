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

Nijika has a TypeScript interface [(nijika.ts)](./nijika.ts).<br>
You will have to pass in the module buffer since it cannot find it on it's own.

```typescript
import { Nijika } from "./nijika.ts";

const nijika = new Nijika(
    await fetch("./nijika.wasm").then(response => response.arrayBuffer())
);
await nijika.loaded;

// For DDS (DXT1, DXT3, DXT5)
nijika.dds(ddsBuffer) // -> outputs blob (not a promise)

// For DDS (BC1/2/3, BC7 & RGB-likes)
nijika.assetBundle(abBuffer) // -> outputs blob (not a promise)
```
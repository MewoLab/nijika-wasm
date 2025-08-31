# Nijika

![nijika](https://github.com/user-attachments/assets/870c2df1-8c30-4333-bb7c-a29762e2427a)

Nijika is a custom lightweight WebAssembly module capable of reading specific AssetBundles and DDS textures. It is the backend for [AquaNet2](https://github.com/MewoLab/AquaNet2)'s local asset viewer.

Nijika is NOT a general-purpose asset reader yet, it is only made to meet our needs. The limitations for what it is capable of reading are shown below.<br>

## Features

It can read...
 - Unity (specifically 2018.4 at the moment) Texture2Ds within AssetBundles
 - DDS files compressed with DXT1, DXT3, DXT5 or uncompressed RGB, RGBA, BGR, and BGRA.
 - AWB (with ACB) sound files using [vgmstream](https://github.com/raymonable/vgmstream)

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

> :exclamation: You can find the latest stable build in the [Releases](https://github.com/MewoLab/nijika-wasm/releases) tab.<br>
> Please read the release notes for more information.

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

// For AWB/ACB (no custom key support, it MUST be one of the keys included with VGMStream)
// Note: You cannot swap between AWBs willy nilly, you must reinstantiate with .awb() every time you swap.
const awb = nijika.awb(awbBuffer, acbBuffer) // (acb is optional) -> outputs an AWB
awb.trackList() // -> outputs string[]
awb.get(trackIndex) // (index is optional) -> outputs blob

```


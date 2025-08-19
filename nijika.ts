/*

Nijika TypeScript Interface
For V2.0

API may change.

*/

export class Nijika {
    constructor(wasmBuffer: ArrayBuffer) {
        this.loaded = new Promise<void>(async complete => {
            await WebAssembly.instantiate(
                wasmBuffer, {
                    env: {
                        emscripten_notify_memory_growth: () => {
                            this.memory = new Uint8Array(
                                this.wasmModule!.instance.exports.memory.buffer
                            );
                        }
                    }
                }
            ) as typeof this.wasmModule;
            this.memory = new Uint8Array(
                this.wasmModule!.instance.exports.memory.buffer
            );
            complete();
        })
    };

    dds(input: ArrayBuffer | Uint8Array): Blob | undefined {
        const pointer = this.getInputBuffer(input);
        if (!this.wasmModule.instance.exports.getDDS(pointer))
            return;
        this.free(pointer);
        return this.getOutputBuffer("image/png");
    }

    assetBundle(input: ArrayBuffer | Uint8Array): Blob | undefined {
        const pointer = this.getInputBuffer(input);
        if (!this.wasmModule.instance.exports.getAssetBundleDirect(pointer))
            return;
        this.free(pointer);
        return this.getOutputBuffer("image/png");
    }

    private getInputBuffer(input: ArrayBuffer | Uint8Array) {
        const pointer = this.wasmModule.instance.exports.malloc(input.byteLength);
        this.memory.set(
            new Uint8Array(input), pointer
        );
        return pointer;
    };

    private free(pointer: number) {
        this.wasmModule.instance.exports.free(pointer);
    }

    private getOutputBuffer(type: string) {
        const size = this.wasmModule.instance.exports.fifoOutputSize();
        const pointer = this.wasmModule.instance.exports.fifoOutput();
        return new Blob([this.memory.slice(pointer, pointer + size)], { type });
    }

    loaded: Promise<void> | undefined;

    private memory: Uint8Array;
    private wasmModule: WebAssembly.WebAssemblyInstantiatedSource & {
        instance: {
            exports: {
                fifoOutputSize: () => number,
                fifoOutput: () => number,

                getDDS: (ptr: number) => boolean,
                getAssetBundleDirect: (ptr: number) => boolean,

                malloc: (size: number) => number,
                free: (ptr: number) => void,

                memory: {
                    buffer: ArrayBuffer
                }
            }
        }
    }
}
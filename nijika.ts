/*

Nijika TypeScript Interface
For V2.0

API may change.

*/

export interface AWB {
    trackList: () => string[],
    get: (index?: number) => Blob | undefined
}

export class Nijika {
    constructor(wasmBuffer: ArrayBuffer) {
        this.loaded = new Promise<void>(async complete => {
            this.wasmModule = await WebAssembly.instantiate(
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
        return new Blob([this.getOutputBuffer()], { type: "image/png" });
    }

    assetBundle(input: ArrayBuffer | Uint8Array): Blob | undefined {
        const pointer = this.getInputBuffer(input);
        if (!this.wasmModule.instance.exports.getAssetBundleDirect(pointer))
            return;
        this.free(pointer);
        return new Blob([this.getOutputBuffer()], { type: "image/png" });
    }

    awb(inputAwb: ArrayBuffer | Uint8Array, inputAcb?: ArrayBuffer | Uint8Array): AWB | undefined {
        let awbPointer = this.getInputBuffer(inputAwb);
        let acbPointer = inputAcb ? this.getInputBuffer(inputAcb) : 0;

        if (!this.wasmModule.instance.exports.loadAwb(
            awbPointer, inputAwb.byteLength,
            acbPointer, inputAcb?.byteLength ?? 0
        )) return;

        this.awbOffset++;
        const index = this.awbOffset;

        this.free(awbPointer);
        if (acbPointer != 0)
            this.free(acbPointer);

        return {
            trackList: () => {
                if (index != this.awbOffset) return [];

                let trackCount = this.wasmModule.instance.exports.getAwbTrackList();
                let buffer = this.getOutputBuffer();

                if (buffer.byteLength <= 0) return Array.from({ length: trackCount })

                let tracks: string[] = [];
                for (let i = 0; trackCount > i; i++) {
                    let trackName = buffer.subarray(i * 256, (i + 1) * 256).filter(n => n != 0);
                    tracks.push(Array.from(trackName).map(v => String.fromCharCode(v)).join(""));
                } 
                return tracks;
            },
            get: (trackIndex?: number) => {
                if (index != this.awbOffset) return;
                if (!this.wasmModule.instance.exports.getAwb(trackIndex ?? 0)) return;
                return new Blob([this.getOutputBuffer()], { type: "audio/wav" });
            }
        } as AWB;
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

    private getOutputBuffer() {
        const size = this.wasmModule.instance.exports.fifoOutputSize();
        const pointer = this.wasmModule.instance.exports.fifoOutput();
        return this.memory.slice(pointer, pointer + size);
    }

    loaded: Promise<void> | undefined;

    private awbOffset = 0;

    private memory: Uint8Array;
    private wasmModule: WebAssembly.WebAssemblyInstantiatedSource & {
        instance: {
            exports: {
                fifoOutputSize: () => number,
                fifoOutput: () => number,

                getDDS: (ptr: number) => boolean,
                getAssetBundleDirect: (ptr: number) => boolean,

                getAwb: (subtrack: number) => boolean,
                getAwbTrackList: () => number,
                loadAwb: (awbPtr: number, awbSize: number, acbPtr: number, acbSize: number) => boolean,

                malloc: (size: number) => number,
                free: (ptr: number) => void,

                memory: {
                    buffer: ArrayBuffer
                }
            }
        }
    }
}
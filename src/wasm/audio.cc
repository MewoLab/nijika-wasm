//
// Created by Raymond on 19/08/2025.
//

#include <headers.hh>
#include <wasm/bindings.hh>

#include <library/audio/awb.hh>
#include <library/audio/riff.hh>

libvgmstream_t* vgmstream = nullptr;
AWB* awb = nullptr;

bool vgmStreamInit() {
    if (vgmstream != nullptr) return true;
    vgmstream = libvgmstream_init();
    if (!vgmstream) return false;
    auto config = new libvgmstream_config_t{
        .force_sfmt = LIBVGMSTREAM_SFMT_PCM16
    };
    libvgmstream_setup(vgmstream, config);
    return true;
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    bool loadAwb(void* awbBuffer, size_t awbSize, void* acbBuffer, size_t acbSize) {
        if (!vgmStreamInit())
            return false;
        if (awb != nullptr) {
            libvgmstream_close_stream(vgmstream);
            delete awb;
        }
        awb = new AWB(awbBuffer, awbSize, acbBuffer, acbSize);
        return true;
    }
    EMSCRIPTEN_KEEPALIVE
    int getAwbTrackList() {
        if (!vgmStreamInit())
            return -1;
        if (!awb) return -2;

        libvgmstream_open_stream(vgmstream, awb, 0);

        // ACB contains the actual track information, without it there's no track names
        if (awb->hasAcb()) {
            auto buffer = malloc(vgmstream->format->subsong_count * 256);
            for (size_t idx = 0; idx < vgmstream->format->subsong_count; idx++) {
                // Don't need to reopen it if we're already on index 0, silly
                if (idx != 0)
                    libvgmstream_open_stream(vgmstream, awb, idx);
                memcpy(static_cast<char*>(buffer) + (idx * 256), vgmstream->format->stream_name, 256);
            }
            fifoSetOutputBuffer(buffer, vgmstream->format->subsong_count * 256);
            free(buffer);
        }

        return vgmstream->format->subsong_count;
    };
    EMSCRIPTEN_KEEPALIVE
    bool getAwb(int subTrack) {
        if (!vgmStreamInit())
            return false;
        if (!awb) return false;

        libvgmstream_open_stream(vgmstream, awb, subTrack);

        size_t totalSize = (vgmstream->format->stream_samples *
            vgmstream->format->channels *
            vgmstream->format->sample_size
        );
        size_t sizeWithHeader = totalSize + sizeof(RiffHeader);

        RiffHeader header{
            .size = sizeWithHeader - 8,
            .channels = static_cast<uint16_t>(vgmstream->format->channels),
            .sampleRate = static_cast<uint32_t>(vgmstream->format->sample_rate),
            .byteRate = static_cast<uint32_t>(vgmstream->format->sample_rate * vgmstream->format->sample_size * vgmstream->format->channels),
            .blockAlign = static_cast<uint16_t>(vgmstream->format->sample_size * vgmstream->format->channels),
            .dataChunkSize = static_cast<uint32_t>(totalSize)
        };

        void* buffer = malloc(sizeWithHeader);
        memcpy(buffer, &header, sizeof(RiffHeader));
        libvgmstream_fill(vgmstream, static_cast<char*>(buffer) + sizeof(RiffHeader), vgmstream->format->stream_samples);

        fifoSetOutputBuffer(buffer, totalSize);

        return true;
    }
}

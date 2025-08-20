//
// Created by Raymond on 19/08/2025.
//

#include <headers.hh>
#include <library/audio/awb.hh>

AWB::AWB(void* buffer, size_t size) {
    this->ptr = malloc(size);
    this->size = size;
    memcpy(this->ptr, buffer, size);

    // C compatibility
    this->read = &AWB::awb_read;
    this->get_size = &AWB::awb_get_size;
    this->get_name = &AWB::awb_get_name;
    this->open = &AWB::awb_open;
    this->close = &AWB::awb_close;

    this->user_data = this;
};
AWB::AWB(void* awbBuffer, size_t awbSize, void* acbBuffer, size_t acbSize)
    : AWB(awbBuffer, awbSize) {
    this->acb = new AWB(acbBuffer, acbSize);
}
AWB::~AWB() {
    free(this->ptr);
}

int AWB::awb_read(void* user_data, uint8_t* dst, int64_t offset, int length) {
    auto self = static_cast<AWB*>(user_data);
    memcpy(dst, static_cast<char*>(self->ptr) + offset, length);
    return length;
};

int64_t AWB::awb_get_size(void* user_data) { return static_cast<AWB*>(user_data)->size; }

const char* AWB::awb_get_name(void* user_data) { return "file.awb"; }

libstreamfile_t* AWB::awb_open(void* user_data, const char* filename) {
    return (strcmp("file.awb", filename) == 0) ? static_cast<libstreamfile_t*>(user_data) :
        (strcmp("file.acb", filename) == 0) ? static_cast<libstreamfile_t*>(static_cast<AWB*>(user_data)->acb) : nullptr;
};

void AWB::awb_close(libstreamfile_t* libsf) {};

bool AWB::hasAcb() {
    return this->acb != nullptr;
}

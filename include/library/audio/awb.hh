//
// Created by Raymond on 19/08/2025.
//

#ifndef AWB_HH
#define AWB_HH

#include <headers.hh>

// gaslight gatekeep girlboss
struct AWB : libstreamfile_t {
    AWB(void* awbBuffer, size_t awbSize);
    AWB(void* awbBuffer, size_t awbSize, void* acbBuffer, size_t acbSize);
    ~AWB();

    bool hasAcb();
private:
    static int awb_read(void* user_data, uint8_t* dst, int64_t offset, int length);
    static int64_t awb_get_size(void* user_data);
    static const char* awb_get_name(void* user_data);
    static libstreamfile_t* awb_open(void* user_data, const char* filename);
    static void awb_close(libstreamfile_t* libsf);

    void* ptr = nullptr;
    size_t size = 0;

    AWB* acb = nullptr;
};

#endif //AWB_HH

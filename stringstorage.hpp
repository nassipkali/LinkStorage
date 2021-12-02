#include "Links/linkstorage.hpp"

class StringStorageASCII {
    private:
        LinkStorage<uint32_t> links;
        std::vector CreateSeq(std::vector seq) {
            size_t size = seq.size();
            if(size % 2 == 0) {
                for(uint32_t i = 0; i < size; i+=2) {
                    links.CreateLink(src[i] + 1, src[i+1] + 1);
                }
            }
            else {
                for(uint32_t i = 0; i < size - 1; i+=2) {
                    links.CreateLink(src[i] + 1, src[i+1] + 1);
                }
                links.CreateLink(src.
            }
        }
    public:
        StringStorageASCII(const char* data_file, const char* index_file) : links(data_file, index_file) {
            for(uint32_t i = 1; i < 129; i++) {
                links.CreateLink(i, i);
            }
        }
        uint32_t CreateLink(std::string src, std::string trg) {
            size_t src_size = src.size();
            if(size % 2 == 0) {
                for(uint32_t i = 0; i < src_size; i+=2) {
                    links.CreateLink(src[i] + 1, src[i+1] + 1);
                }
            }
            else {
                for(uint32_t i = 0; i < src_size - 1; i+=2) {
                    links.CreateLink(src[i] + 1, src[i+1] + 1);
                }
                links.CreateLink(
            }
        }
}

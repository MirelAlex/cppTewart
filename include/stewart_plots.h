#include <vector>
#include "stewart_platform.h"
namespace StewartPlots {

    // utility structure for realtime plot
    struct ScrollingBuffer {
        int MaxSize;
        int Offset;
        ImVector<ImVec2> Data;
        ScrollingBuffer(int max_size = 2000) {
            MaxSize = max_size;
            Offset  = 0;
            Data.reserve(MaxSize);
        }
        void AddPoint(float x, float y) {
            if (Data.size() < MaxSize)
                Data.push_back(ImVec2(x,y));
            else {
                Data[Offset] = ImVec2(x,y);
                Offset =  (Offset + 1) % MaxSize;
            }
        }
        void Erase() {
            if (Data.size() > 0) {
                Data.shrink(0);
                Offset  = 0;
            }
        }
    };

    // std::vector<ScrollingBuffer> angles; // Declare the vector

    void Stewart_RealtimePlots();
    void ShowStewartPlots(bool *p_open);
}
#ifndef CAPABILITY_TREE_BUILDER_H
#define CAPABILITY_TREE_BUILDER_H

#include <capability.h>
#include <format.h>

#include <functional>
#include <unordered_map>
#include <vector>

namespace webcam_capture {

/**
 * Helper class that builds std::vector<CapabilityFormat> out of pixel format, resolution and fps entries.
 */
class CapabilityTreeBuilder
{
public:
    CapabilityTreeBuilder();

    /**
     * Adds pixel format, reolution and fps entries to the builder.
     * @param pixelFormat Format to add.
     * @param width Width part of the resolution a camera supports for captureing using the format.
     * @param height Height part of the resolution a camera supports for captureing using the format.
     * @param fps List of FPS values a camera supports for captureing using the resolution and the format.
     * FPS values don't have to be unique, repeated values will be added once.
     */
    void addCapability(Format pixelFormat, int width, int height, std::vector<int> fps);

    /**
     * Builds the capability tree-like structure out of added entries.
     * @return the tree-like capability structure.
     */
    std::vector<CapabilityFormat> build() const;

private:
    static std::size_t formatHash(const Format &f);
    static std::size_t resolutionHash(const std::pair<int, int> &p);
    static bool resolutionEquals(const std::pair<int, int> &p, const std::pair<int, int> &q);

    typedef std::unordered_map<int, bool>
            FpsMap;

    typedef std::unordered_map<std::pair<int, int>,
            FpsMap,
            std::function<std::size_t(const std::pair<int, int>&)>,
            std::function<bool(const std::pair<int, int>&, const std::pair<int, int>&)>>
            ResolutionMap;

    typedef std::unordered_map<Format, ResolutionMap,
            std::function<std::size_t(const Format&)>>
            FormatMap;

    FormatMap formatMap;

    static const size_t FORMAT_MAP_INITIAL_BUCKET_COUNT;
    static const size_t RESOLUTION_MAP_INITIAL_BUCKET_COUNT;
};

} // namespace webcam_capture

#endif // CAPABILITY_TREE_BUILDER_H

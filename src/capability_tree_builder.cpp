#include "capability_tree_builder.h"

#include "utils.h"

namespace webcam_capture {

const size_t CapabilityTreeBuilder::FORMAT_MAP_INITIAL_BUCKET_COUNT = 5;
const size_t CapabilityTreeBuilder::RESOLUTION_MAP_INITIAL_BUCKET_COUNT = 7;

CapabilityTreeBuilder::CapabilityTreeBuilder() :
    formatMap(FORMAT_MAP_INITIAL_BUCKET_COUNT, formatHash)
{
}

size_t CapabilityTreeBuilder::resolutionHash(const std::pair<int, int> &p)
{
    return std::hash<int>()(p.first + p.second);
}

size_t CapabilityTreeBuilder::formatHash(const Format &f)
{
    return static_cast<std::size_t>(f);
}

bool CapabilityTreeBuilder::resolutionEquals(const std::pair<int, int> &p, const std::pair<int, int> &q)
{
    return p.first == q.first && p.second == q.second;
}

bool CapabilityTreeBuilder::fpsEquals(const float &p, const float &q)
{
    return FPS_EQUAL(p, q);
}

void CapabilityTreeBuilder::addCapability(Format pixelFormat, int width, int height, std::vector<float> fps)
{
    // create a format key-value pair of it doesn't already exist.
    // since the value (ResolutionMap) is not default constuctable (because it's map of a custom type),
    // we can't just use [] and have it constructed for us, as we do below for fpsMap.
    auto formatMapItem = formatMap.find(pixelFormat);

    if (formatMapItem == formatMap.end()) {
        // no such format found, explicitly insert it.
        formatMap[pixelFormat] = ResolutionMap(RESOLUTION_MAP_INITIAL_BUCKET_COUNT, resolutionHash, resolutionEquals);
    }

    FpsMap &fpsMap = formatMap[pixelFormat][std::pair<int, int>(width, height)];

    for (const float &f : fps) {
        fpsMap[f] = true;
    }
}

std::vector<CapabilityFormat> CapabilityTreeBuilder::build() const
{
    std::vector<CapabilityFormat> capFormat;
    capFormat.reserve(formatMap.size());

    for (auto formatMapItem : formatMap) {
        const Format &format               = formatMapItem.first;
        const ResolutionMap &resolutionMap = formatMapItem.second;

        std::vector<CapabilityResolution> capResolutions;
        capResolutions.reserve(resolutionMap.size());

        for (auto resolutionMapItem : resolutionMap) {
            const std::pair<int, int> &resolution = resolutionMapItem.first;
            const FpsMap &fpsMap                  = resolutionMapItem.second;

            std::vector<CapabilityFps> capFps;
            capFps.reserve(fpsMap.size());

            for (auto fpsMapItem : fpsMap) {
                capFps.push_back(CapabilityFps(fpsMapItem.first));
            }

            capResolutions.push_back(CapabilityResolution(resolution.first, resolution.second, std::move(capFps)));
        }

        capFormat.push_back(CapabilityFormat(format, std::move(capResolutions)));
    }

    return capFormat;
}

} // namespace webcam_capture

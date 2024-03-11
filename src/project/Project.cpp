#include "project/Project.h"

namespace mer::project {

core::MediaSourcePtr Project::mediaForClip(const util::Uuid& clipId) const
{
    const ClipPtr clip = findClip(clipId);
    return clip ? media_.find(clip->mediaId()) : nullptr;
}

} // namespace mer::project

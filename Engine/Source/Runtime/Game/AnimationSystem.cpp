#include "pch.h"
#include "AnimationSystem.h"
#include "EngineComponents.h"
#include "GameContext.h"

#include <algorithm>

namespace
{
    template<typename T>
    T SampleKeys(const List<AnimationClip::Key<T>>& keys, float time)
    {
        // A single key is constant. Otherwise clamp at the ends and interpolate between keys.
        if (time <= keys.front().time)
            return keys.front().value;
        if (time >= keys.back().time)
            return keys.back().value;

        const auto next = std::upper_bound(keys.begin(), keys.end(), time,
            [](float t, const auto& key) { return t < key.time; });
        const auto& previous = *(next - 1);
        const float alpha = (time - previous.time) / (next->time - previous.time);
        if constexpr (std::is_same_v<T, glm::quat>)
            return glm::normalize(glm::slerp(previous.value, next->value, alpha));
        else
            return glm::mix(previous.value, next->value, alpha);
    }
}

void AnimationSystem(GameContext& gameContext)
{
    for (const auto& [entity, animator] : gameContext.world.Query<Animator>())
    {
        if (!animator.clip || !animator.skeleton)
            continue;

        const auto& clip = *animator.clip;
        const auto& skeleton = *animator.skeleton;
        auto& pose = animator.pose;
        const float nextTime = animator.currentTime + gameContext.deltaTime;
        animator.currentTime = animator.loop
            ? std::fmod(nextTime, clip.duration)
            : std::min(nextTime, clip.duration);
        const float time = animator.currentTime;

        pose.resize(skeleton.nodes.size());
        for (uint32 i = 0; i < skeleton.nodes.size(); ++i)
            pose[i] = skeleton.nodes[i].defaultLocal;

        // Keys replace the node's local transform; they are not deltas from its default.
        for (const auto& track : clip.tracks)
        {
            pose[track.node] = glm::translate(glm::mat4(1.0f), SampleKeys(track.positions, time)) *
                glm::mat4_cast(SampleKeys(track.rotations, time)) *
                glm::scale(glm::mat4(1.0f), SampleKeys(track.scales, time));
        }

        // Parents precede children, so one pass produces the model-space pose.
        for (uint32 i = 0; i < skeleton.nodes.size(); ++i)
        {
            const int32 parent = skeleton.nodes[i].parent;
            if (parent >= 0)
                pose[i] = pose[parent] * pose[i];
        }
    }
}

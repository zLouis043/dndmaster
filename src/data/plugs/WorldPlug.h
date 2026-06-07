#pragma once
#include "../inspector/Inspection.h"
#include "../../core/ecs/world.h"


template <typename T>
struct TypeCodec<World<T>> {
    static void write(IInspector& ins, const std::string& name, World<T>& world, const PropertyTags& tags) {
        auto buffer = world.flatten();
        ins.resolveProperty(name, buffer, tags);
    }
    static void read(IInspector& ins, const std::string& name, World<T>& world, const PropertyTags& tags) {
        std::vector<std::shared_ptr<T>> buffer;
        ins.resolveProperty(name, buffer, tags);
        world.fill(buffer);
    }
};
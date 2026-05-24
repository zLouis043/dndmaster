#pragma once
#include "../inspector/Inspection.h"
#include "MapDescriptors.h"
#include <string>
#include <vector>
#include <include/core/SkPoint.h>

class IMapElement : public IInspectable {
private:
    std::string m_typeName;

public:
    IMapElement(std::string typeName) : m_typeName(std::move(typeName)) {}
    virtual ~IMapElement() = default;

    std::string getType() const override { return m_typeName; }

    virtual SkPoint getPivot() const = 0;
    virtual void setPivot(SkPoint newPivot) = 0;

    virtual std::vector<DrawDescriptor> getDrawDescriptors() const = 0;
    virtual CollisionDescriptor getCollider() const = 0;
    
    virtual bool customHitTest(SkPoint point, float threshold) const { return false; }
};
#pragma once

#include <memory>

#include "chrono/physics/ChLink.h"

namespace v3d {
class Physics;

class ConstrainLink {
    friend class Physics;

   private:
   protected:
    std::shared_ptr<chrono::ChLinkBase> m_link = nullptr;
    Physics* m_phSystem = nullptr;

   public:
    ConstrainLink(Physics* system) : m_phSystem(system) {};
    virtual ~ConstrainLink() = default;
};

}  // namespace v3d

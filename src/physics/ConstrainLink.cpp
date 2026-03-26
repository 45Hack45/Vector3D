#include "ConstrainLink.h"

#include "physics.h"
#include "physics/utils.hpp"
#include "rigidbody.h"

namespace v3d {
// ConstraintSpringDamper::ConstraintSpringDamper(Physics* phSystem,
//                                                RigidBody& body1,
//                                                RigidBody& body2, bool local,
//                                                const glm::vec3& pos1,
//                                                const glm::vec3& pos2)
//     : ConstraintSpringDamper(phSystem, body1.m_body, body2.m_body, local,
//     pos1,
//                              pos2) {}

ConstraintSpringDamper::ConstraintSpringDamper(
    Physics* phSystem, std::shared_ptr<chrono::ChBody> body1,
    std::shared_ptr<chrono::ChBody> body2, bool local,
    const chrono::ChVector3d pos1, const chrono::ChVector3d pos2)
    : ConstrainLink(phSystem) {
    std::shared_ptr<chrono::ChLinkTSDA> link =
        chrono_types::make_shared<chrono::ChLinkTSDA>();
    static int count = 0;
    link->SetName("constraint_springDamper_" + std::to_string(count++));
    link->Initialize(body1, body2, local, pos1, pos2);
    m_link = link;
    m_linkSpring = link.get();

    m_phSystem->addLink(*this);
}
ConstraintSpringDamper::~ConstraintSpringDamper() {
    m_phSystem->removeLink(*this);
}
}  // namespace v3d

#include "type_topology_verifier.hpp"
#include <sstream>
#include <memory>
#include <vector>

namespace topos {
namespace topology {

bool TypeTopologyVerifier::verify(const types::TopologyType& type) {
    try {
        if (!verifyBasicProperties(type)) {
            return false;
        }
        if (!verifySeparationAxioms(type)) {
            return false;
        }
        if (!verifyCompositeType(type)) {
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        setError(std::string("Verification failed: ") + e.what());
        return false;
    } catch (...) {
        setError("Verification failed: Unknown error");
        return false;
    }
}

bool TypeTopologyVerifier::verifyBasicProperties(const types::TopologyType& type) {
    try {
        if (!type.verifyProperty("empty_set") || !type.verifyProperty("whole_set")) {
            setError("Basic property verification failed: Empty set or whole set property not satisfied");
            return false;
        }
        if (!type.verifyProperty("finite_intersection_closed")) {
            setError("Basic property verification failed: Finite intersection closed property not satisfied");
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Basic property verification failed: " << e.what();
        setError(ss.str());
        return false;
    } catch (...) {
        setError("Basic property verification failed: Unknown error");
        return false;
    }
}

bool TypeTopologyVerifier::verifySeparationAxioms(const types::TopologyType& type) {
    try {
        if (!type.verifyProperty("t0_separation")) {
            setError("Separation axiom verification failed: T0 property not satisfied");
            return false;
        }
        if (type.verifyProperty("requires_t1") && !type.verifyProperty("t1_separation")) {
            setError("Separation axiom verification failed: T1 property not satisfied");
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Separation axiom verification failed: " << e.what();
        setError(ss.str());
        return false;
    } catch (...) {
        setError("Separation axiom verification failed: Unknown error");
        return false;
    }
}

bool TypeTopologyVerifier::verifyCompositeType(const types::TopologyType& type) {
    try {
        if (type.verifyProperty("composite")) {
            const types::Type* baseType = type.getBaseType();
            if (!baseType) {
                setError("Composite type verification failed: Base type not found");
                return false;
            }

            if (auto topologyBase = dynamic_cast<const types::TopologyType*>(baseType)) {
                return verify(*topologyBase);
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Composite type verification failed: " << e.what();
        setError(ss.str());
        return false;
    } catch (...) {
        setError("Composite type verification failed: Unknown error");
        return false;
    }
}

} // namespace topology
} // namespace topos

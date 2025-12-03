#include <ontoflow/core/Logger.hpp>
#include <ontoflow/domain/Registry.hpp>

using namespace of::domain;

int main() {
    LOG(Info) << "================================";
    LOG(Info) << "ontoflow headless (Refactored)";
    LOG(Info) << "================================";

    Registry registry;

    // Legacy logic removed during OntoFlow refactor.
    // New Dataflow architecture test will be implemented here.
    
    return 0;
}
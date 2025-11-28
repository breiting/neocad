#include <gtest/gtest.h>
#include <neocad/domain/Registry.hpp>
#include <neocad/occt/OCCTBackend.hpp>

using namespace nc::domain;
using namespace nc::occt;

// Minimal test to check linking and basic functionality of OCCT backend
// Requires OCCT to be installed and available.

class TestOCCTBackend : public ::testing::Test {
protected:
    OCCTBackend backend;
};

TEST_F(TestOCCTBackend, CreateExtrudedBodyBasic) {
    // Create a simple square profile
    Polygon profile;
    profile.vertices = {
        {0.0, 0.0, 0.0},
        {10.0, 0.0, 0.0},
        {10.0, 10.0, 0.0},
        {0.0, 10.0, 0.0}
    };
    
    double height = 5.0;
    BackendShapeHandle handle = backend.CreateExtrudedBody(profile, height);
    
    EXPECT_GT(handle, 0u);
    
    // Try to export (to dev/null equivalent or temp file) to check if shape is valid
    // We use a dummy filename, export might fail on I/O but shouldn't crash
    // Actually, let's try to export to a temp file if possible, or just assume success if handle > 0
    // For now, just checking handle creation is a good smoke test for OCCT integration.
}

TEST_F(TestOCCTBackend, CreateExtrudedBodyInvalidProfile) {
    Polygon emptyProfile;
    BackendShapeHandle handle = backend.CreateExtrudedBody(emptyProfile, 10.0);
    EXPECT_EQ(handle, 0u); // Should fail
}

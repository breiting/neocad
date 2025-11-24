#pragma once

namespace nc {

/// High-level editor modes, similar to Vim's Normal/Insert/etc.
enum class EditorMode {
    Normal,
    InsertPoint,
    InsertLine,
    InsertCircle,
    InsertSketch,
    Select
};

}  // namespace nc
//

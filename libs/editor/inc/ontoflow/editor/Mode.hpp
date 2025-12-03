#pragma once

namespace of::editor {

/**
 * \brief High-level editor modes, similar to Vim's Normal/Insert/etc.
 *
 * Each mode typically has an associated `ITool` that handles input
 * and defines the editor's behavior in that mode.
 */
enum class EditorMode {
    Normal,         ///< Default mode, no specific tool active.
    InsertPoint,    ///< Mode for inserting individual point entities.
    InsertLine,     ///< Mode for inserting line segments.
    InsertCircle,   ///< Mode for inserting circle entities.
    InsertSketch,   ///< Mode for free-hand sketching or polyline creation.
    Select          ///< Mode for selecting entities.
};

}  // namespace of::editor

#!/usr/bin/env bash

# Usage: ./rename_namespace.sh vis
# Effect: namespace nc { --> namespace nc::vis {
#         namespace nc::X { --> namespace nc::vis::X {

if [ -z "$1" ]; then
    echo "Usage: $0 <subnamespace>"
    exit 1
fi

SUB="$1"

# You MUST have gsed on macOS. If not: brew install gnu-sed
SED="sed"
if command -v gsed >/dev/null 2>&1; then
    SED="gsed"
fi

echo "Using sed: $SED"
echo "Replacing 'namespace nc {' with 'namespace nc::$SUB {'"
echo "Replacing 'namespace nc::X {' with 'namespace nc::$SUB::X {'"

# Process all .hpp / .cpp recursively
find . -type f \( -name "*.hpp" -o -name "*.cpp" \) | while read FILE; do
    echo "Processing: $FILE"
    
    # Case 1: namespace nc {
    $SED -i "s/namespace[[:space:]]\+nc[[:space:]]*{/namespace nc::$SUB {/" "$FILE"
done

echo "Done."

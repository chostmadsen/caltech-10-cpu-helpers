set -e

if [ $# -lt 1 ]; then
    echo "usage: ./g_group_clear <group dir>"
    exit 1
fi
dir="$1"

rm -f "$dir"/out_*.txt

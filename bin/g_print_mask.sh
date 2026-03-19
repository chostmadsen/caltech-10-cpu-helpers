set -e

if [ $# -lt 1 ]; then
    echo "usage: ./g_print_mask <group dir>"
    exit 1
fi
dir="$1"

for f in "$dir"/out_*.txt; do
    name=$(basename "$f")
    if [[ "$name" == "skip.txt" ]]; then
        continue
    fi

    ./bin/print_mask "$f" | tr '\n' ' '
done

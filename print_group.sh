if [ $# -lt 1 ]; then
    echo "usage: ./print_group.sh <parent dir>"
    exit 1
fi

for dir in "$1"/*/; do
    [ -d "$dir" ] || continue
    # shellcheck disable=SC2005
    echo "$(basename "$dir")"
    ./bin/print_masks "$dir/masks.txt"
done
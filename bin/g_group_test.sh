set -e

if [ $# -lt 1 ]; then
    echo "usage: ./g_group_test <group dir>"
    exit 1
fi
dir="$1"

for f in "$dir"/*.txt; do
    name=$(basename "$f")
    if [[ "$name" == "skip.txt" || "$name" == out_*.txt || "$name" == "temp.txt" || "$name" == "full.txt" ]]; then
        continue
    fi

    cat "$dir/skip.txt" > "$dir/temp.txt"
    grep -vxFf "$f" "$dir/full.txt" >> "$dir/temp.txt"

    echo "./bin/group masks/instr.csv $dir/temp.txt $f"
    ./bin/group masks/instr.csv "$dir/temp.txt" "$f"
    rm "$dir/temp.txt"
done

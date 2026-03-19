set -e

if [ $# -lt 1 ]; then
    echo "usage: ./g_group_make <group dir>"
    exit 1
fi
dir="$1"

rm -f "$dir"/out_*.txt
for f in "$dir"/*.txt; do
    name=$(basename "$f")
    if [[ "$name" == "skip.txt" ]]; then
        continue
    fi
    out="$dir/out_$name.txt"

    echo "./bin/group masks/instr.csv $dir/skip.txt $f $out"
    ./bin/group masks/instr.csv "$dir/skip.txt" "$f" "$out"
done

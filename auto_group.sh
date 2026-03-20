set -e

if [ $# -lt 1 ]; then
    echo "usage: ./auto_group.sh <parent dir>"
    exit 1
fi

for dir in "$1"/*/; do
    [ -d "$dir" ] || continue
    rm -f "$dir"/masks.txt
    touch "$dir"/skip.txt
    touch "$dir"/masks.txt
    echo "./bin/group masks/instrs.csv $dir/skip.txt $dir/group.txt $dir/masks.txt"
    ./bin/group "masks/instrs.csv" "$dir"/skip.txt "$dir"/group.txt "$dir"/masks.txt
done

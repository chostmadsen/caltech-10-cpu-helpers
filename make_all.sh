set -e
for f in main/*.c; do
    base=$(basename "$f" .c)
    echo "clang main/$base.c src/*.c -I include -o bin/$base"
    clang "main/$base.c" src/*.c -I include -o "bin/$base"
done

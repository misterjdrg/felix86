DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
(cd $DIR
for file in **/*.c; do

    echo Compiling ${file%.c}.out
    gcc -O3 -I./ -o ${file%.c}.out $file
    strip ${file%.c}.out

    if [[ "$IA32" -eq 1 ]]; then
        echo Compiling ${file%.c}_m32.out
        if gcc -m32 -O3 -I./ -o ${file%.c}_m32.out $file; then
            strip ${file%.c}_m32.out
        else
            echo Failed to compile ${file} in 32bit mode
        fi
    fi
done
)
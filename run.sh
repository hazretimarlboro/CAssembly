rm -f ./*.o
gcc $(find ./source -name "*.c") -lcjson -o program
python3 ./source/Parser.py $1

./program
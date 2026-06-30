mkdir -p ./bin
chmod u+w ./bin
rm -f ./bin/*.bin
touch ./bin/program.bin
rm -f ./*.o
gcc $(find ./source -name "*.c") -o program
python3 ./source/Language.py $1 | python3 ./source/ByteCode.py > ./bin/program.bin
./program ./bin/program.bin
@echo off
gcc .\source\Cpu.c .\source\Instructions.c .\source\Tools.c -o program
python3 ./source/Language.py %1 | python3 ./source/ByteCode.py > ./bin/program.bin
.\program.exe .\bin\program.bin
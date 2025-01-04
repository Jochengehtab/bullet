bullet-utils.exe convert --from text --input output.txt --output data.bin --threads 8
bullet-utils.exe shuffle --input data.bin --mem-used-mb 4096 --output shuffled.bin
del output.txt
del data.bin
pause
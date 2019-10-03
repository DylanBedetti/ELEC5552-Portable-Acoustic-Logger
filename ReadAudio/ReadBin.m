
fileID = fopen('MIC.bin')
A = fread(fileID,"int16")
audiowrite("test.wav",A,44100)
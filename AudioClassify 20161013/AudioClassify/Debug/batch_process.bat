@set "Classify=0"
@rem �Ƿ���࣬��������࣬�ͼ���������������ļ�
@if %Classify% EQU 0 (echo  "======�������������浽�ļ�======")   else    (echo  "=======���ݷ��࣬���������ļ�======")

del��d:\SegFea_Voice.txt
@ test D:\MatlabProg\ClassifyData\speech8k\39woman8k.wav   d:\SegFea_Voice.txt %Classify%
@ test D:\MatlabProg\ClassifyData\speech8k\56man8k.wav   d:\SegFea_Voice.txt  %Classify%
del��d:\SegFea_Song.txt
@ for /r D:\MatlabProg\ClassifyData\song8k/ %%i in (*.wav)  do @test %%i d:\SegFea_Song.txt %Classify%
del d:\SegFea_PureMusic.txt
@ for /r D:\MatlabProg\ClassifyData\pure_music8k/ %%i in (*.wav) do @test %%i d:\SegFea_PureMusic.txt %Classify%

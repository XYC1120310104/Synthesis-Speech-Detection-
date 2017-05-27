@set "Classify=0"
@rem 是否分类，如果不分类，就计算特征存入输出文件
@if %Classify% EQU 0 (echo  "======计算特征，保存到文件======")   else    (echo  "=======数据分类，保存结果到文件======")

del　d:\SegFea_Voice.txt
@ test D:\MatlabProg\ClassifyData\speech8k\39woman8k.wav   d:\SegFea_Voice.txt %Classify%
@ test D:\MatlabProg\ClassifyData\speech8k\56man8k.wav   d:\SegFea_Voice.txt  %Classify%
del　d:\SegFea_Song.txt
@ for /r D:\MatlabProg\ClassifyData\song8k/ %%i in (*.wav)  do @test %%i d:\SegFea_Song.txt %Classify%
del d:\SegFea_PureMusic.txt
@ for /r D:\MatlabProg\ClassifyData\pure_music8k/ %%i in (*.wav) do @test %%i d:\SegFea_PureMusic.txt %Classify%

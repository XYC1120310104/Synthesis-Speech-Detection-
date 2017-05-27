function [ startTime, stopTime, totalTime, Audio, fs ] = getActiveSpeech( audioFile, ADR, LEN, INC)
%GETACTIVESPEECH Compute the start and stop time-stamps from audio file
%   Detailed explanation goes here

% Downsample audio signal
[AudioRaw, Fs]= audioread(audioFile);  
Audio = downsample(AudioRaw, ADR);%½µ²ÉÑù
fs = Fs/ADR;
totalTime = size(AudioRaw, 1) / Fs;

% output the index of start and ending frames
LEN=LEN*fs/1000;
INC=INC*fs/1000;

[startFrameIdx, stopFrameIdx] = vad(Audio, LEN, INC);

startTime   = ( (startFrameIdx - 1) * INC + LEN ) /fs ;    
stopTime    = ( (stopFrameIdx - 1)  * INC + LEN ) /fs ;


% figure('Name', 'Voice', 'NumberTitle', 'off');
% numPts = size(Audio, 1);
% audioTimeAxis = (0 : numPts-1) ./ (1.0*fs);
% 
% hold on;
% plot(audioTimeAxis, Audio);
% plot([startTime startTime], [-1 1], 'g', 'LineWidth', 5);
% plot([stopTime stopTime], [-1 1], 'r', 'LineWidth', 5);
% hold off;
% 
% ax = gca;
% ax.FontSize = 16;
% ax.XLabel.String = 'time (s)';
% ax.XLabel.FontSize = 16;
% ax.YLabel.String = 'Normalized Amplitude';
% ax.YLabel.FontSize = 16;
% grid 'on';

end
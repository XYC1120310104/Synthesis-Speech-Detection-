close all;

%% Input parameters
audioFile = 'D:\testdata_true\12306(3-37s).mp3';
fontSize = 20;


%% Get output parameters after applying downsampling and VAD
ADR = 10;
FrameLen = 300;
StepLen = 210;
[ startTime, stopTime, totalTime, audioOut, fsOut ] = getActiveSpeech( audioFile, ADR, FrameLen, StepLen);


%% Plot audio waveform
figure();
subplot(2,1,1);
hold on;

% Create time axis
numAudioPts = size(audioOut);
time = 0 : ( numAudioPts - 1);
time = (1.0 / fsOut) .* time; 

% Plot downsampled audio data as waveform with start and stop active speech
plot(time, audioOut, 'b', 'LineWidth', 1);
plot([startTime startTime], [-1 1], 'g', 'LineWidth', 5);
plot([stopTime stopTime], [-1 1], 'r', 'LineWidth', 5);
hold off;

% Format axis
ax = gca;
ax.FontSize = fontSize;
ax.Title.String = 'Audio Waveform of "11"';
% ax.XLabel.String = 'Time (s)';
ax.XLim = [0 , 4];
ax.YLabel.String = 'Normalized Amplitude';
ax.YLim = [-0.5 , 0.5];

%% Spectrogram
subplot(2,1,2);
spectrogram(audioOut, 128, 120, 128, fsOut, 'yaxis');
colorbar off;   % Needed to align plot with waveform

ax = gca;
ax.FontSize = fontSize;
ax.Title.String = 'Spectrogram of "11"';
ax.XLabel.String = 'Time (s)';
ax.XLim = [0 , 4];


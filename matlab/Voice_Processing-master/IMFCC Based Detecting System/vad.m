function [x1,x2] = vad(x,FrameLen,FrameInc)
%x1:the start frame index of voice
%x2:the end fram index of voice

% normalize the wave amplitude to [-1,1] 
x = double(x);

x = x/max(abs(x));    % Uncessary, if the raw data is zero mean and has been normalized to [-1,1]


% Compute zero-crossing rate(¹ýÁãÂÊ)
tmp1  = enframe(x(1:end-1), FrameLen, FrameInc);
tmp2  = enframe(x(2:end)  , FrameLen, FrameInc);
signs = (tmp1.*tmp2)<0;
diffs = (tmp1 -tmp2)>0.02;
zcr   = sum(signs.*diffs, 2);

% compute the Amplitude of each segment, use "abs" is more robust than "square"
amp = sum(abs(enframe(filter([1 -0.9375], 1, x), FrameLen, FrameInc)), 2);   
% freqz([1 -0.9375],1,20,96000) % look into the frequency reponse, it is a
% high pass filter to get rid of the low frequency trend of each frame signal
% filter([1 -0.9375] is pre-emphasis


% figure
% plot (amp)
% grid on

%set the Amplitude threshod 
amp1 = 10;  % 10
amp2 = 2;  % 
amp1 = min(amp1, max(amp)/4);        %  Absolute confidential threshold

amp2 = min(amp2, max(amp)/8);        %  Likelihood threshold


% amp1 = 5;
% amp2 = 1;
% amp1 = min(amp1, max(amp)/8);         %  Absolute confidential threshold
% amp2 = min(amp2, max(amp)/16);      %  Likelihood confidential threshold


% cross zeros rate threshod 

zcr2 = 5;
%zcr2 = 100;

maxsilence = 20;    %  silent frame for the judement of voice ending
%maxsilence = 50;    % silent frame for the judement of voice ending

minlen  = 60;       % Asumed the minimum length of continuous voice, otherwise it is judged the noise
%minlen  = 150;       % Asumed the minimum length of continuous voice, otherwise it is judged the noise  (minlen*FrameInc/96000)

status  = 0;
count   = 0;
silence = 0;

x1 = 0; 
x2 = 0;
for n=1:length(zcr)
   goto = 0;
   switch status
   case {0,1}                   % 0 = silence phase, 1 = likelihood begining of the voice
      if amp(n) > amp1          % sure voice begin
         x1 = max(n-count-1,1);
         status  = 2;
         silence = 0;
         count   = count + 1;
      elseif amp(n) > amp2 || ... % maybe voice begin
             zcr(n) > zcr2
         status = 1;
         count  = count + 1;
      else                       % silence phase
         status  = 0;
         count   = 0;
      end
   case 2,                       % 2 = voice phase
      if amp(n) > amp2 || ...     % keep voice
         zcr(n) > zcr2
         count = count + 1;
      else                       % the voice is ending
         silence = silence+1;
         if silence < maxsilence % not enough long silence, still in voice phase
            count  = count + 1;
         elseif count < minlen   % the voice phase is too short. It is abandanted as noise
            status  = 0;
            silence = 0;
            count   = 0;
         else                    % voice phase is end
            status  = 3;
         end
      end
   case 3,
      break;
   end
end   

count = floor(count-silence/8-0.5);
x2 = x1 + count -1;


function feature= modified_group_delay_feature(speech,fs, rho, gamma, num_coeff,segment)

%     speech: wav sample.
%     rho: a parameter to control the shape of modified group delay spectra
%     gamma: a parameter to control the shape of the modified group delay spectra


frame_length = 50; %msec
frame_shift  = 20; %msec

NFFT         = 512;
pre_emph     = true;
do_diff      = true;
do_seg       = true;
%%% Pre-emphasis + framing 
if (pre_emph)
    speech = filter([1 -0.97], 1, speech);
end;
frame_length = round((frame_length/1000)*fs);
frame_shift = round((frame_shift/1000)*fs);
frames = enframe(speech, hamming(frame_length), frame_shift);%x(n)

frame_num    = size(frames, 1);
frame_length = size(frames, 2);
delay_vector = [1:1:frame_length];
delay_matrix = repmat(delay_vector, frame_num, 1);

delay_frames = frames .* delay_matrix;%nx(n)

x_spec = fft(frames', NFFT);
y_spec = fft(delay_frames', NFFT);
x_spec = x_spec(1:NFFT/2+1, :);
y_spec = y_spec(1:NFFT/2+1, :);

temp_x_spec = abs(x_spec);

dct_spec = dct(medfilt1(log(temp_x_spec)+eps, 5));%lift_W=5  modified By Xu
smooth_spec = idct(dct_spec(1:30,:), NFFT/2+1);

grp_phase1 = (real(x_spec).*real(y_spec) + imag(y_spec) .* imag(x_spec)) ./(exp(smooth_spec).^ (2*rho));
%%   modulation
grp_phase = (grp_phase1 ./ abs(grp_phase1)) .* (abs(grp_phase1).^ gamma);
grp_phase = grp_phase ./ (max(max(abs(grp_phase))));%πÈ“ªªØ
grp_phase(isnan(grp_phase)) = 0.0;%nan÷√0

cep = dct(grp_phase);
cep = cep(2:num_coeff+1, :)';
feature = cep;
if (do_diff)
    velocity = zeros(size(cep));
    for l=3:size(cep,1)-2
        velocity(l,:)=-2*cep(l-2,:)-cep(l-1,:)+cep(l+1,:)+2*cep(l+2,:);
    end
    velocity=velocity/3;
    acceleration = zeros(size(cep)); 
    for l=3:size(cep,1)-2
        acceleration(l,:)=-2*velocity(l-2,:)-velocity(l-1,:)+velocity(l+1,:)+2*velocity(l+2,:);
    end
    acceleration=acceleration/3;
    feature=[cep velocity acceleration];
end;
if (do_seg)
    seg_inc=segment/2;
    [n1,d]=size(feature);
    nsize = fix((n1-segment)/seg_inc);
    final_F=zeros(nsize,d);
    for i=1:1:nsize
      final_F(i,:)=mean(feature((i-1)*seg_inc+1:(i-1)*seg_inc+segment,:),1);
    end
    feature = final_F;
end;


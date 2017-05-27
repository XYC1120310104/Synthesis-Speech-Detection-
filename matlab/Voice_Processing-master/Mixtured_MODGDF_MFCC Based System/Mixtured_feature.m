function feature = Mixtured_feature(speech,fs, rho, gamma, num_coeff,segment)
fprintf(1,'Start Extract MFCC...\n');
MFCC = MFCC_feature(speech,fs,segment);
%save('D:\liblinear-2.1\matlab\Mixtured_feature\trainData_MFCC.mat','MFCC');
fprintf(1,'End ExtractMFCC...\n');
fprintf(1,'Start Extract MPDGDF...\n');
MODGDF = modified_group_delay_feature(speech,fs, rho, gamma, num_coeff,segment);
%save('D:\liblinear-2.1\matlab\Mixtured_feature\trainData_MODGDF.mat','MODGDF');
fprintf(1,'End Extract MODGDF...\n');
feature = [MFCC,MODGDF];



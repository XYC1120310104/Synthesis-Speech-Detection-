function model=modelTrain(segment,ADR,do_feat_ext)
%do_feat_ext:if extract trainMFCC
if (do_feat_ext)
    %% Read Data
    fprintf(1,'Start to read trainData...\n');
    Truefiles = dir('D:\traindata_true\');
    Falsefiles = dir('D:\traindata_false\');
    Truefiles = Truefiles(3:end);
    Falsefiles = Falsefiles(3:end);
    y_true=[];
    y_false=[];
    %% Feature Extraction
    FrameLen = 300;
    FrameInc = 210;
    fprintf(1,'Start to get true trainData...\n');
    for fn=1:length(Truefiles)
    fprintf(1,['Start extract from truefile',num2str(fn),'...\n']);
    filepath=fullfile('D:\traindata_true\',Truefiles(fn).name);
    [ start, stop, ~, audioOut, fsOut ] = getActiveSpeech( filepath, ADR, FrameLen, FrameInc);
    y_true=[y_true;audioOut(floor(start*fsOut):floor(stop*fsOut),1)];
    end
    MSPC_true=IMFCC_feature(y_true,fsOut,segment);
    fprintf(1,'End to get true trainData...\n');
    fprintf(1,'Start to get false trainData...\n');
    for fn=1:length(Falsefiles)
    filepath=fullfile('D:\traindata_false\',Falsefiles(fn).name);
    fprintf(1,['Start extract from falsefile',num2str(fn),'...\n']);
    [ start, stop, ~, audioOut, fsOut ] = getActiveSpeech( filepath, ADR, FrameLen, FrameInc);
    y_false=[y_false;audioOut(floor(start*fsOut):floor(stop*fsOut),1)];
    end
    MSPC_false=IMFCC_feature(y_false,fsOut,segment);
    fprintf(1,'End to get false trainData...\n');
    MSPC=[MSPC_true;MSPC_false];
    label=[ones(size(MSPC_true,1),1);zeros(size(MSPC_false,1),1)];
    save('D:\liblinear-2.1\matlab\IMFCC\trainData.mat','MSPC');
    save('D:\liblinear-2.1\matlab\IMFCC\trainLabel.mat','label');
end
load 'D:\liblinear-2.1\matlab\IMFCC\trainData.mat';
load 'D:\liblinear-2.1\matlab\IMFCC\trainLabel.mat';
%% Model Training
%model = ClassificationDiscriminant.fit(MSPC, label);
fprintf(1,'Start modelTraining...\n');
%model=fitcsvm(MSPC,label,'Standardize',true,'KernelFunction','RBF',...
%    'KernelScale','auto');
%train(label,sparse(MSPC), '-s 2 -v 5 -C');
model = train(label,sparse(MSPC),'-s 2 -c 1');
fprintf(1,'End modelTraining...\n');
save('D:\liblinear-2.1\matlab\IMFCC\IMFCC_SVMmodel.mat','model');

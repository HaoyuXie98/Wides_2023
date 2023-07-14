fileID = fopen('test_writing_1.bin', 'rb');
data = fread(fileID, [8, 4000], 'double');
fclose(fileID);

%disp(data);
% Combine every two doubles into a complex number
data_complex = data(1:2:end) + 1j*data(2:2:end);

% Compute the Fourier Transform
fftData = fftshift(fft(data_complex)); %half length

% Generate frequency vector for the x-axis
Fs = 2e8; % Fill in with your sampling frequency, should larger than twice of the highest frequency of the signal %200M
N = length(data_complex);
f = (-Fs/2 : Fs/N : Fs/2-Fs/N);

% Plot the magnitude of the FFT
figure;
plot(f, 10*log10(abs(fftData))); %20log10(abs(fftData))
xlabel('Frequency (Hz)');
ylabel('Amplitude(dBm)');
title('FFT Magnitude');

% Define the crop range by indices
crop_start = 200; % Fill in with your crop_start %use dBm as y-label
crop_range = 400; % Fill in with your crop_range

% Crop the FFT data and frequency vector
cropped_fftData = fftData(crop_start : crop_start + crop_range - 1);
cropped_f = f(crop_start : crop_start + crop_range - 1);

% Plot the cropped magnitude of the FFT
figure;
plot(cropped_f, 10*log10(abs(cropped_fftData)));
xlabel('Frequency (Hz)');
ylabel('Amplitude(dBm)');
title('Cropped FFT Magnitude');

% Do calibration before IFFT: b2b, over the air calibration
% Read B2B - I don't know the format of b2b file
fileID = fopen('test_b2b.bin', 'rb'); % reference signals 
data = fread(fileID, [8, 4000], 'double');
fclose(fileID);

td_b2b = data(1:2:end) + 1j*data(2:2:end);
fd_b2b = fft(td_b2b)/10000*sqrt(2); % why do "/10000*sqrt(2)" here, is it for normalization?

% Read thru - I don't know the format of thru file
fileID = fopen('thru.bin', 'rb');
data = fread(fileID, [8, 4000], 'double');
fclose(fileID);

td_thru = data(1:2:end) + 1j*data(2:2:end);
fd_thru = fft(td_thru)/10000*sqrt(2); % why do "/10000*sqrt(2)" here, is it for normalization?

H_sounder = fd_b2b ./ fd_thru;

cropped_H_sounder = H_sounder(crop_start : crop_start + crop_range - 1);

% B2B Calibration
H = cropped_fftData ./ cropped_H_sounder;


% Plot the cropped magnitude of the FFT after calibration
figure;
plot(cropped_f, 10*log10(abs(H)));
xlabel('Frequency (Hz)');
ylabel('Amplitude(dBm)');
title('Cropped FFT Magnitude after Calibration');

% Perform the IFFT 
ifftData = ifft(H);

% Transform to the delay domain
t = (0:length(ifftData)-1)/Fs; % Generate the time vector


% PDP
PDP = 10*log10(abs(ifftData)); %dBm-delay,dBm,dB, W,mW

figure;
plot(t, PDP);
xlabel('Time (s)');
ylabel('Amplitude(dBm)');
title('Delay Domain Representation');








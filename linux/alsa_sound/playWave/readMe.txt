本程序可以播放wav文件。
wav文件格式很多，本文只能播放PMC格式的wav文件（包括8-bit / 16bit /24bit / 32bit)

wav文件格式请参考：
https://blog.csdn.net/imxiangzi/article/details/80265978
https://www.cnblogs.com/ranson7zop/p/7657874.html


//! -----------  命令格式 ------------- !//
./playWave plughw:0  48k_32bit.wav format_le 0.5 2.0
该命令表示以format_le的格式播放48k_32bit.wav. 其中第1个声道的所有值变为原来的0.5倍；另一个声道的所有值变为原来的2倍。

其中：
第2个参数plughw:0 是播放设备的设备名， 对于jetstream来说，该值为 plughw:1
第3个参数为wav的文件名
第4个参数为设置声卡的格式： FORMAT_LE (包括： SND_PCM_FORMAT_S16_LE / SND_PCM_FORMAT_S24_3LE / SND_PCM_FORMAT_S32_LE) 还是 FORMAT_BE (包括：SND_PCM_FORMAT_S16_BE / SND_PCM_FORMAT_S24_3BE / SND_PCM_FORMAT_S32_BE)
	对于8bit的wav模式，没有LE或BE模式的区别，因为8-bit的大端模式和小端模式是相同的值。
第5个参数为channel 0 的放大系数，该系数不能小于-5或大于5；
第6个参数为channel 1 的放大系数，该系数不能小于-5或大于5；
第7个参数为channel 2 的放大系数，该系数不能小于-5或大于5；
第8个参数为channel 3 的放大系数，该系数不能小于-5或大于5；
...

如果我们设置声卡时使用LE参数，则WAV文件可以直接发送到声卡中，不用任何转换；
如果我们设置声卡时使用BE参数，则WAV文件必须对每个channel上的每个sample都要进行一次小端到大端模式的转换。

该程序可以在intel和ARM等小端字节序的

输出信息：
pzhang@BS04:~/workspace/study/linux/alsa_sound/playWave$ ./playWave plughw:0 48k_24bit.wav format_be 0.5 2.0 
fileName = 48k_24bit.wav
total File Len = 11163504	（wave文件长度）
format block len 16
channel = 2
SamplePerSecond = 48000
BytesPerSecond = 288000		（每秒钟需要288000个字节的内容 = SamplePerSecond * BytesPerSample）
BytesPerSample = 6
BitsPerSample = 24
data Block Len = 11163360
sampleCount = 1860560
Channel 1 will be amplified at 0.500000
Channel 2 will be amplified at 2.000000
rate = 48000
channels = 2
sampleDepthInByte = 3
sampleCount = 1860560		（每个channel所含的采样点总数）
seconds = 38
!!-----------------------------pcm_format = S24_3BE!
PCM name: 'plughw:0'		（播放设备名）
PCM state: PREPARED
channels: 2 (stereo)
rate: 48000 bps
seconds: 38					（总的播放时间）
frames = 256				（每次播放需要256个采样点）	snd_pcm_hw_params_get_period_size(params, &frames, 0);	printf("frames = %ld\n", frames);
period_size = 1536			（所有channel每次播放需要缓冲的空间为1536字节）
period time = 5333			（每次播放所占用的时间，单位为微秒）	snd_pcm_hw_params_get_period_time(params, &tmp, NULL); 	printf("period time = %d\n", tmp);
buffer_size = 16384			(整个播放缓冲区所占用的空间为16384字节)
loops = 7267				

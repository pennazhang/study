本程序可以播放wav文件。
wav文件格式很多，本文只能播放PMC格式的wav文件（包括8-bit / 16bit /24bit / 32bit)

wav文件格式请参考：
https://blog.csdn.net/imxiangzi/article/details/80265978
https://www.cnblogs.com/ranson7zop/p/7657874.html


播放命令：
./playWave Front_Right.wav format_le 0.5 0.6
该命令表示以format_le的格式播放Fribt_Right.wav，其中第1个声道的所有值变为原来的0.5倍；另一个声道的所有值变为原来的2倍。

其中：
第2个参数为wav的文件名
第3个参数为设置声卡的格式： FORMAT_LE (包括： SND_PCM_FORMAT_S16_LE / SND_PCM_FORMAT_S24_3LE / SND_PCM_FORMAT_S32_LE) 还是 FORMAT_BE (包括：SND_PCM_FORMAT_S16_BE / SND_PCM_FORMAT_S24_3BE / SND_PCM_FORMAT_S32_BE)
	对于8bit的wav模式，没有LE或BE模式的区别，因为8-bit的大端模式和小端模式是相同的值。
第4个参数为channel 0 的放大系数，该系数不能小于-5或大于5；
第5个参数为channel 1 的放大系数，该系数不能小于-5或大于5；
第6个参数为channel 2 的放大系数，该系数不能小于-5或大于5；
第7个参数为channel 3 的放大系数，该系数不能小于-5或大于5；
...

如果我们设置声卡时使用LE参数，则WAV文件可以直接发送到声卡中，不用任何转换；
如果我们设置声卡时使用BE参数，则WAV文件必须对每个channel上的每个sample都要进行一次小端到大端模式的转换。

；


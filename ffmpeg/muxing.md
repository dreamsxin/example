# ffmpeg 混音以及音视频混合

## 1. Audio merge

### 1.1 amerge

`-ac 2` speed up

```shell
ffmpeg -i test.mp4 -i test.mp3 -filter_complex "[0:a] [1:a]amerge=inputs=2[aout]" -map "[aout]" -ac 2 mix_amerge.aac
```
PS: Without ac speed will be half.

### 1.2 amix

speed = amerge + ac 2

```shell
ffmpeg -i test.aac -i test.mp3 -filter_complex amix=inputs=2:duration=first:dropout_transition=2 mix.aac
```

## 2. Simple A/V Muxer

`-shortest` : duration = MIN(inputs)

```shell
ffmpeg -i test.mp4 -i test.mp3 -vcodec copy -acodec aac -map 0:v:0 -map 1:a:0 -shortest mix_test.mp4
```

adjust volume

```shell
ffmpeg -i test.mp4 -i test.mp3 -vcodec copy -acodec aac -map 0:v:0 -map 1:a:0 -vol 60 mix_test.mp4
```

## 3. Complex A/V Merge (audio merge)

amerge : duration is shortest

```shell
ffmpeg -i test.mp4 -i test.mp3 -c:v copy -map 0:v:0 -filter_complex "[0:a][1:a]amerge=inputs=2[aout]" -map "[aout]" -ac 2  mix_amerge.mp4
```

amix : setup duration

```shell
ffmpeg -i test.mp4 -i test.mp3 -filter_complex "[0:a][1:a]amix=inputs=2:duration=first[aout]" -map "[aout]" -c:v copy -map 0:v:0 mix_amerge.mp4
```

## 4. Loop shorter iterm

concat shorter one

```shell
ffmpeg -i input_audio -f concat -i <(for i in {1..n}; do printf "file '%s'\n" input_video; done) -c:v copy -c:a copy -shortest output_video
```

## 5. Adjust Volume

audio:

```shell
ffmpeg -i test.aac -i test.mp3 -filter_complex "[0:a]aformat=sample_fmts=fltp:sample_rates=44100:channel_layouts=stereo,volume=0.9[a0]; [1:a]aformat=sample_fmts=fltp:sample_rates=44100:channel_layouts=stereo,volume=0.5[a1]; [a0][a1]amerge=inputs=2[aout]" -map "[aout]" -ac 2 mix_v0.5.aac
```

video:

```shell
ffmpeg -i test.mp4 -i test.mp3 -filter_complex "[0:a]aformat=sample_fmts=fltp:sample_rates=44100:channel_layouts=stereo,volume=0.9[a0]; [1:a]aformat=sample_fmts=fltp:sample_rates=44100:channel_layouts=stereo,volume=0.5[a1]; [a0][a1]amix=inputs=2:duration=first[aout]" -map "[aout]" -ac 2 -c:v copy -map 0:v:0 mix_amerge.mp4
```

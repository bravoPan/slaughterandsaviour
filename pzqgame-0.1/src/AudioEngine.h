#ifndef PZQGAME_AUDIOENGINE_H
#define PZQGAME_AUDIOENGINE_H

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#define SDL_AUDIO_BUFFER_SIZE 1024

struct AudioPacketQueue{
  AVPacketList *first_pkt, *last_pkt;
  int nb_packets;
  int size;
  SDL_cond * cond;
  SDL_mutex * mutex;
  SDL_mutex * workMutex;
};

struct AudioEngine{
  void Initialize();

  SDL_AudioDeviceID devID;
  AVFormatContext * musicFormatCtx;
  int musicStream;
  AVCodecContext * musicCodecCtx;
  AVCodec * musicCodec;
  AudioPacketQueue aQueue;
  SwrContext * swrCtx;
};

int PacketReader(void * eng);
int AudioDecodeFrame(AVCodecContext * audioCodecCtx,Uint8 * audioBuf,int bufSize,AudioPacketQueue * audioQueue,SwrContext * swrCtx);
void AudioCallBack(void * eng,Uint8 * stream,int len);

#endif

#include "common.h"
#include "AudioEngine.h"
#include "GlobalObjects.h"

void AudioEngine::Initialize(){
  int i,ret;

  fmusic = fopen("music.ogg","rb");
  ov_open_callbacks(fmusic,&musicFile,NULL,0,OV_CALLBACKS_NOCLOSE);
  /*
  if((ret = avformat_open_input(&musicFormatCtx,"music.ogg",NULL,NULL)) < 0){return;}
  if((ret = avformat_find_stream_info(musicFormatCtx,NULL)) < 0){return;}
  musicStream = av_find_best_stream(musicFormatCtx,AVMEDIA_TYPE_AUDIO,-1,-1,&musicCodec,0);
  musicCodecCtx = avcodec_alloc_context3(musicCodec);
  avcodec_parameters_to_context(musicCodecCtx,musicFormatCtx->streams[musicStream]->codecpar);
  avcodec_open2(musicCodecCtx,musicCodec,NULL);
  */

  SDL_AudioSpec wantedSpec,spec;
  wantedSpec.freq = 44100;
  wantedSpec.channels = 2;
  wantedSpec.silence = 0;
  wantedSpec.format = AUDIO_S16SYS;
  wantedSpec.samples = SDL_AUDIO_BUFFER_SIZE;
  wantedSpec.callback = AudioCallBack;
  wantedSpec.userdata = this;

  /*
  swrCtx = swr_alloc();
  swrCtx = swr_alloc_set_opts(swrCtx,AV_CH_LAYOUT_STEREO,AV_SAMPLE_FMT_S16,44100,musicCodecCtx->channel_layout,musicCodecCtx->sample_fmt,musicCodecCtx->sample_rate,0,NULL);
  swr_init(swrCtx);
  */
  
  devID = SDL_OpenAudioDevice(NULL,0,&wantedSpec,&spec,0);

  std::memset(&aQueue,0,sizeof(AudioPacketQueue));
  aQueue.cond = SDL_CreateCond();
  aQueue.mutex = SDL_CreateMutex();
  aQueue.workMutex = SDL_CreateMutex();

  SDL_CreateThread(PacketReader,"PacketReader",(void*)this);
  SDL_PauseAudioDevice(devID,0);
}

int PacketReader(void * eng){
  AudioEngine * audioEng = (AudioEngine*)eng;
  AudioPacketQueue * audioQueue = &(audioEng->aQueue);
  SDL_LockMutex(audioQueue->workMutex);
  AudioPacket * pktl;
  int currSection;
  while(!quitGame){
    if(audioQueue->nb_packets >= 100){SDL_CondWait(audioQueue->cond,audioQueue->workMutex);}

    pktl = new AudioPacket;
    int ret = ov_read(&(audioEng->musicFile),pktl->data,4096,0,2,1,&currSection);

    if(ret > 0){
      pktl->len = ret;
      SDL_LockMutex(audioQueue->mutex);
      if(audioQueue->last_pkt == NULL){
	audioQueue->first_pkt = pktl;
      } else {
	audioQueue->last_pkt->next = pktl;
      }
      audioQueue->last_pkt = pktl;
      ++(audioQueue->nb_packets);
      audioQueue->size += pktl->len;
      SDL_UnlockMutex(audioQueue->mutex);
    } else {
      fclose(audioEng->fmusic);
      audioEng->fmusic = fopen("music.ogg","rb");
      ov_open_callbacks(audioEng->fmusic,&(audioEng->musicFile),NULL,0,OV_CALLBACKS_NOCLOSE);
    }
  }
  SDL_UnlockMutex(audioQueue->workMutex);
  return 0;
  /*
  AudioEngine * audioEng = (AudioEngine*)eng;
  AVFormatContext * musicFormatCtx = audioEng->musicFormatCtx;
  int musicStream = audioEng->musicStream;
  AudioPacketQueue * audioQueue = &(audioEng->aQueue);
  AVPacket packet;
  SDL_LockMutex(audioQueue->workMutex);
  while(!quitGame){
    if(audioQueue->nb_packets >= 100){SDL_CondWait(audioQueue->cond,audioQueue->workMutex);}
    int ret = av_read_frame(musicFormatCtx,&packet);
    if(ret < 0){
      av_seek_frame(musicFormatCtx,-1,0,0);
    }
    if(packet.stream_index != musicStream){continue;}
    AVPacketList * pktl;
    pktl = (AVPacketList*)av_malloc(sizeof(AVPacketList));
    av_packet_ref(&(pktl->pkt),&packet);
    pktl->next = NULL;

    SDL_LockMutex(audioQueue->mutex);
    if(audioQueue->last_pkt == NULL){
      audioQueue->first_pkt = pktl;
    } else {
      audioQueue->last_pkt->next = pktl;
    }
    audioQueue->last_pkt = pktl;
    ++(audioQueue->nb_packets);
    audioQueue->size += packet.size;
    SDL_UnlockMutex(audioQueue->mutex);
  }
  SDL_UnlockMutex(audioQueue->workMutex);
  return 0;
  */
}

/*
int AudioDecodeFrame(AVCodecContext * audioCodecCtx,Uint8 * audioBuf,int bufSize,AudioPacketQueue * audioQueue,SwrContext * swrCtx){
  static AVPacket pkt;
  static AVFrame frame;
  static bool hasPacket = false;
  int dataSize = 0;
  int ret;
  if(quitGame){return -1;}
  
  while(!hasPacket){
    if(audioQueue->nb_packets <= 50){
      SDL_CondBroadcast(audioQueue->cond);
    }
    if(audioQueue->nb_packets <= 0){
      return -1;
    }
    SDL_LockMutex(audioQueue->mutex);
    AVPacketList *pktl = audioQueue->first_pkt;
    audioQueue->first_pkt = pktl->next;
    if(pktl->next == NULL){
      audioQueue->last_pkt = NULL;
    }
    --(audioQueue->nb_packets);
    audioQueue->size -= pktl->pkt.size;
    pkt = pktl->pkt;
    avcodec_send_packet(audioCodecCtx,&pkt);
    av_free(pktl);
    ret = avcodec_receive_frame(audioCodecCtx,&frame);
    if(ret >= 0){hasPacket = true;}
    SDL_UnlockMutex(audioQueue->mutex);
  }

  int sampleNum = swr_convert(swrCtx,&audioBuf,bufSize / 2,(const uint8_t**)frame.data,frame.nb_samples);
  int outputLen = av_samples_get_buffer_size(NULL,2,sampleNum,AV_SAMPLE_FMT_S16,1);
  ret = avcodec_receive_frame(audioCodecCtx,&frame);
  if(ret < 0){hasPacket = false;}
  return outputLen;
}
*/

void AudioCallBack(void * eng,Uint8 * stream,int len){
  static char audioBuf[4096];
  AudioEngine * audioEng = (AudioEngine *)eng;
  AudioPacketQueue * audioQueue = &(audioEng->aQueue);
  static int audioBufLen = 0,audioBufPtr = 0;
  int streamPtr = 0,pktPtr;
  while(streamPtr < len && audioBufPtr < audioBufLen){
    stream[streamPtr++] = audioBuf[audioBufPtr++];
  }
  if(audioQueue->nb_packets <= 50){
    SDL_CondBroadcast(audioQueue->cond);
  }
  while(streamPtr < len){
    if(audioQueue->first_pkt == NULL){break;}
    SDL_LockMutex(audioQueue->mutex);
    AudioPacket *pktl = audioQueue->first_pkt;
    audioQueue->first_pkt = pktl->next;
    if(pktl->next == NULL){
      audioQueue->last_pkt = NULL;
    }
    --(audioQueue->nb_packets);
    audioQueue->size -= pktl->len;
    SDL_UnlockMutex(audioQueue->mutex);
    pktPtr = 0;
    while(streamPtr < len && pktPtr < pktl->len){
      stream[streamPtr++] = pktl->data[pktPtr++];
    }
    if(pktPtr < pktl->len){
      audioBufLen = pktl->len - pktPtr;
      audioBufPtr = 0;
      while(audioBufPtr < audioBufLen){
	audioBuf[audioBufPtr++] = pktl->data[pktPtr++];
      }
    }
    delete pktl;
  }
  if(streamPtr < len){
    while(streamPtr < len){
      stream[streamPtr++] = 0;
    }
  }
  /*
  AudioEngine * audioEng = (AudioEngine *)eng;
  static Uint8 audioBuf[300000];
  static unsigned int audioBufSize = 0,audioBufIndex = 0;

  while(len > 0){
    if(audioBufIndex >= audioBufSize){
      int t = AudioDecodeFrame(audioEng->musicCodecCtx,audioBuf,300000,&(audioEng->aQueue),audioEng->swrCtx);
      if(t < 0){
	audioBufSize = 1024;
	memset(audioBuf,0,audioBufSize);
      } else {
	audioBufSize = t;
      }
      audioBufIndex = 0;
    }
    int m = audioBufSize - audioBufIndex;
    if(m > len){m = len;}
    memcpy(stream,audioBuf + audioBufIndex,m);
    len -= m;
    stream += m;
    audioBufIndex += m;
  }
  */
}

//
// Created by zou on 2022/9/14.
//
#include "utils/AndroidLog.h"
#include "FFDemuxingDecoder.h"


FFDemuxingDecoder::FFDemuxingDecoder() {

}

FFDemuxingDecoder::~FFDemuxingDecoder() {

}

static int get_format_from_sample_fmt(const char **fmt,
                                      enum AVSampleFormat sample_fmt)
{
    int i;
    struct sample_fmt_entry {
        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
            { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
            { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
            { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
            { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
            { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
    };
    *fmt = NULL;

    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt) {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }

    ERROR("sample format %s is not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
    return -1;
}

bool FFDemuxingDecoder::Init(const char* url, const std::shared_ptr<RenderBase>& render,
                             const std::shared_ptr<VideoDecoderObserver>& video_decoder_observer) {
    if(!url) return false;
    m_render = render;
    m_video_decoder_observer = video_decoder_observer;
    int size = strlen(url);
    if(m_Url){
        free((void *) m_Url);
        m_Url = nullptr;
    }

    m_Url = static_cast<char *>(malloc(size + 1));

    strcpy(m_Url, url);

    m_format_context = avformat_alloc_context();

    if(avformat_open_input(&m_format_context, m_Url, nullptr, nullptr)){
        ERROR("avformat open input failed, url=%s", m_Url);
        return false;
    }

    if(avformat_find_stream_info(m_format_context, nullptr)){
        ERROR("avformat find stream info failed, url=%s", m_Url);
        return false;
    }

    if(OpenCodecContext(&m_video_stream_index, &m_video_codec_context, m_format_context, AVMEDIA_TYPE_VIDEO)){
        m_video_stream = m_format_context->streams[m_video_stream_index];

        m_frame_params.format = AVPixelFormatToImageFormat(m_video_codec_context->pix_fmt);
        m_frame_params.width = m_video_codec_context->width;
        m_frame_params.height = m_video_codec_context->height;
        INFO("video format:%s, width:%d, height:%d", av_get_pix_fmt_name(m_video_codec_context->pix_fmt), m_frame_params.width, m_frame_params.height);
        if(m_video_decoder_observer){
            m_video_decoder_observer->OnDecoderReady(m_frame_params.width, m_frame_params.height);
        }

    }

    if(OpenCodecContext(&m_audio_stream_index, &m_audio_codec_context, m_format_context, AVMEDIA_TYPE_AUDIO)){
        m_audio_stream = m_format_context->streams[m_audio_stream_index];

        enum AVSampleFormat sfmt = m_audio_codec_context->sample_fmt;
        int n_channels = m_audio_codec_context->ch_layout.nb_channels;
        const char *fmt;

        if (av_sample_fmt_is_planar(sfmt)) {
            const char *packed = av_get_sample_fmt_name(sfmt);
            WARNING(" the sample format the decoder produced is planar "
                   "(%s). This example will output the first channel only.\n",
                   packed ? packed : "?");
            sfmt = av_get_packed_sample_fmt(sfmt);
            n_channels = 1;
        }

        if ((get_format_from_sample_fmt(&fmt, sfmt)) < 0){
            ERROR("can not get audio fmt");
            return false;
        }


        LOGI("audio format:%s, ac:%d, ar:%d", fmt,
             n_channels, m_audio_codec_context->sample_rate);
    }

    if(!m_video_stream){
        WARNING("the url does not has a video stream");
    }

    if(!m_audio_stream){
        WARNING("the url does not has an audio stream");
    }

    m_frame = av_frame_alloc();
    if(!m_frame){
        ERROR("can not allocate frame");
        return false;
    }

    m_packet = av_packet_alloc();
    if(!m_packet){
        ERROR("can not allocate packet");
        return false;
    }

    return true;
}

bool FFDemuxingDecoder::OpenCodecContext(int *stream_idx, AVCodecContext **dec_ctx,
                                 AVFormatContext *fmt_ctx, enum AVMediaType media_type) {
    int ret, stream_index;
    AVStream* stream;
    const AVCodec* dec = nullptr;

    ret = av_find_best_stream(fmt_ctx, media_type, -1, -1, nullptr, 0);
    if(ret < 0){
        ERROR("can not find %s stream in input file", av_get_media_type_string(media_type));
        return false;
    }else{
        stream_index = ret;
        stream = fmt_ctx->streams[stream_index];

        dec = avcodec_find_decoder(stream->codecpar->codec_id);
        if(!dec){
            ERROR("failed to find %s codec", av_get_media_type_string(media_type));
            return false;
        }

        *dec_ctx = avcodec_alloc_context3(dec);
        if(!*dec_ctx){
            ERROR("failed to allocate %s codec context", av_get_media_type_string(media_type));
            return false;
        }

        if( avcodec_parameters_to_context(*dec_ctx, stream->codecpar) < 0){
            ERROR("failed to copy %s codec parameters to decoder context", av_get_media_type_string(media_type));
            return false;
        }

        if( avcodec_open2(*dec_ctx, dec, nullptr) < 0){
            ERROR("failed to open %s codec", av_get_media_type_string(media_type));
            return false;
        }
    }

    *stream_idx = stream_index;

    return true;
}

bool FFDemuxingDecoder::Start() {
    if(m_is_running) return true;

    m_decode_thread = std::thread(&FFDemuxingDecoder::DecodeLoop, this);

    m_is_running = true;
    return true;
}

void FFDemuxingDecoder::DecodeLoop() {
    bool ret;
    while (m_is_running && av_read_frame(m_format_context, m_packet) >=0 ){
        if(m_packet->stream_index == m_video_stream_index){
            ret = DecodeOnePacket(m_video_codec_context, m_packet);
        }else if(m_packet->stream_index == m_audio_stream_index){
            ret = DecodeOnePacket(m_audio_codec_context, m_packet);
        }

        av_packet_unref(m_packet);

        if(!ret) break;
    }
}

bool FFDemuxingDecoder::DecodeOnePacket(AVCodecContext *codec_context, AVPacket *packet) {
    int ret = 0;

    ret = avcodec_send_packet(codec_context, packet);
    if(ret < 0){
        ERROR("failed to submitting a packet for decoding (%s)", av_err2str(ret));
        return false;
    }

    while(ret >= 0){
        ret = avcodec_receive_frame(codec_context, m_frame);
        if(ret < 0){
            if(ret == AVERROR_EOF || ret == AVERROR(EAGAIN)){
                return true;
            }

            ERROR("error during decoding (%s)", av_err2str(ret));
            return false;
        }

        if(codec_context->codec->type == AVMEDIA_TYPE_VIDEO){

            for(int i=0; i<4; ++i){
                m_frame_params.video_raw_data[i] = m_frame->data[i];
                m_frame_params.video_raw_linesize[i] = m_frame->linesize[i];
            }
            m_frame_params.timestamp = m_frame->pts;


            if(m_video_decoder_observer){
                m_video_decoder_observer->OnDecoderOneFrame(&m_frame_params);
            }

            if(m_render){
                m_render->OnDecodedFrame(&m_frame_params);
            }

        }else{
//            size_t unpadded_linesize = m_frame->nb_samples * av_get_bytes_per_sample(
//                    static_cast<AVSampleFormat>(m_frame->format));

        }

        av_frame_unref(m_frame);

        if(ret < 0) return false;
    }


    return true;
}

ImageFormat FFDemuxingDecoder::AVPixelFormatToImageFormat(AVPixelFormat pixelFormat) {
    ImageFormat format = IMAGE_FORMAT_NONE;
    switch (pixelFormat) {
        case AV_PIX_FMT_RGBA:
            format = IMAGE_FORMAT_RGBA;
            break;
        case AV_PIX_FMT_YUV420P:
            format = IMAGE_FORMAT_I420;
            break;
        case AV_PIX_FMT_YUVJ420P:
            format = IMAGE_FORMAT_J420;
            break;
        case AV_PIX_FMT_NV12:
            format = IMAGE_FORMAT_NV12;
            break;
        case AV_PIX_FMT_NV21:
            format = IMAGE_FORMAT_NV21;
            break;
        default:
            format = IMAGE_FORMAT_NONE;
    }

    return format;

}

bool FFDemuxingDecoder::Stop() {
    if(!m_is_running) return true;
    m_is_running = false;
    if(m_decode_thread.joinable())
        m_decode_thread.join();

    if(m_video_decoder_observer){
        m_video_decoder_observer->OnDecoderDone();
    }
    return true;
}

bool FFDemuxingDecoder::Destroy() {
    Stop();

    if(m_video_codec_context){
        avcodec_free_context(&m_video_codec_context);
        m_video_codec_context = nullptr;
    }

    if(m_audio_codec_context){
        avcodec_free_context(&m_audio_codec_context);
        m_audio_codec_context = nullptr;
    }

    if(m_format_context){
        avformat_close_input(&m_format_context);
        m_format_context = nullptr;
    }

    if(m_packet){
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }

    if(m_frame){
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }
    return true;
}





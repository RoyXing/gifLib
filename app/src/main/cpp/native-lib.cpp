#include <jni.h>
#include <string>
#include "gif_lib.h"
#include <android/bitmap.h>
#include <android/log.h>

#define LOG_TAG "roy"
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,FORMAT,##__VA_ARGS__)
#define  argb(a,r,g,b) ( ((a) & 0xff) << 24 ) | ( ((b) & 0xff) << 16 ) | ( ((g) & 0xff) << 8 ) | ((r) & 0xff)

typedef struct GifBean {
    //播放帧数，第几帧
    int current_frame;

    int total_frame;

    //延迟时间数组
    int *delays;

} GifBean;


void drawFrame(GifFileType *gifFileType, GifBean *gifBean, AndroidBitmapInfo info, void *pixels) {
    //当前帧
    SavedImage frame = gifFileType->SavedImages[gifBean->current_frame];
    //整幅图的首地址
    int *px = (int *) pixels;

    //每一行的首地址
    int *line;

    //压缩数据
    int pointPixel;
    GifByteType gifByteType;
    GifColorType gifColorType;
    GifImageDesc imageDesc = frame.ImageDesc;
    //rgb数据 压缩工具
    ColorMapObject *colorMapObject = imageDesc.ColorMap;
    px = (int *) ((char*)px + info.stride * imageDesc.Top);
    for (int y = imageDesc.Top; y < imageDesc.Top + imageDesc.Height; ++y) {
        line=px;
        for (int x = imageDesc.Left; x < imageDesc.Left + imageDesc.Width; ++x) {
            //拿到某一帧坐标的位置
            pointPixel = (y - imageDesc.Top) * imageDesc.Width + (x - imageDesc.Left);

            gifByteType = frame.RasterBits[pointPixel];
            gifColorType = colorMapObject->Colors[gifByteType];
            line[x] = argb(255,gifColorType.Red, gifColorType.Green, gifColorType.Blue);
        }
        px = (int *) ((char*)px + info.stride);
    }
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_xingzy_GifHandler_loadPath(JNIEnv *env, jobject instance, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    int error;
    GifFileType *gifFileType = DGifOpenFileName(path, &error);
    DGifSlurp(gifFileType);

    GifBean *gifBean = (GifBean *) malloc(sizeof(GifBean));
    //清空内存地址
    memset(gifBean, 0, sizeof(GifBean));
    //初始化数组
    gifBean->delays = (int *) malloc(sizeof(int) * gifFileType->ImageCount);
    memset(gifBean->delays, 0, sizeof(int) * gifFileType->ImageCount);

    gifFileType->UserData = gifBean;
    gifBean->current_frame = 0;
    gifBean->total_frame = gifFileType->ImageCount;

    ExtensionBlock *ext = NULL;
    for (int i = 0; i < gifFileType->ImageCount; ++i) {
        SavedImage frame = gifFileType->SavedImages[i];
        for (int j = 0; j < frame.ExtensionBlockCount; ++j) {
            if (frame.ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {
                ext = &frame.ExtensionBlocks[j];
                break;
            }
        }
        if (ext) {
            int frame_delay = 10 * (ext->Bytes[2] << 8 | ext->Bytes[1]);
            LOGE("时间  %d   ",frame_delay);
            gifBean->delays[i] = frame_delay;
        }
    }
    LOGE("gif 长度大小 %d", gifFileType->ImageCount);

    env->ReleaseStringUTFChars(path_, path);
    return (jlong) gifFileType;
}



extern "C"
JNIEXPORT jint JNICALL
Java_com_xingzy_GifHandler_getWidth__J(JNIEnv *env, jobject instance, jlong ndkGif) {

    GifFileType *gifFileType = (GifFileType *) ndkGif;
    return gifFileType->SWidth;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_xingzy_GifHandler_getHeight__J(JNIEnv *env, jobject instance, jlong ndkGif) {

    GifFileType *gifFileType = (GifFileType *) ndkGif;
    return gifFileType->SHeight;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_xingzy_GifHandler_updateFrame__JLandroid_graphics_Bitmap_2(JNIEnv *env, jobject instance,
                                                                    jlong ndkGif, jobject bitmap) {
    GifFileType *gifFileType = (GifFileType *) ndkGif;
    GifBean *gifBean = (GifBean *) gifFileType->UserData;
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, bitmap, &info);

    void *pixels;
    AndroidBitmap_lockPixels(env, bitmap, &pixels);
    drawFrame(gifFileType, gifBean, info, pixels);
    gifBean->current_frame += 1;

    if (gifBean->current_frame >= gifBean->total_frame - 1) {
        gifBean->current_frame = 0;
        LOGE("gif 重新播放 ");
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    return gifBean->delays[gifBean->current_frame];
}
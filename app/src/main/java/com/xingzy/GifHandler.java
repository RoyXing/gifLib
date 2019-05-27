package com.xingzy;

import android.graphics.Bitmap;

/**
 * @author roy.xing
 * @date 2019-05-27
 */
public class GifHandler {

    static {
        System.loadLibrary("native-lib");
    }

    private long gifAddress;

    public GifHandler(String path) {
        gifAddress = loadPath(path);
    }

    public int getWidth() {
        return getWidth(gifAddress);
    }

    public int getHeight() {
        return getHeight(gifAddress);
    }

    public int updateFrame(Bitmap bitmap) {
        return updateFrame(gifAddress, bitmap);
    }

    private native long loadPath(String path);

    private native int getWidth(long ndkGif);

    private native int getHeight(long ndkGif);

    private native int updateFrame(long ndkGif, Bitmap bitmap);
}

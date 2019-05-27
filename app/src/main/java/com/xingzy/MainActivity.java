package com.xingzy;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ImageView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private ImageView imageView;
    private GifHandler gifHandler;
    private Bitmap bitmap;
    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            int nextFrame = gifHandler.updateFrame(bitmap);
            handler.sendEmptyMessageDelayed(0, nextFrame);
            imageView.setImageBitmap(bitmap);
            return false;
        }
    });

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        imageView = findViewById(R.id.imageview);
    }


    public void loadGif(View view) {
        File file = new File(Environment.getExternalStorageDirectory(), "demo.gif");
        gifHandler = new GifHandler(file.getAbsolutePath());
        int width = gifHandler.getWidth();
        int height = gifHandler.getHeight();
        bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        int nextFrame = gifHandler.updateFrame(bitmap);
        handler.sendEmptyMessageDelayed(0, nextFrame);
    }
}

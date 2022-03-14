package com.example.eye_detection;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.example.eye_detection.databinding.ActivityMainBinding;

import org.apache.commons.compress.compressors.bzip2.BZip2CompressorInputStream;


import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    private Button btnCamara;
    private ImageView imgView ;
    private static String TAG = "MainActivity";


    // Used to load the 'eye_detection' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        checkExternalStoragePermission();
        /* ----------------------------------------------------*/
        String p = "/storage/emulated/0/shape_predictor_68_face_landmarks.dat";
        //String p = "/storage/emulated/0/shape_predictor_68_face_landmarks.dat.bz2";

        if(new File(p).exists()){
            if (new File(p).isFile()){
                System.out.println("Es archivo");
            }else {
                System.out.println("no lo es archivo");
            }
            System.out.println("Existe");
        }else {
            System.out.println("No es existe");
        }
        /* ----------------------------------------------------*/
/*
        FileInputStream in = null;
        try {
            in = new FileInputStream("storage/emulated/0/shape_predictor_68_face_landmarks.dat.bz2");
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        FileOutputStream out = null;
        try {
            out = new FileOutputStream("storage/emulated/0/shape_predictor_68_face_landmarks.dat");
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        BZip2CompressorInputStream bzIn = null;
        try {
            bzIn = new BZip2CompressorInputStream(in);
        } catch (IOException e) {
            e.printStackTrace();
        }
        final byte[] buffer = new byte[1024];
        int n = 0;
        while (true) {
            try {
                if (!(-1 != (n = bzIn.read(buffer)))) break;
            } catch (IOException e) {
                e.printStackTrace();
            }
            try {
                out.write(buffer, 0, n);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        try {
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        try {
            bzIn.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
*/

        /* ----------------------------------------------------*/
        btnCamara = findViewById(R.id.btnCamara);
        imgView = findViewById(R.id.imageView);


        btnCamara.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                abrirCamara();
            }
        });



        // Example of a call to a native method
        //TextView tv = binding.sampleText;
        //tv.setText(stringFromJNI());
    }


    private void abrirCamara(){
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        if(intent.resolveActivity(getPackageManager()) != null){
            startActivityForResult(intent, 1);
        }
    }
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1 && resultCode == RESULT_OK) {
            Save savefile = new Save();
            Toast.makeText(this,"Empieza proceso", Toast.LENGTH_SHORT).show();
            Bundle extras = data.getExtras();
            Bitmap imgBitmap = (Bitmap) extras.get("data");
            savefile.SaveImage(this,imgBitmap);


            imgView.setImageBitmap(imgBitmap);
            //imgView.setImageBitmap(convertMat2Bitmap(matoutput));
            String Fullpath = savefile.getAbsolutePath()+"/"+savefile.getAbsoluteName();
            String path = savefile.getAbsolutePath();
            String name = savefile.getAbsoluteName();
            System.out.println(Fullpath);
            String a =loadImages(Fullpath,path,name);
            detectIris(name);
            Toast.makeText(this,"Termina proceso proceso", Toast.LENGTH_SHORT).show();

        }
    }





    //------------- temp ------------------
    private void checkExternalStoragePermission() {
        int permissionCheck = ContextCompat.checkSelfPermission(
                this, Manifest.permission.READ_EXTERNAL_STORAGE);
        if (permissionCheck != PackageManager.PERMISSION_GRANTED) {
            Log.i("Mensaje", "No se tiene permiso para leer.");
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,Manifest.permission.READ_EXTERNAL_STORAGE}, 225);
        } else {
            Log.i("Mensaje", "Se tiene permiso para leer!");
        }
    }
    //----------------------------------------






    /**
     * A native method that is implemented by the 'eye_detection' native library,
     * which is packaged with this application.
     */
    //public native long[] LandmarkDetection(long addrInput,long addrOutput);
    public  native  String loadImages(String Fullpath,String path,String name);
    public native void detectIris(String name);
}
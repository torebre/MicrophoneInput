package com.kjipo.microphoneinput

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.media.AudioDeviceCallback
import android.media.AudioDeviceInfo
import android.media.AudioManager
import android.os.Bundle
import android.support.v4.app.ActivityCompat
import android.support.v7.app.AppCompatActivity
import android.util.Log
import kjipo.com.microphoneinput.R
import kotlinx.android.synthetic.main.activity_main.*
import java.io.BufferedInputStream
import java.io.File
import java.io.FileInputStream
import java.nio.Buffer
import java.nio.ByteBuffer

class MainActivity : AppCompatActivity() {
    private lateinit var audioManager: AudioManager
    private var selectedInputDevice = 0

    private lateinit var pipePath: File


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

//        pipePath = applicationContext.filesDir.resolve ("/data/com.kjipo.microphoneinput/record_pipe")
        pipePath = File ("/data/data/com.kjipo.microphoneinput/record_pipe")



        val recordPermissionGranted = ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) === PackageManager.PERMISSION_GRANTED;
        if (!recordPermissionGranted) {
            requestRecordPermission()
        }

        setupAudioDeviceCallback()

        btnStart.setOnClickListener { startRecording() }
        btnStop.setOnClickListener { stopRecording() }

    }


    private fun setupAudioDeviceCallback() {
//        audioManager = applicationContext.getSystemService(Context.AUDIO_SERVICE) as AudioManager
//        audioManager.registerAudioDeviceCallback(object : AudioDeviceCallback() {
//
//            override fun onAudioDevicesAdded(addedDevices: Array<AudioDeviceInfo>) {
//                for (device in addedDevices) {
//                    if (device.type == AudioDeviceInfo.TYPE_BUILTIN_MIC) {
//                        if (device.id != selectedInputDevice) {
//                            selectedInputDevice = device.id
//                            MicrophoneRecording.setRecordingDeviceId(selectedInputDevice)
//                        }
//
//                    }
//                }
//
//            }
//
//            override fun onAudioDevicesRemoved(removedDevices: Array<AudioDeviceInfo>) {
//                // TODO
//            }
//        }, null)

        // TODO Does this mean auto-select?
        MicrophoneRecording.setRecordingDeviceId(0)
    }


    private fun startRecording() {
        val recordPermissionGranted = ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) === PackageManager.PERMISSION_GRANTED
        if (!recordPermissionGranted) {
            requestRecordPermission()
            return
        }

        MicrophoneRecording.startRecording()

        val testThread = Thread {
            val pipeFile = File(pipePath.absolutePath)

            while(!pipeFile.exists()) {
                Thread.sleep(500)
                Log.i("Main", "Pipe file does not exist")
            }

            Log.i("Main", "Found pipe file")

            val outputStream = FileInputStream(pipePath)

            var readByte = outputStream.read()
            while (readByte != -1) {

                Log.i("Main", "Read byte: $readByte")

                readByte = outputStream.read()
            }

        }
        testThread.start()





    }

    private fun stopRecording() {
        MicrophoneRecording.stop()

    }


    private fun requestRecordPermission() {
        ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.RECORD_AUDIO),
                0)
    }

//    /**
//     * A native method that is implemented by the 'native-lib' native library,
//     * which is packaged with this application.
//     */
//    external fun stringFromJNI(): String
//
//    companion object {
//
//        // Used to load the 'native-lib' library on application startup.
//        init {
//            System.loadLibrary("native-lib")
//        }
//    }

    override fun onResume() {
        super.onResume()
        MicrophoneRecording.create()
    }

    override fun onPause() {
        MicrophoneRecording.stop()
        super.onPause()
    }


}

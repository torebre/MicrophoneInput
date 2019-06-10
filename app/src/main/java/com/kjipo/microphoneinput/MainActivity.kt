package com.kjipo.microphoneinput

import android.Manifest
import android.content.pm.PackageManager
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
        MicrophoneRecording.initializeEssentia()

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
            val bytes = ByteArray(4) {
                0
            }
            var counter = 0


            val pipeFile = File(pipePath.absolutePath)

            while(!pipeFile.exists()) {
                Thread.sleep(500)
                Log.i("Main", "Pipe file does not exist")
            }

            Log.i("Main", "Found pipe file")

            val inputStream = FileInputStream(pipePath)

            val bufferedInputStream = BufferedInputStream(inputStream)


            while(true) {
                val read = inputStream.read(bytes)
                if(read != 4) {
                    break
                }
                bytes.reverse()
                val buffer = ByteBuffer.wrap(bytes)
                val readInt = buffer.int

                val pitchValue = java.lang.Float.intBitsToFloat(readInt)
                Log.i("Main", "Pitch: ${pitchValue}. Integer: $readInt")
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

    override fun onResume() {
        super.onResume()
        MicrophoneRecording.create("/data/data/com.kjipo.microphoneinput/record_pipe", "/data/data/com.kjipo.microphoneinput/record_pipe_confidence")
    }

    override fun onPause() {
        MicrophoneRecording.stop()
        super.onPause()
    }


}

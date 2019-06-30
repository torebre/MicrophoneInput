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
import java.io.File
import java.io.FileInputStream
import java.nio.ByteBuffer
import java.text.NumberFormat

class MainActivity : AppCompatActivity() {
    private lateinit var audioManager: AudioManager
    private var selectedInputDevice = 0

    private lateinit var pitchPipeFile: File
    private lateinit var certaintyPipeFile: File


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

//        pitchPipeFile = applicationContext.filesDir.resolve ("/data/com.kjipo.microphoneinput/record_pipe")

//        Log.i("Test23", "Files dir: ${applicationContext.filesDir}")

        pitchPipeFile = File("/data/data/com.kjipo.microphoneinput/record_pipe")
        certaintyPipeFile = File("/data/data/com.kjipo.microphoneinput/record_pipe_confidence")


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


    var readPitchThread: Thread? = null


    private fun startRecording() {
        val recordPermissionGranted = ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) === PackageManager.PERMISSION_GRANTED
        if (!recordPermissionGranted) {
            requestRecordPermission()
            return
        }

        MicrophoneRecording.startRecording()

        val pipeFile = File(pitchPipeFile.absolutePath)
        val certaintyFile = File(certaintyPipeFile.absolutePath)

        while (!pipeFile.exists() || !certaintyFile.exists()) {
            Thread.sleep(500)
            Log.i("Main", "Pipe file does not exist")
        }

        readPitchThread = Thread {
            val bytes = ByteArray(4) {
                0
            }

            Log.i("Main", "Found pipe file")

            val pitchInputStream = FileInputStream(pitchPipeFile)
            val certaintyInputStream = FileInputStream(certaintyPipeFile)

            while (true) {
                // Read 4 bytes to get a float representing a pitch value
                val read = pitchInputStream.read(bytes)
                if (read != 4) {
                    break
                }
                bytes.reverse()
                val buffer = ByteBuffer.wrap(bytes)
                val readInt = buffer.int

                val pitchValue = java.lang.Float.intBitsToFloat(readInt)
                Log.i("Main", "Pitch: ${pitchValue}. Integer: $readInt")


                // Read 4 bytes to get a float representing the certainty of the pitch
                val read2 = certaintyInputStream.read(bytes)
                if (read2 != 4) {
                    break
                }
                bytes.reverse()
                val buffer2 = ByteBuffer.wrap(bytes)
                val readInt2 = buffer2.int

                val certaintyValue = java.lang.Float.intBitsToFloat(readInt2)
//                Log.i("Main", "Pitch: ${pitchValue}. Integer: $readInt")

                runOnUiThread { txtPitch.setText(NumberFormat.getInstance().format(pitchValue)) }
                runOnUiThread { txtCertainty.setText(NumberFormat.getInstance().format(certaintyValue)) }
            }

        }
        readPitchThread?.start()

    }

    private fun stopRecording() {
        readPitchThread?.interrupt()
        readPitchThread = null
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

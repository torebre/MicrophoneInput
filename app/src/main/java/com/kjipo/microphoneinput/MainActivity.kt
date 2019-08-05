package com.kjipo.microphoneinput

import android.Manifest
import android.content.pm.PackageManager
import android.media.AudioManager
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProviders
import kjipo.com.microphoneinput.R
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.File
import java.io.FileInputStream
import java.nio.ByteBuffer
import java.text.NumberFormat
import java.time.Instant

class MainActivity : AppCompatActivity() {
    private lateinit var audioManager: AudioManager
    private lateinit var model: ColourWheelModel
    private var selectedInputDevice = 0

    private val mainThreadScope = CoroutineScope(Dispatchers.Main)

    private lateinit var pitchPipeFile: File
    private lateinit var certaintyPipeFile: File


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        pitchPipeFile = applicationContext.filesDir.resolve("record_pipe").absoluteFile
        certaintyPipeFile = applicationContext.filesDir.resolve("record_pipe_confidence").absoluteFile

        val recordPermissionGranted = ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) === PackageManager.PERMISSION_GRANTED;
        if (!recordPermissionGranted) {
            requestRecordPermission()
        }

        setupAudioDeviceCallback()

        btnStart.setOnClickListener { startRecording() }
        btnStop.setOnClickListener { stopRecording() }

        var lastUpdate = System.currentTimeMillis()

        model = ViewModelProviders.of(this)[ColourWheelModel::class.java]
        model.inputData.observe(this, Observer<Pair<Double, Double>> {
            updatedDate ->
            val now = System.currentTimeMillis()
            if(now - lastUpdate > 2000) {
                viewColourWheel.updateHighlight(updatedDate)
                lastUpdate = now
            }

        })
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

            var lastUpdate = System.currentTimeMillis()

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

                // Read 4 bytes to get a float representing the certainty of the pitch
                val read2 = certaintyInputStream.read(bytes)
                if (read2 != 4) {
                    break
                }
                bytes.reverse()
                val buffer2 = ByteBuffer.wrap(bytes)
                val readInt2 = buffer2.int
                val certaintyValue = java.lang.Float.intBitsToFloat(readInt2)

//                runOnUiThread { txtPitch.setText(NumberFormat.getInstance().format(pitchValue)) }
//                runOnUiThread { txtCertainty.setText(NumberFormat.getInstance().format(certaintyValue)) }

//                runOnUiThread { viewColourWheel.updateHighlight(Pair(pitchValue.toDouble(), certaintyValue.toDouble())) }

                val now = System.currentTimeMillis()
                if(now - lastUpdate > 1000) {
                mainThreadScope.launch {
                    txtPitch.setText(NumberFormat.getInstance().format(pitchValue))
                    txtCertainty.setText(NumberFormat.getInstance().format(certaintyValue))




                        viewColourWheel.updateHighlight(Pair(pitchValue.toDouble(), certaintyValue.toDouble()))
                        lastUpdate = now
                    }




//                    model.setCurrentData(Pair(pitchValue.toDouble(), certaintyValue.toDouble()))
                }


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
        MicrophoneRecording.create(pitchPipeFile.absolutePath, certaintyPipeFile.absolutePath)
    }


    override fun onPause() {
        MicrophoneRecording.stop()
        super.onPause()
    }

//    suspend fun updateColourWheel() = withContext(Dispatchers.Main) {
//
//        // TODO
//
//
//    }


}

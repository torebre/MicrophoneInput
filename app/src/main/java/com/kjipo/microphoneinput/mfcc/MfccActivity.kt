package com.kjipo.microphoneinput.mfcc

import android.media.AudioManager
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModelProviders
import kjipo.com.microphoneinput.R
import kotlinx.android.synthetic.main.activity_mfcc.*

class MfccActivity : AppCompatActivity() {
    private lateinit var audioManager: AudioManager
    private lateinit var model: SpectrumGraphModel

    private var currentlyRecording = false


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_mfcc)

//        setupAudioDeviceCallback()

        // TODO Does this mean auto-select?
        MfccLibrary.setRecordingDeviceId(0)
        MfccLibrary.create()

        btnMfccStart.setOnClickListener {
            MfccLibrary.testCallback()
            record()
        }

        model = ViewModelProviders.of(this)[SpectrumGraphModel::class.java]
        model.lineData.observe(this, { lineData ->
                spectrumGraph.updateData(lineData)
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
//        MicrophoneRecording.setRecordingDeviceId(0)
    }


    private fun record() {
        if (!currentlyRecording) {
            Log.i("MfccActivity", "Starting recording")
            MfccLibrary.start()
            btnMfccStart.text = resources.getString(R.string.stop_mfcc)
        } else {
            Log.i("MfccActivity", "Stopping recording")
            MfccLibrary.stop()
            btnMfccStart.text = resources.getString(R.string.start_mfcc)
        }
        currentlyRecording != currentlyRecording
    }


}

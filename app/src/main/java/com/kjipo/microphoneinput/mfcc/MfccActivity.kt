package com.kjipo.microphoneinput.mfcc

import android.media.AudioManager
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.kjipo.microphoneinput.MicrophoneRecording
import com.kjipo.microphoneinput.colourwheel.ColourWheelModel
import kjipo.com.microphoneinput.R
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.activity_mfcc.*

class MfccActivity : AppCompatActivity() {
    private lateinit var audioManager: AudioManager
    private lateinit var model: ColourWheelModel

    private var currentlyRecording = false




    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_mfcc)

//        setupAudioDeviceCallback()

        // TODO Does this mean auto-select?
        MicrophoneRecording.setRecordingDeviceId(0)

        MfccLibrary.create()

        btnMfccStart.setOnClickListener {
//            MfccLibrary.testCallback()
            record() }

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
        if (currentlyRecording) {
           MfccLibrary.start()
            btnMfccStart.text = resources.getString(R.string.start_mfcc)
        } else {
            MfccLibrary.stop()
            btnMfccStart.text = resources.getString(R.string.stop_mfcc)
        }
        currentlyRecording != currentlyRecording
    }

    override fun onResume() {
        super.onResume()
        // TODO
    }


    override fun onPause() {
//        MicrophoneRecording.stop()
        super.onPause()
    }


}

package com.kjipo.microphoneinput

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import com.kjipo.microphoneinput.colourwheel.ColourWheelActivity
import com.kjipo.microphoneinput.mfcc.MfccActivity
import kjipo.com.microphoneinput.R
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)


        val recordPermissionGranted = ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED
        if (!recordPermissionGranted) {
            requestRecordPermission()
        }

        btnStartColourWheel.setOnClickListener {
            startActivity(Intent(this, ColourWheelActivity::class.java))
        }

        btnStartMfcc.setOnClickListener {
            startActivity(Intent(this, MfccActivity::class.java))
        }

    }



    private fun requestRecordPermission() {
        ActivityCompat.requestPermissions(
            this,
            arrayOf(Manifest.permission.RECORD_AUDIO),
            0)
    }

}

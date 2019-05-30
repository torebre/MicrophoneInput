package com.kjipo.microphoneinput;

public enum MicrophoneRecording {

    INSTANCE;


    static {
        System.loadLibrary("native-lib");
    }

    static native boolean create();
    static native void startRecording();
    static native void stop();
    static native void setRecordingDeviceId(int deviceId);


}

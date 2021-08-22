package com.kjipo.microphoneinput;

public enum MicrophoneRecording {

    INSTANCE;


    static {
        System.loadLibrary("native-lib");
    }

    public static native boolean create(String pitchFifo, String pitchConfidenceFifo);
    public static native void startRecording();
    public static native void stop();
    public static native void setRecordingDeviceId(int deviceId);


}

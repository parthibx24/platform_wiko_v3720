/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

package com.android.settings.fingerprint;

import android.annotation.Nullable;
import android.app.Activity;
import android.content.Intent;
import android.hardware.fingerprint.FingerprintManager;
import android.os.Bundle;
import android.os.CancellationSignal;
import android.os.Handler;
import android.os.UserHandle;

import com.android.internal.logging.MetricsProto.MetricsEvent;
import com.android.settings.ChooseLockSettingsHelper;
import com.android.settings.InstrumentedFragment;
import com.android.settings.R;

//import android.util.Log;
import com.ape.emFramework.Log;
import com.ape.emFramework.EmFrameworkStatic;
/**
 * Sidecar fragment to handle the state around fingerprint enrollment.
 */
public class FingerprintEnrollSidecar extends InstrumentedFragment {
    private static final String TAG = "FingerprintEnrollSidecar-tag";
	
    private int mEnrollmentSteps = -1;
    private int mEnrollmentRemaining = 0;
    private Listener mListener;
    private boolean mEnrolling;
    private CancellationSignal mEnrollmentCancel;
    private Handler mHandler = new Handler();
    private byte[] mToken;
    private boolean mDone;
    private int mUserId;
    private FingerprintManager mFingerprintManager;

    public static final int FINGERPRINT_ACQUIRED_ENROLL_DUPLICATE= 6; 
    public static final int FINGERPRINT_ACQUIRED_ENROLL_TOO_NEARBY = 7;  

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRetainInstance(true);
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        mFingerprintManager = activity.getSystemService(FingerprintManager.class);
        mToken = activity.getIntent().getByteArrayExtra(
                ChooseLockSettingsHelper.EXTRA_KEY_CHALLENGE_TOKEN);
        mUserId = activity.getIntent().getIntExtra(Intent.EXTRA_USER_ID, UserHandle.USER_NULL);
    }

    @Override
    public void onStart() {
        super.onStart();
        if (!mEnrolling) {
            boolean isOn = EmFrameworkStatic.isScreenON(getActivity());
            boolean isLock= EmFrameworkStatic.isKeyguardLocked(getActivity());
            if (isOn && !isLock) {
                startEnrollment();
            }
            else {
                Log.i(TAG, "-err!--onStart----isOn:"+isOn+" isLock:"+isLock);
            }
        }
    }
 
    @Override
    public void onStop() {
        super.onStop();
        if (!getActivity().isChangingConfigurations()) {
            boolean isOn = EmFrameworkStatic.isScreenON(getActivity());
            boolean isLock= EmFrameworkStatic.isKeyguardLocked(getActivity());
            if (isOn && !isLock) {
                cancelEnrollment();
            }
            else {
                Log.i(TAG, "-err!--onStop----isOn:"+isOn+" isLock:"+isLock);
            }
        }
    }

    private void startEnrollment() {
        mHandler.removeCallbacks(mTimeoutRunnable);
        mEnrollmentSteps = -1;
        mEnrollmentCancel = new CancellationSignal();
        if (mUserId != UserHandle.USER_NULL) {
            mFingerprintManager.setActiveUser(mUserId);
        }
        mFingerprintManager.enroll(mToken, mEnrollmentCancel,
                0 /* flags */, mUserId, mEnrollmentCallback);
        mEnrolling = true;
    }

    boolean cancelEnrollment() {
        mHandler.removeCallbacks(mTimeoutRunnable);
        if (mEnrolling) {
            mEnrollmentCancel.cancel();
            mEnrolling = false;
            mEnrollmentSteps = -1;
            return true;
        }
        return false;
    }

    public void setListener(Listener listener) {
        mListener = listener;
    }

    public int getEnrollmentSteps() {
        return mEnrollmentSteps;
    }

    public int getEnrollmentRemaining() {
        return mEnrollmentRemaining;
    }

    public boolean isDone() {
        return mDone;
    }

    private FingerprintManager.EnrollmentCallback mEnrollmentCallback
            = new FingerprintManager.EnrollmentCallback() {

        @Override
        public void onEnrollmentProgress(int remaining) {
            Log.d("Enrolling", "onEnrollmentProgress remaining = " + remaining);       
            if (mEnrollmentSteps == -1) {
                mEnrollmentSteps = remaining;
            }
            mEnrollmentRemaining = remaining;
            mDone = remaining == 0;
            if (mListener != null) {
                mListener.onEnrollmentProgressChange(mEnrollmentSteps, remaining);
            }
        }

        @Override
        public void onEnrollmentHelp(int helpMsgId, CharSequence helpString) {
            if (getActivity() == null || getFragmentManager() == null ||getFragmentManager().isDestroyed()) {
                Log.e("Enrolling", "onEnrollmentHelp err! ");
                return;
            }
            if (helpMsgId == FINGERPRINT_ACQUIRED_ENROLL_DUPLICATE) {
            helpString = getResources().getString(R.string.fingerprint_acquired_enroll_duplicate);
            } 
            else if (helpMsgId == FINGERPRINT_ACQUIRED_ENROLL_TOO_NEARBY) {
            helpString = getResources().getString(R.string.fingerprint_acquired_enroll_too_nearby);
            }
            Log.d("Enrolling", "onEnrollmentHelp helpMsgId = " + helpMsgId  + "| helpString = " + helpString);
            if (mListener != null) {
                mListener.onEnrollmentHelp(helpString);
            }
        }

        @Override
        public void onEnrollmentError(int errMsgId, CharSequence errString) {
            Log.d("Enrolling", "onEnrollmentError errMsgId = " + errMsgId  + "| errString = " + errString);
            if (mListener != null) {
                mListener.onEnrollmentError(errMsgId, errString);
            }
            mEnrolling = false;
        }
    };

    private final Runnable mTimeoutRunnable = new Runnable() {
        @Override
        public void run() {
            cancelEnrollment();
        }
    };

    @Override
    protected int getMetricsCategory() {
        return MetricsEvent.FINGERPRINT_ENROLL_SIDECAR;
    }

    public interface Listener {
        void onEnrollmentHelp(CharSequence helpString);
        void onEnrollmentError(int errMsgId, CharSequence errString);
        void onEnrollmentProgressChange(int steps, int remaining);
    }

    public boolean isEnrolling() {
        return mEnrolling;
    }
}

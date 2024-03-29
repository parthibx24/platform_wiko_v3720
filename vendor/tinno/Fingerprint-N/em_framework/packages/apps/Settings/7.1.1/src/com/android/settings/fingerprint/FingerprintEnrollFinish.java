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

import android.content.Context;
import android.content.Intent;
import android.hardware.fingerprint.FingerprintManager;
import android.os.Bundle;
import android.os.UserHandle;
import android.view.View;
import android.widget.Button;

import com.android.internal.logging.MetricsProto.MetricsEvent;
import com.android.settings.R;

/**
 * Activity which concludes fingerprint enrollment.
 */
public class FingerprintEnrollFinish extends FingerprintEnrollBase {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //TINNO BEGIN
        //start by lipeng for myos_ui
        if (isMyosUi){
            setContentView(R.layout.myos_fingerprint_enroll_finish_base);
        }else {
            setContentView(R.layout.fingerprint_enroll_finish);
        }
        //TINNO END
        setHeaderText(R.string.security_settings_fingerprint_enroll_finish_title);
        Button addButton = (Button) findViewById(R.id.add_another_button);

        FingerprintManager fpm = (FingerprintManager) getSystemService(Context.FINGERPRINT_SERVICE);
        int enrolled = fpm.getEnrolledFingerprints(mUserId).size();
        int max = getResources().getInteger(
                com.android.internal.R.integer.config_fingerprintMaxTemplatesPerUser);
        if (enrolled >= max) {
            /* Don't show "Add" button if too many fingerprints already added */
            addButton.setVisibility(View.INVISIBLE);
        } else {
            addButton.setOnClickListener(this);
        }
    }

    @Override
    protected void onNextButtonClick() {
        setResult(RESULT_FINISHED);
        finish();
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.add_another_button) {
            final Intent intent = getEnrollingIntent();
            intent.addFlags(Intent.FLAG_ACTIVITY_FORWARD_RESULT);
            startActivity(intent);
            finish();
        }
        super.onClick(v);
    }

    @Override
    protected int getMetricsCategory() {
        return MetricsEvent.FINGERPRINT_ENROLL_FINISH;
    }
}

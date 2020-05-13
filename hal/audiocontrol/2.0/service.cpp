/*
 * Copyright (C) 2020 The Android Open Source Project
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
 * limitations under the License.
 */
#include <unistd.h>

#include <android-base/logging.h>
#include <android/binder_process.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/Errors.h>
#include <utils/Looper.h>
#include <utils/StrongPointer.h>

#include "AudioControl.h"
#include "WatchdogClient.h"

// libhidl:
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

// Generated HIDL files
using android::hardware::automotive::audiocontrol::V2_0::IAudioControl;

using android::Looper;
using android::OK;
using android::hardware::automotive::audiocontrol::V2_0::implementation::AudioControl;
using android::hardware::automotive::audiocontrol::V2_0::implementation::WatchdogClient;

// Main service entry point
int main() {
    // Create an instance of our service class
    android::sp<AudioControl> service = new AudioControl();
    configureRpcThreadpool(4, false /*callerWillJoin*/);

    if (service->registerAsService() != OK) {
        LOG(ERROR) << "registerAsService failed";
        return 1;
    }

    // Setup a binder thread pool to be a car watchdog client.
    ABinderProcess_setThreadPoolMaxThreadCount(1);
    ABinderProcess_startThreadPool();
    android::sp<Looper> looper(Looper::prepare(0 /* opts */));
    std::shared_ptr<WatchdogClient> watchdogClient =
            ndk::SharedRefBase::make<WatchdogClient>(looper, service.get());
    if (!watchdogClient->initialize()) {
        ALOGE("Failed to initialize car watchdog client");
        return 1;
    }

    while (true) {
        looper->pollAll(-1 /* timeoutMillis */);
    }

    // We don't ever actually expect to return, so return an error if we do get here
    return 2;
}

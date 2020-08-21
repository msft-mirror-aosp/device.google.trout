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
#include <cutils/properties.h>
#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>

#include <sstream>

#include "DumpstateDevice.h"

#include <vsockinfo.h>

using ::android::OK;
using ::android::sp;
using ::android::hardware::configureRpcThreadpool;
using ::android::hardware::joinRpcThreadpool;
using ::android::hardware::automotive::utils::VsockConnectionInfo;
using ::android::hardware::dumpstate::V1_1::IDumpstateDevice;
using ::android::hardware::dumpstate::V1_1::implementation::makeVirtualizationDumpstateDevice;

int main() {
    const auto si = VsockConnectionInfo::fromRoPropertyStore(
            {
                    "ro.boot.vendor.dumpstate.server.cid",
                    "ro.vendor.dumpstate.server.cid",
            },
            {
                    "ro.boot.vendor.dumpstate.server.port",
                    "ro.vendor.dumpstate.server.port",
            });

    if (!si) {
        ALOGE("failed to get server connection cid/port; configure and try again.");
        return 1;
    } else {
        ALOGI("Connecting to vsock server at %s", si->str().c_str());
    }

    sp<IDumpstateDevice> dumpstate = makeVirtualizationDumpstateDevice(si->str());
    // This method MUST be called before interacting with any HIDL interfaces.
    configureRpcThreadpool(1, true);
    if (dumpstate->registerAsService() != OK) {
        ALOGE("Could not register service.");
        return 1;
    }
    joinRpcThreadpool();
}

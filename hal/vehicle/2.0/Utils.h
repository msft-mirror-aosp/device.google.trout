/*
 * Copyright (C) 2019 The Android Open Source Project
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

#ifndef android_hardware_automotive_vehicle_V2_0_impl_virtualization_Utils_H_
#define android_hardware_automotive_vehicle_V2_0_impl_virtualization_Utils_H_

#include <optional>
#include <string>

#include <android-base/logging.h>

#include <vsockinfo.h>

namespace android {
namespace hardware {
namespace automotive {
namespace vehicle {
namespace V2_0 {
namespace impl {

struct VirtualizedVhalServerInfo {
    android::hardware::automotive::utils::VsockConnectionInfo vsock;

    std::string powerStateMarkerFilePath;
    std::string powerStateSocket;

    static std::optional<VirtualizedVhalServerInfo> fromCommandLine(int argc, char* argv[],
                                                                    std::string* error);
    static std::optional<VirtualizedVhalServerInfo> fromRoPropertyStore();

    std::string getServerUri() const;
};

}  // namespace impl
}  // namespace V2_0
}  // namespace vehicle
}  // namespace automotive
}  // namespace hardware
}  // namespace android

#endif  // android_hardware_automotive_vehicle_V2_0_impl_virtualization_Utils_H_

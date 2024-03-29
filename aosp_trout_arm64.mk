#
# Copyright (C) 2020 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

$(call inherit-product, device/google/cuttlefish/vsoc_arm64/auto/aosp_cf.mk)

# Audio Control HAL
# TODO (chenhaosjtuacm, egranata): move them to kernel command line
LOCAL_AUDIOCONTROL_PROPERTIES ?= \
    ro.vendor.audiocontrol.server.cid=1000 \
    ro.vendor.audiocontrol.server.port=9410 \

include device/google/trout/aosp_trout_common.mk

DEVICE_MANIFEST_FILE += device/google/trout/manifest.xml

# Sensor HAL
# The implementations use SCMI, which only works on arm architecture
LOCAL_SENSOR_PRODUCT_PACKAGE ?= \
    android.hardware.sensors@2.0-service.multihal \
    android.hardware.sensors@2.0-service.multihal.rc \
    android.hardware.sensors@2.0-Google-IIO-Subhal \

PRODUCT_COPY_FILES += \
    device/google/trout/product_files/odm/ueventd.rc:$(TARGET_COPY_OUT_ODM)/ueventd.rc \
    device/google/trout/product_files/odm/usr/idc/Vendor_0fff_Product_0fff.idc:$(TARGET_COPY_OUT_ODM)/usr/idc/Vendor_0fff_Product_0fff.idc \

PRODUCT_COPY_FILES += device/google/trout/product_files/vendor/etc/sensors/hals.conf:$(TARGET_COPY_OUT_VENDOR)/etc/sensors/hals.conf

PRODUCT_NAME := aosp_trout_arm64
PRODUCT_DEVICE := vsoc_arm64
PRODUCT_MODEL := arm64 trout

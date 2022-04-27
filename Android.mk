LOCAL_PATH := $(call my-dir)

$(eval $(call declare-1p-copy-files,device/google/trout,))

# Include the sub-makefiles
include $(call all-makefiles-under,$(LOCAL_PATH))

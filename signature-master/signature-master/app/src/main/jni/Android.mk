LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := swig
LOCAL_SRC_FILES := swig/dtw.cpp swig/global_signature.cpp swig/gmm.cpp swig/GMM_SHORT.cpp swig/hmm.cpp swig/kmeans.cpp swig/memalc.cpp swig/Obs.cpp swig/pca.cpp swig/signature.cpp swig/signature_wrap.cpp swig/statistics.cpp swig/stdafx.cpp
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/swig

include $(BUILD_SHARED_LIBRARY)


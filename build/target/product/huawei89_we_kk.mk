
PRODUCT_PACKAGES := \
	EngineerCode \
#    FMRadio
#    MyTube \
#    VideoPlayer


$(call inherit-product, $(SRC_TARGET_DIR)/product/common.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/telephony.mk)

# Overrides
PRODUCT_BRAND  := alps
PRODUCT_NAME   := $(TARGET_PRODUCT)
PRODUCT_DEVICE := $(TARGET_PRODUCT)




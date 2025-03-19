#!/bin/bash

set -e

FIRMWARE_BINARY_SUFFIX="$1"
BOOTLOADER_BINARY_SUFFIX="$2"

NRFUTIL_CMD="pc-nrfutil/./nrfutil"

BINARY_PATH="build_release/Binary"
APP_HEX="${BINARY_PATH}/manufacturing/LDLC-Karmeliet-nRF${FIRMWARE_BINARY_SUFFIX}-${APP_STRING_VERSION_NUMBER}.hex"
BL_HEX="${BINARY_PATH}/manufacturing/LDLC-Karmeliet-nRF-secure-bootloader-${BL_STRING_VERSION_NUMBER}${BOOTLOADER_BINARY_SUFFIX}.hex"

PACKAGENAME_APP_ONLY="${BINARY_PATH}/update/Nemeio_Keyboard_NRF52_pkg_app_only${FIRMWARE_BINARY_SUFFIX}-${APP_STRING_VERSION_NUMBER}.zip"
PACKAGENAME_APP_WITH_BOOT_AND_SD="${BINARY_PATH}/update/Nemeio_Keyboard_NRF52_pkg_complete${FIRMWARE_BINARY_SUFFIX}-${APP_STRING_VERSION_NUMBER}${BOOTLOADER_BINARY_SUFFIX}.zip"
SETTINGS_NAME="${BINARY_PATH}/manufacturing/Nemeio_Keyboard_NRF52_settings-${APP_STRING_VERSION_NUMBER}.hex"

KEY_FILE="keys/nemeiopriv.pem"

# Softdevice information
export SD_REQ="0xB7,0x101"
export SD_ID=0x101
SD_FILE="nRF5_SDK/components/softdevice/s132/hex/s132_nrf52_${SD_VERSION}_softdevice.hex"

if [ "$USING_AZURE_KEYVAULT" = true ] ; then
        echo "Using Azure keyvault to perform the signature"
        keysubcmd="--azure_keyvault_url "$NEMEIO_AZURE_KEYVAULT_URL" --azure_keyvault_key_name "$NEMEIO_AZURE_KEYVAULT_NRF_KEYNAME" --azure_keyvault_public_key "$NEMEIO_AZURE_KEYVAULT_NRF_PUBKEY_PEM_PATH""
    else
        echo "Using local private key to perform the signature"
        keysubcmd="--key-file $KEY_FILE"
fi

# The required Softdevice for application update is the current SD_ID (SD_REQ is used for SD+BL update)
if ! $NRFUTIL_CMD pkg generate --hw-version 52 --sd-req $SD_ID \
 --sd-id $SD_ID \
 --application-version $APP_BINARY_VERSION_NUMBER --application $APP_HEX --app-boot-validation VALIDATE_ECDSA_P256_SHA256 \
 $keysubcmd $PACKAGENAME_APP_ONLY
then
	exit 1
fi

if ! $NRFUTIL_CMD pkg generate --hw-version 52 --sd-req $SD_REQ \
 --softdevice $SD_FILE --sd-id $SD_ID \
 --bootloader-version $BL_BINARY_VERSION_NUMBER --bootloader $BL_HEX --sd-boot-validation VALIDATE_ECDSA_P256_SHA256 \
 --application-version $APP_BINARY_VERSION_NUMBER --application $APP_HEX --app-boot-validation VALIDATE_ECDSA_P256_SHA256 \
 $keysubcmd $PACKAGENAME_APP_WITH_BOOT_AND_SD
then
	exit 1
fi

# Bootloader settings is set to use witekio header (rollback support)
if ! $NRFUTIL_CMD settings generate --family NRF52 --application $APP_HEX --application-version $APP_BINARY_VERSION_NUMBER --app-boot-validation VALIDATE_ECDSA_P256_SHA256 \
 --bootloader-version $BL_BINARY_VERSION_NUMBER \
 --softdevice $SD_FILE --sd-boot-validation VALIDATE_ECDSA_P256_SHA256 \
 --bl-settings-version 130 $keysubcmd $SETTINGS_NAME
then
	exit 1
fi


srec_cat ${BL_HEX} -Intel ${APP_HEX}  -Intel ${SETTINGS_NAME} -Intel ${SD_FILE} -Intel -o ${BINARY_PATH}/manufacturing/Scratch-Install${FIRMWARE_BINARY_SUFFIX}-${APP_STRING_VERSION_NUMBER}${BOOTLOADER_BINARY_SUFFIX}.hex -Intel

# For vscode
cp ${BINARY_PATH}/manufacturing/Scratch-Install${FIRMWARE_BINARY_SUFFIX}-${APP_STRING_VERSION_NUMBER}${BOOTLOADER_BINARY_SUFFIX}.hex ${BINARY_PATH}/manufacturing/Scratch-Install${FIRMWARE_BINARY_SUFFIX}${BOOTLOADER_BINARY_SUFFIX}.hex


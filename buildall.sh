#! /bin/bash -e

NPROC=`nproc`

FIRMWARE_BINARY_SUFFIX=""
ADDITIONAL_CMAKE_FIRMWARE_FLAGS=""
BOOTLOADER_BINARY_SUFFIX=""

# Entering correct directory
BASEDIR=$(dirname "$0")
cd ${BASEDIR}

echo "Source version number"
source set_version.sh

# Generate public
NRFUTIL_CMD="pc-nrfutil/./nrfutil"
DFU_PUBLIC_KEY_C_PATH="secure_bootloader/dfu_public_key.c"
LOCAL_KEY_FILE="keys/nemeiopriv.pem"

# Environment variables can give additional information about build
# We need to set variable to 1 or 0 because of CMake Cache (it uses the old value if not defined)
if [ "x${CEM_TESTS}" = "x1" ]; then
	FIRMWARE_BINARY_SUFFIX="-cem"
	ADDITIONAL_CMAKE_FIRMWARE_FLAGS="-DINTERNAL_CEM_TESTS=1"
	echo "------ Building with cem ENABLED ------"
else
	ADDITIONAL_CMAKE_FIRMWARE_FLAGS="-DINTERNAL_CEM_TESTS=0"
fi

if [ "x${ROLLBACK_TESTS}" = "x1" ]; then
	FIRMWARE_BINARY_SUFFIX="${FIRMWARE_BINARY_SUFFIX}-rollback-test"
	ADDITIONAL_CMAKE_FIRMWARE_FLAGS="${ADDITIONAL_CMAKE_FIRMWARE_FLAGS} -DINTERNAL_ROLLBACK_TEST=1"
	echo "------ Building with rollback tests ENABLED ------"
else
	ADDITIONAL_CMAKE_FIRMWARE_FLAGS="${ADDITIONAL_CMAKE_FIRMWARE_FLAGS} -DINTERNAL_ROLLBACK_TEST=0"
fi

if [ "x${ENABLE_SECURE_BOOT}" = "x1" ]; then
	SECURE_SUFFIX=-APPROTECT
	BOOTLOADER_BINARY_SUFFIX=${BOOTLOADER_BINARY_SUFFIX}${SECURE_SUFFIX}
	ADDITIONAL_CMAKE_BOOTLOADER_FLAGS="-DINTERNAL_ENABLE_SECURE_BOOT=1"
	echo "------ Building with secure boot ENABLED ------"
else
	SECURE_SUFFIX=-UNSECURE
	BOOTLOADER_BINARY_SUFFIX=${BOOTLOADER_BINARY_SUFFIX}${SECURE_SUFFIX}
	# Force flags to 0 because of CMake Cache
	ADDITIONAL_CMAKE_BOOTLOADER_FLAGS="-DINTERNAL_ENABLE_SECURE_BOOT=0"
	echo "------ Building with secure boot DISABLED ------"
fi

USING_AZURE_KEYVAULT=false
if ! [ -z "${NEMEIO_AZURE_KEYVAULT_URL}" ]; then
	echo "Using Keyvault URL :" $NEMEIO_AZURE_KEYVAULT_URL
	if ! [ -z "${NEMEIO_AZURE_KEYVAULT_NRF_KEYNAME}" ]; then
	echo "Using Key :" $NEMEIO_AZURE_KEYVAULT_NRF_KEYNAME
		if [ -f "$NEMEIO_AZURE_KEYVAULT_NRF_PUBKEY_PEM_PATH" ]; then
        echo "Using Public Key :" $NEMEIO_AZURE_KEYVAULT_NRF_PUBKEY_PEM_PATH
		USING_AZURE_KEYVAULT=true
		else
			echo "Public Key path is not set"
		fi
	else
	echo "Azure Key Vault key name is not set"
	fi
else
	echo "Azure Key Vault URL is not set"
fi

export USING_AZURE_KEYVAULT

if [ "$USING_AZURE_KEYVAULT" = true ] ; then
        echo "Using Azure keyvault to perform the signature"
		SOURCE_KEY="azure"
        keysubcmd="--key pk --azure_keyvault_url "$NEMEIO_AZURE_KEYVAULT_URL" --azure_keyvault_key_name "$NEMEIO_AZURE_KEYVAULT_NRF_KEYNAME" --azure_keyvault_public_key "$NEMEIO_AZURE_KEYVAULT_NRF_PUBKEY_PEM_PATH""
    else
        echo "Using local private key to perform the signature"
		SOURCE_KEY="local"
        keysubcmd="--key pk $LOCAL_KEY_FILE"
fi

BOOTLOADER_BINARY_SUFFIX=-${SOURCE_KEY}${BOOTLOADER_BINARY_SUFFIX}

ADDITIONAL_CMAKE_BOOTLOADER_FLAGS="${ADDITIONAL_CMAKE_BOOTLOADER_FLAGS} -DSOURCE_KEY=${SOURCE_KEY}"

rm -f $DFU_PUBLIC_KEY_C_PATH
$NRFUTIL_CMD keys display $keysubcmd --format code > $DFU_PUBLIC_KEY_C_PATH

mkdir -p build_release
cd build_release

mkdir -p Binary
mkdir -p Binary/manufacturing
cp ../cmake-tools/flash.sh Binary/manufacturing/.

mkdir -p Binary/update

mkdir -p firmware
cd firmware
cmake ../../LDLC_BLE_HID_Keyboard -DVERSION=${APP_STRING_VERSION_NUMBER} -DAPP_MAJOR_NUMBER=${APP_MAJOR_NUMBER} -DAPP_BUILD_NUMBER=${APP_BUILD_NUMBER} ${ADDITIONAL_CMAKE_FIRMWARE_FLAGS}
if ! make -j$NPROC
then
	exit 1
fi

cp LDLC-Karmeliet-nRF ../Binary/LDLC-Karmeliet-nRF${FIRMWARE_BINARY_SUFFIX}-${APP_STRING_VERSION_NUMBER}
cp LDLC-Karmeliet-nRF.hex ../Binary/manufacturing/LDLC-Karmeliet-nRF${FIRMWARE_BINARY_SUFFIX}-${APP_STRING_VERSION_NUMBER}.hex
cp output.map ../Binary/LDLC-Karmeliet-nRF${FIRMWARE_BINARY_SUFFIX}-${APP_STRING_VERSION_NUMBER}.map
cp flash.jlink ../Binary/manufacturing/.
cp erase.jlink ../Binary/manufacturing/.

# For vscode
cp LDLC-Karmeliet-nRF ../Binary/.
cp LDLC-Karmeliet-nRF.hex ../Binary/.

cd ..

mkdir -p secure-bootloader
cd secure-bootloader
cmake ../../secure_bootloader ${ADDITIONAL_CMAKE_BOOTLOADER_FLAGS}
if ! make -j$NPROC
then
	exit 1
fi

cp LDLC-Karmeliet-nRF-secure-bootloader${BOOTLOADER_BINARY_SUFFIX} ../Binary/LDLC-Karmeliet-nRF-secure-bootloader-${BL_STRING_VERSION_NUMBER}${BOOTLOADER_BINARY_SUFFIX}
cp LDLC-Karmeliet-nRF-secure-bootloader${BOOTLOADER_BINARY_SUFFIX}.hex ../Binary/manufacturing/LDLC-Karmeliet-nRF-secure-bootloader-${BL_STRING_VERSION_NUMBER}${BOOTLOADER_BINARY_SUFFIX}.hex
cp output.map ../Binary/LDLC-Karmeliet-nRF-secure-bootloader-${BL_STRING_VERSION_NUMBER}${BOOTLOADER_BINARY_SUFFIX}.map

# For vscode
cp LDLC-Karmeliet-nRF-secure-bootloader${BOOTLOADER_BINARY_SUFFIX} ../Binary/.
cp LDLC-Karmeliet-nRF-secure-bootloader${BOOTLOADER_BINARY_SUFFIX}.hex ../Binary/.

cd ..

cd ..

./make_package_nrf.sh "${FIRMWARE_BINARY_SUFFIX}" "${BOOTLOADER_BINARY_SUFFIX}"

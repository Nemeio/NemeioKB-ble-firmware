#!/bin/sh

set -x

# Bitbucket information must be given using environment variables
SONAR_TOKEN=$1

COMPILATION_DIR=build_tests/sonar
OUTPUT_DIR=work

if [ ! -d ${COVERAGE_DIR} ]; then
  exit 2
fi

mkdir -p ${COMPILATION_DIR} && cd ${COMPILATION_DIR}
mkdir -p ${OUTPUT_DIR}
cmake ../../LDLC_BLE_HID_Keyboard -DVERSION="1"

build-wrapper-linux-x86-64 --out-dir "$OUTPUT_DIR" make clean all -j$(nproc)

cd - || exit

if [ ! -z $BITBUCKET_PR_ID ]; then
  BITBUCKET_OPTS="-Dsonar.pullrequest.key=$BITBUCKET_PR_ID -Dsonar.pullrequest.branch=$BITBUCKET_BRANCH -Dsonar.pullrequest.base=$BITBUCKET_PR_DESTINATION_BRANCH"
else
  BITBUCKET_OPTS="-Dsonar.branch.name=$BITBUCKET_BRANCH"
fi

echo "============ Start analysis ============"
sonar-scanner -X \
  -Dsonar.cfamily.build-wrapper-output="${COMPILATION_DIR}/${OUTPUT_DIR}" \
  -Dsonar.sources=. \
  -Dsonar.organization=adeneo-embedded \
  -Dsonar.projectKey=adeneo-embedded_b1994-ldlc-karmeliet-firmware-ble \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.login=${SONAR_TOKEN} \
  ${BITBUCKET_OPTS} \
  -Dsonar.cfamily.threads=$(nproc) \
  -Dsonar.cfamily.cache.enabled=true \
  -Dsonar.cfamily.cache.path=$(pwd)/.sonar \
  -Dsonar.inclusions="SPI_Protocol_Common/**/*,LDLC_BLE_HID_Keyboard/**/*" \
  -Dsonar.exclusions="secure_bootloader/**/*,nRF5_SDK/**/*" \
  -Dsonar.verbose=false \
  -Dsonar.log.level=INFO \
  -Dsonar.scm.exclusions.disabled=true

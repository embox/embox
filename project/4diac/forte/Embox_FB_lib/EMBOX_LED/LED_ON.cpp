/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x NG!
 ***
 *** Name: LED_ON
 *** Description: Service Interface Function Block Type
 *** Version:
***     1.0: 2025-05-16/dmitry -  - 
 *************************************************************************/

#include "LED_ON.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "LED_ON_gen.cpp"
#endif


DEFINE_FIRMWARE_FB(FORTE_LED_ON, g_nStringIdLED_ON)

const CStringDictionary::TStringId FORTE_LED_ON::scm_anDataInputNames[] = {g_nStringIdLED_N};

const CStringDictionary::TStringId FORTE_LED_ON::scm_anDataInputTypeIds[] = {g_nStringIdDINT};

const CStringDictionary::TStringId FORTE_LED_ON::scm_anDataOutputNames[] = {g_nStringIdSUCCESS};

const CStringDictionary::TStringId FORTE_LED_ON::scm_anDataOutputTypeIds[] = {g_nStringIdBOOL};

const TForteInt16 FORTE_LED_ON::scm_anEIWithIndexes[] = {-1};
const CStringDictionary::TStringId FORTE_LED_ON::scm_anEventInputNames[] = {g_nStringIdREQ};

const TForteInt16 FORTE_LED_ON::scm_anEOWithIndexes[] = {-1};
const CStringDictionary::TStringId FORTE_LED_ON::scm_anEventOutputNames[] = {g_nStringIdCNF};


const SFBInterfaceSpec FORTE_LED_ON::scm_stFBInterfaceSpec = {
  1, scm_anEventInputNames, nullptr, scm_anEIWithIndexes,
  1, scm_anEventOutputNames, nullptr, scm_anEOWithIndexes,
  1, scm_anDataInputNames, scm_anDataInputTypeIds,
  1, scm_anDataOutputNames, scm_anDataOutputTypeIds,
  0, nullptr
};

void FORTE_LED_ON::executeEvent(int pa_nEIID) {
  if (pa_nEIID == scm_nEventREQID) {
      int err = leddrv_led_on(!st_LED_N());
      st_SUCCESS() = err? false : true;
      sendOutputEvent(scm_nEventCNFID);
  }
}



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

#ifndef _LED_ON_H_
#define _LED_ON_H_

#include "funcbloc.h"
#include "forte_bool.h"
#include "forte_dint.h"
#include "forte_array_at.h"

#include <lib/leddrv.h>

class FORTE_LED_ON: public CFunctionBlock {
  DECLARE_FIRMWARE_FB(FORTE_LED_ON)

private:
  static const CStringDictionary::TStringId scm_anDataInputNames[];
  static const CStringDictionary::TStringId scm_anDataInputTypeIds[];
  
  static const CStringDictionary::TStringId scm_anDataOutputNames[];
  static const CStringDictionary::TStringId scm_anDataOutputTypeIds[];
  
  static const TEventID scm_nEventREQID = 0;
  
  static const TForteInt16 scm_anEIWithIndexes[];
  static const CStringDictionary::TStringId scm_anEventInputNames[];
  
  static const TEventID scm_nEventCNFID = 0;
  
  static const TForteInt16 scm_anEOWithIndexes[];
  static const CStringDictionary::TStringId scm_anEventOutputNames[];
  

  static const SFBInterfaceSpec scm_stFBInterfaceSpec;

  CIEC_DINT &st_LED_N() {
    return *static_cast<CIEC_DINT*>(getDI(0));
  }
  
  CIEC_BOOL &st_SUCCESS() {
    return *static_cast<CIEC_BOOL*>(getDO(0));
  }
  

  FORTE_FB_DATA_ARRAY(1, 1, 1, 0);

  void executeEvent(int pa_nEIID);

public:
   FORTE_LED_ON(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes) :
       CFunctionBlock( pa_poSrcRes, &scm_stFBInterfaceSpec, pa_nInstanceNameId, m_anFBConnData, m_anFBVarsData) {
   };

  virtual ~FORTE_LED_ON() = default;
};

#endif // _LED_ON_H_



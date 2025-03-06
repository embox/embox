FUNCTION_BLOCK LEDDRV
  VAR_INPUT
    LED: UINT;
    VAL: BOOL;
  END_VAR
  VAR_OUTPUT
    SUCCESS: BOOL;
  END_VAR
  VAR
    R_TRIG0: R_TRIG;
    F_TRIG0: F_TRIG;
  END_VAR

  R_TRIG0(CLK := VAL);
  F_TRIG0(CLK := VAL);
  IF R_TRIG0.Q THEN
    { SetFbVar(SUCCESS, leddrv_led_on(GetFbVar(LED)) ? FALSE: TRUE); }
  END_IF;
  IF F_TRIG0.Q THEN
    { SetFbVar(SUCCESS, leddrv_led_off(GetFbVar(LED)) ? FALSE: TRUE); }
  END_IF;
END_FUNCTION_BLOCK

PROGRAM Blink
  VAR
    led_num AT %MW0: UINT := 0;
  END_VAR
  VAR
    led_val: BOOL;
    TON0: TON;
    TOF0: TOF;
    LEDDRV0: LEDDRV;
    LEDDRV1: LEDDRV;
  END_VAR

  TON0(EN := TRUE, IN := NOT(led_val), PT := T#1s);
  TOF0(EN := TON0.ENO, IN := TON0.Q, PT := T#1s);
  led_val := TOF0.Q;
  LEDDRV0(LED := led_num, VAL := led_val);
END_PROGRAM


CONFIGURATION blink_leddrv

  RESOURCE resource0 ON PLC
    TASK task0(INTERVAL := T#50ms, PRIORITY := 0);
    PROGRAM instance0 WITH task0: Blink;
  END_RESOURCE
END_CONFIGURATION

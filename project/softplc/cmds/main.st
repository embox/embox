PROGRAM Blink
  VAR
    blink_led: BOOL;
    TON0: TON;
    TOF0: TOF;
    LEDDRV_SET0: LEDDRV_SET;
    R_TRIG0: R_TRIG;
    LEDDRV_CLR0: LEDDRV_CLR;
    F_TRIG0: F_TRIG;
    _TMP_NOT14_OUT: BOOL;
  END_VAR

  _TMP_NOT14_OUT := NOT(blink_led);
  TON0(IN := _TMP_NOT14_OUT, PT := T#1000ms);
  TOF0(IN := TON0.Q, PT := T#1000ms);
  blink_led := TOF0.Q;
  R_TRIG0(CLK := blink_led);
  LEDDRV_SET0(EN := R_TRIG0.Q, LED_NUM := 0);
  F_TRIG0(CLK := blink_led);
  LEDDRV_CLR0(EN := F_TRIG0.Q, LED_NUM := 0);
END_PROGRAM


CONFIGURATION config

  RESOURCE resource0 ON PLC
    TASK task0(INTERVAL := T#50ms, PRIORITY := 0);
    PROGRAM instance0 WITH task0: Blink;
  END_RESOURCE
END_CONFIGURATION

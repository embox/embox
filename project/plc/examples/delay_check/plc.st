PROGRAM DelayCheck
  VAR
    OLD_TIME_SAVED: BOOL;
    OLD_TIME: TIME;
  END_VAR

  IF OLD_TIME_SAVED THEN
  {
    IEC_TIME tmp = timespec_sub(__CURRENT_TIME, GetFbVar(OLD_TIME));
    printk("%ld ms\n", (long)timespec_to_ms(&tmp));
  }
  ELSE
    OLD_TIME_SAVED := TRUE;
  END_IF;
  {
    SetFbVar(OLD_TIME, __CURRENT_TIME);
  }
END_PROGRAM


CONFIGURATION delay_check

  RESOURCE resource0 ON PLC
    TASK task0(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM instance0 WITH task0: DelayCheck;
  END_RESOURCE
END_CONFIGURATION

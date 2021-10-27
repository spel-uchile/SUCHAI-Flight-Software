# Report of osTaskDelayUntil bug

## Results after the fix

### Single failure test

The loop recovers the timing after two loops, the total execution time is not effected

    [DEBUG][1532556297][TEST 1] Iter 00, tick 633551126, loop 0999023 us, sleep 0999023 us, work 0000000 us
    [DEBUG][1532556298][TEST 1] Iter 01, tick 634550174, loop 0999012 us, sleep 0999012 us, work 0000000 us
    [DEBUG][1532556299][TEST 1] Iter 02, tick 635550174, loop 0999981 us, sleep -500033 us, work 1500000 us
    [DEBUG][1532556300][TEST 1] Iter 03, tick 636549241, loop 0499012 us, sleep 0499012 us, work 0000000 us
    [DEBUG][1532556301][TEST 1] Iter 04, tick 637548279, loop 0999016 us, sleep 0999015 us, work 0000000 us
    [INFO ][1532556301][TEST 1] Test executed in 4996199 us, expected ~5000000.000000 us
    
### Random load test

The loop can recover the timing when the load exceed the 1000ms, in this cases one loop can be shorter but the total
execution time is not affected.

    [DEBUG][1532556216][TEST 1] Iter 00, tick 552711858, loop 0999999 us, sleep -282075 us, work 1282000 us
    [DEBUG][1532556217][TEST 1] Iter 01, tick 553710981, loop 0717023 us, sleep 0314010 us, work 0403000 us
    [DEBUG][1532556218][TEST 1] Iter 02, tick 554710982, loop 0999983 us, sleep -196029 us, work 1196000 us
    [DEBUG][1532556219][TEST 1] Iter 03, tick 555710982, loop 0803952 us, sleep -374060 us, work 1178000 us
    [DEBUG][1532556220][TEST 1] Iter 04, tick 556710088, loop 0625027 us, sleep 0044012 us, work 0581000 us
    [DEBUG][1532556221][TEST 1] Iter 05, tick 557709138, loop 0999028 us, sleep 0025011 us, work 0974000 us
    [DEBUG][1532556222][TEST 1] Iter 06, tick 558708185, loop 0999026 us, sleep 0102013 us, work 0897000 us
    [DEBUG][1532556223][TEST 1] Iter 07, tick 559707233, loop 0999027 us, sleep 0672014 us, work 0327000 us
    [DEBUG][1532556224][TEST 1] Iter 08, tick 560706283, loop 0999028 us, sleep 0324014 us, work 0675000 us
    [DEBUG][1532556225][TEST 1] Iter 09, tick 561706283, loop 0999974 us, sleep -098042 us, work 1098000 us
    [DEBUG][1532556226][TEST 1] Iter 10, tick 562706284, loop 0901932 us, sleep -351082 us, work 1253000 us
    [DEBUG][1532556227][TEST 1] Iter 11, tick 563705408, loop 0648023 us, sleep 0423012 us, work 0225000 us
    [DEBUG][1532556228][TEST 1] Iter 12, tick 564704469, loop 0999043 us, sleep 0063025 us, work 0936000 us
    [DEBUG][1532556229][TEST 1] Iter 13, tick 565704470, loop 0999971 us, sleep -091043 us, work 1091000 us
    [DEBUG][1532556230][TEST 1] Iter 14, tick 566703563, loop 0908026 us, sleep 0373013 us, work 0535000 us
    [DEBUG][1532556231][TEST 1] Iter 15, tick 567702606, loop 0999024 us, sleep 0472012 us, work 0527000 us
    [DEBUG][1532556232][TEST 1] Iter 16, tick 568702607, loop 0999983 us, sleep -090028 us, work 1090000 us
    [DEBUG][1532556233][TEST 1] Iter 17, tick 569702607, loop 0909953 us, sleep -274062 us, work 1184000 us
    [DEBUG][1532556234][TEST 1] Iter 18, tick 570701717, loop 0725024 us, sleep 0570013 us, work 0155000 us
    [DEBUG][1532556235][TEST 1] Iter 19, tick 571701717, loop 0999978 us, sleep -143036 us, work 1143000 us
    [INFO ][1532556235][TEST 1] Test executed in 20132918 us, expected ~20000000.000000 us


## Results before the fix

### Single failure test

The after the fail the loop loses control and executes without sleep. The test fails the assert

    [DEBUG][1532556381][TEST 1] Iter 00, tick 718287253, loop 0999014 us, sleep 0999014 us, work 0000000 us
    [DEBUG][1532556382][TEST 1] Iter 01, tick 719286293, loop 0999014 us, sleep 0999014 us, work 0000000 us
    [DEBUG][1532556384][TEST 1] Iter 02, tick 719286293, loop -000024 us, sleep -1500038 us, work 1500000 us
    [DEBUG][1532556384][TEST 1] Iter 03, tick 719286293, loop -1500060 us, sleep -1500061 us, work 0000000 us
    [DEBUG][1532556384][TEST 1] Iter 04, tick 719286293, loop -1500069 us, sleep -1500070 us, work 0000000 us
    [INFO ][1532556384][TEST 1] Test executed in 3498129 us, expected ~5000000.000000 us
    SUCHAI_Flight_Software_Test: /mnt/data2/Documentos/GitHub/SUCHAI-Flight-Software/test/test_bug_delay/src/system/taskTest.c:52: taskTest: Assertion `xFinalTime - xStartTime >= 0.999*TEST_MAX_SECONDS*1000000.0' failed.

### Random load test

Same as above, the loop do not resume the correct timing 

    [DEBUG][1532556438][TEST 1] Iter 00, tick 775124422, loop 0999027 us, sleep 0537013 us, work 0462000 us
    [DEBUG][1532556439][TEST 1] Iter 01, tick 775124422, loop -000020 us, sleep -1251038 us, work 1251000 us
    [DEBUG][1532556440][TEST 1] Iter 02, tick 775124422, loop -1251060 us, sleep -1954072 us, work 0703000 us
    [DEBUG][1532556440][TEST 1] Iter 03, tick 775124422, loop -1954095 us, sleep -2173106 us, work 0219000 us
    [DEBUG][1532556441][TEST 1] Iter 04, tick 775124422, loop -2173128 us, sleep -2319141 us, work 0146000 us
    [DEBUG][1532556441][TEST 1] Iter 05, tick 775124422, loop -2319162 us, sleep -2621175 us, work 0302000 us
    [DEBUG][1532556441][TEST 1] Iter 06, tick 775124422, loop -2621197 us, sleep -3111208 us, work 0490000 us
    [DEBUG][1532556442][TEST 1] Iter 07, tick 775124422, loop -3111227 us, sleep -3423238 us, work 0312000 us
    [DEBUG][1532556443][TEST 1] Iter 08, tick 775124422, loop -3423259 us, sleep -4691270 us, work 1268000 us
    [DEBUG][1532556444][TEST 1] Iter 09, tick 775124422, loop -4691290 us, sleep -5316303 us, work 0625000 us
    [DEBUG][1532556444][TEST 1] Iter 10, tick 775124422, loop -5316325 us, sleep -5612336 us, work 0296000 us
    [DEBUG][1532556445][TEST 1] Iter 11, tick 775124422, loop -5612355 us, sleep -6607367 us, work 0995000 us
    [DEBUG][1532556446][TEST 1] Iter 12, tick 775124422, loop -6607390 us, sleep -7768401 us, work 1161000 us
    [DEBUG][1532556446][TEST 1] Iter 13, tick 775124422, loop -7768420 us, sleep -7915432 us, work 0147000 us
    [DEBUG][1532556447][TEST 1] Iter 14, tick 775124422, loop -7915454 us, sleep -8719466 us, work 0804000 us
    [DEBUG][1532556448][TEST 1] Iter 15, tick 775124422, loop -8719492 us, sleep -9472504 us, work 0753000 us
    [DEBUG][1532556448][TEST 1] Iter 16, tick 775124422, loop -9472523 us, sleep -9837534 us, work 0365000 us
    [DEBUG][1532556448][TEST 1] Iter 17, tick 775124422, loop -9837556 us, sleep -9976567 us, work 0139000 us
    [DEBUG][1532556449][TEST 1] Iter 18, tick 775124422, loop -9976593 us, sleep -10699603 us, work 0723000 us
    [DEBUG][1532556450][TEST 1] Iter 19, tick 775124422, loop -10699622 us, sleep -11466636 us, work 0767000 us
    [INFO ][1532556450][TEST 1] Test executed in 12465684 us, expected ~20000000.000000 us
    SUCHAI_Flight_Software_Test: /mnt/data2/Documentos/GitHub/SUCHAI-Flight-Software/test/test_bug_delay/src/system/taskTest.c:51: taskTest: Assertion `xFinalTime - xStartTime >= 0.999*TEST_MAX_SECONDS*1000000.0' failed.



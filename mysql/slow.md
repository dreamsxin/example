## 慢查询

```sql
select * from mysql.slow_log;
```

## IO

···shell
SHOW ENGINE INNODB STATUS;
```

```text

=====================================
2024-07-19 09:07:40 140357522642688 INNODB MONITOR OUTPUT
=====================================
Per second averages calculated from the last 19 seconds
-----------------
BACKGROUND THREAD
-----------------
srv_master_thread loops: 3944463 srv_active, 0 srv_shutdown, 25 srv_idle
srv_master_thread log flush and writes: 0
----------
SEMAPHORES
----------
OS WAIT ARRAY INFO: reservation count 75729285
OS WAIT ARRAY INFO: signal count 75308494
RW-shared spins 0, rounds 0, OS waits 0
RW-excl spins 0, rounds 0, OS waits 0
RW-sx spins 0, rounds 0, OS waits 0
Spin rounds per wait: 0.00 RW-shared, 0.00 RW-excl, 0.00 RW-sx
------------------------
LATEST DETECTED DEADLOCK
------------------------
2024-07-01 14:57:21 140358342145792
*** (1) TRANSACTION:
TRANSACTION 575274405, ACTIVE 0 sec fetching rows
mysql tables in use 3, locked 3
LOCK WAIT 685 lock struct(s), heap size 90232, 5 row lock(s)
MySQL thread id 7759846, OS thread handle 140357522913024, query id 1141936724 10.0.0.206 shop executing
SELECT * FROM `account_shares` WHERE account_id in ('fb0d40a0016fc160fb07cc9dfe37855b') AND company_id = '17e1e36eba9440749f8c20be95a80fe4' AND user_id = '46f644309c70400a9b2e4bba4f39b298' AND status = 1 FOR UPDATE

*** (1) HOLDS THE LOCK(S):
RECORD LOCKS space id 1226 page no 3682 n bits 1192 index status of table `yl_shop`.`account_shares` trx id 575274405 lock_mode X locks rec but not gap
Record lock, heap no 275 PHYSICAL RECORD: n_fields 2; compact format; info bits 0
 0: len 1; hex 81; asc  ;;
 1: len 8; hex 0000000000041750; asc        P;;


*** (1) WAITING FOR THIS LOCK TO BE GRANTED:
RECORD LOCKS space id 1226 page no 7111 n bits 136 index PRIMARY of table `yl_shop`.`account_shares` trx id 575274405 lock_mode X locks rec but not gap waiting
Record lock, heap no 67 PHYSICAL RECORD: n_fields 14; compact format; info bits 0
 0: len 8; hex 0000000000041750; asc        P;;
 1: len 6; hex 00002249fda7; asc   "I  ;;
 2: len 7; hex 02000005c0055a; asc       Z;;
 3: len 30; hex 613865396165383439336630303337323032623934336561323836343433; asc a8e9ae8493f0037202b943ea286443; (total 32 bytes);
 4: len 30; hex 376264653764336332626336343638376262393863646232366465356435; asc 7bde7d3c2bc64687bb98cdb26de5d5; (total 32 bytes);
 5: len 30; hex 613536306466396636633230343361666165636566373430333862616631; asc a560df9f6c2043afaecef74038baf1; (total 32 bytes);
 6: len 30; hex 343161656262353037376237343436343935363138303633376130353964; asc 41aebb5077b74464956180637a059d; (total 32 bytes);
 7: len 4; hex 80000018; asc     ;;
 8: len 7; hex 99b3f2e1090000; asc        ;;
 9: len 1; hex 83; asc  ;;
 10: len 0; hex ; asc ;;
 11: len 0; hex ; asc ;;
 12: len 7; hex 99b3c2ee400532; asc     @ 2;;
 13: len 7; hex 99b3c2ee5501cc; asc     U  ;;


*** (2) TRANSACTION:
TRANSACTION 575274407, ACTIVE 0 sec updating or deleting
mysql tables in use 1, locked 1
LOCK WAIT 15 lock struct(s), heap size 1128, 144 row lock(s), undo log entries 2
MySQL thread id 7759836, OS thread handle 140358228625152, query id 1141936799 10.0.0.206 shop updating
UPDATE `account_shares` SET `status`=3,`updated_at`='2024-07-01 14:57:21.046' WHERE id = 268112

*** (2) HOLDS THE LOCK(S):
RECORD LOCKS space id 1226 page no 7111 n bits 136 index PRIMARY of table `yl_shop`.`account_shares` trx id 575274407 lock_mode X locks rec but not gap
Record lock, heap no 4 PHYSICAL RECORD: n_fields 14; compact format; info bits 0
 0: len 8; hex 0000000000041711; asc         ;;
 1: len 6; hex 000022490ca9; asc   "I  ;;
 2: len 7; hex 010000016f11aa; asc     o  ;;
 3: len 30; hex 346234333662356562643231326263663363383538376362633864653635; asc 4b436b5ebd212bcf3c8587cbc8de65; (total 32 bytes);
 4: len 30; hex 376264653764336332626336343638376262393863646232366465356435; asc 7bde7d3c2bc64687bb98cdb26de5d5; (total 32 bytes);
 5: len 30; hex 613536306466396636633230343361666165636566373430333862616631; asc a560df9f6c2043afaecef74038baf1; (total 32 bytes);
 6: len 30; hex 343161656262353037376237343436343935363138303633376130353964; asc 41aebb5077b74464956180637a059d; (total 32 bytes);
 7: len 4; hex 80000018; asc     ;;
 8: len 7; hex 99b3f2e1090000; asc        ;;
 9: len 1; hex 83; asc  ;;
 10: len 0; hex ; asc ;;
 11: len 0; hex ; asc ;;
 12: len 7; hex 99b3c2ebb71b44; asc       D;;
 13: len 7; hex 99b3c2ebe20b86; asc        ;;

Record lock, heap no 6 PHYSICAL RECORD: n_fields 14; compact format; info bits 0
 0: len 8; hex 0000000000041713; asc         ;;
 1: len 6; hex 000022491c9c; asc   "I  ;;
 2: len 7; hex 02000006712f1f; asc     q/ ;;
 3: len 30; hex 653238373363656639303862623664653036303833343736343034653039; asc e2873cef908bb6de06083476404e09; (total 32 bytes);
 4: len 30; hex 376264653764336332626336343638376262393863646232366465356435; asc 7bde7d3c2bc64687bb98cdb26de5d5; (total 32 bytes);
 5: len 30; hex 613536306466396636633230343361666165636566373430333862616631; asc a560df9f6c2043afaecef74038baf1; (total 32 bytes);
 6: len 30; hex 343161656262353037376237343436343935363138303633376130353964; asc 41aebb5077b74464956180637a059d; (total 32 bytes);
 7: len 4; hex 80000018; asc     ;;
 8: len 7; hex 99b3f2e1090000; asc        ;;
 9: len 1; hex 83; asc  ;;
 10: len 0; hex ; asc ;;
 11: len 0; hex ; asc ;;
 12: len 7; hex 99b3c2ebe317e8; asc        ;;
 13: len 7; hex 99b3c2ec0d0cf8; asc        ;;

Record lock, heap no 7 PHYSICAL RECORD: n_fields 14; compact format; info bits 0
 0: len 8; hex 0000000000041714; asc         ;;
 1: len 6; hex 0000224927d4; asc   "I' ;;
 2: len 7; hex 02000002a52554; asc      %T;;
 3: len 30; hex 373234663039376239303333386361653966373935306362393038623434; asc 724f097b90338cae9f7950cb908b44; (total 32 bytes);
 4: len 30; hex 376264653764336332626336343638376262393863646232366465356435; asc 7bde7d3c2bc64687bb98cdb26de5d5; (total 32 bytes);
 5: len 30; hex 613536306466396636633230343361666165636566373430333862616631; asc a560df9f6c2043afaecef74038baf1; (total 32 bytes);
 6: len 30; hex 343161656262353037376237343436343935363138303633376130353964; asc 41aebb5077b74464956180637a059d; (total 32 bytes);
 7: len 4; hex 80000018; asc     ;;
 8: len 7; hex 99b3f2e1090000; asc        ;;
 9: len 1; hex 83; asc  ;;
 10: len 0; hex ; asc ;;
 11: len 0; hex ; asc ;;
 12: len 7; hex 99b3c2ec14008c; asc        ;;
 13: len 7; hex 99b3c2ec2c1248; asc     , H;;

Record lock, heap no 8 PHYSICAL RECORD: n_fields 14; compact format; info bits 0
 0: len 8; hex 0000000000041715; asc         ;;
 1: len 6; hex 000022493784; asc   "I7 ;;
 2: len 7; hex 01000006b81d7f; asc        ;;
 3: len 30; hex 346234613138303635386239646562653637383139306633356361313964; asc 4b4a180658b9debe678190f35ca19d; (total 32 bytes);
 4: len 30; hex 376264653764336332626336343638376262393863646232366465356435; asc 7bde7d3c2bc64687bb98cdb26de5d5; (total 32 bytes);
 5: len 30; hex 613536306466396636633230343361666165636566373430333862616631; asc a560df9f6c2043afaecef74038baf1; (total 32 bytes);
 6: len 30; hex 343161656262353037376237343436343935363138303633376130353964; asc 41aebb5077b74464956180637a059d; (total 32 bytes);
 7: len 4; hex 80000018; asc     ;;
 8: len 7; hex 99b3f2e1090000; asc        ;;
 9: len 1; hex 83; asc  ;;
 10: len 0; hex ; asc ;;
 11: len 0; hex ; asc ;;
 12: len 7; hex 99b3c2ec3408c0; asc     4  ;;
 13: len 7; hex 99b3c2ec592472; asc     Y$r;;

Record lock, heap no 9 PHYSICAL RECORD: n_fields 14; compact format; info bits 0
 0: len 8; hex 0000000000041716; asc         ;;
 1: len 6; hex 000022493e7a; asc   "I>z;;
 2: len 7; hex 0200000669262d; asc     i&-;;
 3: len 30; hex 383431383037346530616530626663373131613663333764313435313565; asc 8418074e0ae0bfc711a6c37d14515e; (total 32 bytes);
 4: len 30; hex 376264653764336332626336343638376262393863646232366465356435; asc 7bde7d3c2bc64687bb98cdb26de5d5; (total 32 bytes);
 5: len 30; hex 613536306466396636633230343361666165636566373430333862616631; asc a560df9f6c2043afaecef74038baf1; (total 32 bytes);
 6: len 30; hex 343161656262353037376237343436343935363138303633376130353964; asc 41aebb5077b74464956180637a059d; (total 32 bytes);
 7: len 4; hex 80000018; asc     ;;
 8: len 7; hex 99b3f2e1090000; asc        ;;
 9: len 1; hex 83; asc  ;;
 10: len 0; hex ; asc ;;
 11: len 0; hex ; asc ;;
 12: len 7; hex 99b3c2ec5a258a; asc     Z% ;;
 13: len 7; hex 99b3c2ec6b1fd6; asc     k  ;;

Record lock, heap no 11 PHYSICAL RECORD: n_fields 14; compact format; info bits 0
 0: len 8; hex 0000000000041718; asc         ;;
 1: len 6; hex 00002249e930; asc   "I 0;;
 2: len 7; hex 020000021610bd; asc        ;;
 3: len 30; hex 353535616432333935386131303939636437383736326564653866626164; asc 555ad23958a1099cd78762ede8fbad; (total 32 bytes);
 4: len 30; hex 376264653764336332626336343638376262393863646232366465356435; asc 7bde7d3c2bc64687bb98cdb26de5d5; (total 32 bytes);
 5: len 30; hex 613536306466396636633230343361666165636566373430333862616631; asc a560df9f6c2043afaecef74038baf1; (total 32 bytes);
 6: len 30; hex 343161656262353037376237343436343935363138303633376130353964; asc 41aebb5077b74464956180637a059d; (total 32 bytes);
 7: len 4; hex 80000018; asc     ;;
 8: len 7; hex 99b3f2e1090000; asc        ;;
 9: len 1; hex 83; asc  ;;
 10: len 0; hex ; asc ;;
 11: len 0; hex ; asc ;;
 12: len 7; hex 99b3c2ec711176; asc     q v;;
 13: len 7; hex 99b3c2ee1f1fa4; asc        ;;

Record lock, heap no 57 PHYSICAL RECORD: n_fields 14; compact format; info bits 0
 0: len 8; hex 0000000000041746; asc        F;;
 1: len 6; hex 00002249f442; asc   "I B;;
 2: len 7; hex 020000022e12d2; asc     .  ;;
 3: len 30; hex 346262653439643336326462393063376135366564386264363138343438; asc 4bbe49d362db90c7a56ed8bd618448; (total 32 bytes);
 4: len 30; hex 376264653764336332626336343638376262393863646232366465356435; asc 7bde7d3c2bc64687bb98cdb26de5d5; (total 32 bytes);
 5: len 30; hex 613536306466396636633230343361666165636566373430333862616631; asc a560df9f6c2043afaecef74038baf1; (total 32 bytes);
 6: len 30; hex 343161656262353037376237343436343935363138303633376130353964; asc 41aebb5077b74464956180637a059d; (total 32 bytes);
 7: len 4; hex 80000018; asc     ;;
 8: len 7; hex 99b3f2e1090000; asc        ;;
 9: len 1; hex 83; asc  ;;
 10: len 0; hex ; asc ;;
 11: len 0; hex ; asc ;;
 12: len 7; hex 99b3c2ee251428; asc     % (;;
 13: len 7; hex 99b3c2ee3a2652; asc     :&R;;

Record lock, heap no 67 PHYSICAL RECORD: n_fields 14; compact format; info bits 0
 0: len 8; hex 0000000000041750; asc        P;;
 1: len 6; hex 00002249fda7; asc   "I  ;;
 2: len 7; hex 02000005c0055a; asc       Z;;
 3: len 30; hex 613865396165383439336630303337323032623934336561323836343433; asc a8e9ae8493f0037202b943ea286443; (total 32 bytes);
 4: len 30; hex 376264653764336332626336343638376262393863646232366465356435; asc 7bde7d3c2bc64687bb98cdb26de5d5; (total 32 bytes);
 5: len 30; hex 613536306466396636633230343361666165636566373430333862616631; asc a560df9f6c2043afaecef74038baf1; (total 32 bytes);
 6: len 30; hex 343161656262353037376237343436343935363138303633376130353964; asc 41aebb5077b74464956180637a059d; (total 32 bytes);
 7: len 4; hex 80000018; asc     ;;
 8: len 7; hex 99b3f2e1090000; asc        ;;
 9: len 1; hex 83; asc  ;;
 10: len 0; hex ; asc ;;
 11: len 0; hex ; asc ;;
 12: len 7; hex 99b3c2ee400532; asc     @ 2;;
 13: len 7; hex 99b3c2ee5501cc; asc     U  ;;


*** (2) WAITING FOR THIS LOCK TO BE GRANTED:
RECORD LOCKS space id 1226 page no 3682 n bits 1192 index status of table `yl_shop`.`account_shares` trx id 575274407 lock_mode X locks rec but not gap waiting
Record lock, heap no 275 PHYSICAL RECORD: n_fields 2; compact format; info bits 0
 0: len 1; hex 81; asc  ;;
 1: len 8; hex 0000000000041750; asc        P;;

*** WE ROLL BACK TRANSACTION (2)
------------
TRANSACTIONS
------------
Trx id counter 660502368
Purge done for trx's n:o < 660502367 undo n:o < 0 state: running but idle
History list length 17
LIST OF TRANSACTIONS FOR EACH SESSION:
---TRANSACTION 421840860283680, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860281256, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860303880, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860231160, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860282064, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860239240, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860321656, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860350744, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860349936, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860332968, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860343472, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860337816, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860348320, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860244896, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860232776, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860292568, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860265096, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860291760, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860261056, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860271560, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860349128, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860341856, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860290144, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860337008, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860336200, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860331352, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860326504, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860249744, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860272368, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860301456, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860324080, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860314384, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860245704, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860311960, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860311152, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860307920, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860297416, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860289336, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860284488, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860240856, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860279640, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860278024, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860275600, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860274792, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860269944, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860269136, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860267520, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860266712, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860265904, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860264288, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860263480, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860261864, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860259440, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860253784, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860252168, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860251360, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860247320, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860240048, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860234392, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860299840, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860307112, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860268328, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860305496, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860333776, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860325696, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860324888, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860323272, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860290952, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860320848, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860320040, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860316808, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860316000, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860313576, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860276408, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860294184, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860256208, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860250552, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860330544, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860254592, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860248936, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860278832, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860327312, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860258632, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860243280, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860285296, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860227928, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860346704, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860345896, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860345088, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860344280, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860341048, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860335392, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860322464, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860328120, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860280448, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860288528, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860286104, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860282872, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860257824, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860255400, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860328928, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860340240, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860362864, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860233584, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860319232, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860334584, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860329736, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860286912, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860235200, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860241664, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860230352, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860226312, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860308728, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860300648, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860294992, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860377408, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860303072, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860304688, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860287720, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860358824, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860357208, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860310344, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860246512, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860257016, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860302264, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860298224, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860299032, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860277216, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860296608, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860312768, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860339432, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860318424, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860231968, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860360440, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860338624, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860347512, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860342664, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860317616, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860306304, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860236008, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860228736, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860270752, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860252976, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860293376, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860262672, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860248128, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860242472, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860237624, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860229544, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860260248, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860295800, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860244088, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860309536, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860236816, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860273984, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860273176, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860219040, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860238432, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860225504, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860224696, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860223888, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860223080, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860222272, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860221464, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860220656, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860219848, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860227120, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
---TRANSACTION 421840860218232, not started
0 lock struct(s), heap size 1128, 0 row lock(s)
--------
FILE I/O
--------
I/O thread 0 state: waiting for i/o request (insert buffer thread)
I/O thread 1 state: waiting for i/o request (log thread)
I/O thread 2 state: waiting for i/o request (read thread)
I/O thread 3 state: waiting for i/o request (read thread)
I/O thread 4 state: waiting for i/o request (read thread)
I/O thread 5 state: waiting for i/o request (read thread)
I/O thread 6 state: waiting for i/o request (write thread)
I/O thread 7 state: waiting for i/o request (write thread)
I/O thread 8 state: waiting for i/o request (write thread)
I/O thread 9 state: waiting for i/o request (write thread)
Pending normal aio reads: [0, 0, 0, 0] , aio writes: [0, 0, 0, 0] ,
 ibuf aio reads:, log i/o's:, sync i/o's:
Pending flushes (fsync) log: 0; buffer pool: 197760
342958677 OS file reads, 476859895 OS file writes, 38422546 OS fsyncs
4.14 reads/s, 16384 avg bytes/read, 142.37 writes/s, 9.68 fsyncs/s
-------------------------------------
INSERT BUFFER AND ADAPTIVE HASH INDEX
-------------------------------------
Ibuf: size 1, free list len 194, seg size 196, 2276434 merges
merged operations:
 insert 2255086, delete mark 723215, delete 127724
discarded operations:
 insert 0, delete mark 0, delete 0
Hash table size 103, node heap has 0 buffer(s)
Hash table size 103, node heap has 0 buffer(s)
Hash table size 103, node heap has 0 buffer(s)
Hash table size 103, node heap has 0 buffer(s)
Hash table size 103, node heap has 0 buffer(s)
Hash table size 103, node heap has 0 buffer(s)
Hash table size 103, node heap has 0 buffer(s)
Hash table size 103, node heap has 0 buffer(s)
0.00 hash searches/s, 58564.65 non-hash searches/s
---
LOG
---
Log sequence number          2405698047016
Log buffer assigned up to    2405698047016
Log buffer completed up to   2405698047016
Log written up to            2405698047016
Log flushed up to            2405698046157
Added dirty pages up to      2405698047016
Pages flushed up to          2405679340060
Last checkpoint at           2405679172192
397964529 log i/o's done, 126.21 log i/o's/second
----------------------
BUFFER POOL AND MEMORY
----------------------
Total large memory allocated 0
Dictionary memory allocated 3365893
Buffer pool size   393075
Free buffers       8192
Database pages     384883
Old database pages 141911
Modified db pages  6036
Pending reads      0
Pending writes: LRU 0, flush list 0, single page 0, flush chunk 0
Pages made young 9204246469, not young 3459134356
429.84 youngs/s, 12.77 non-youngs/s
Pages read 342945787, created 197188848, written 45931090
4.14 reads/s, 1.05 creates/s, 8.37 writes/s
Buffer pool hit rate 1000 / 1000, young-making rate 1 / 1000 not 0 / 1000
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 384883, unzip_LRU len: 0
I/O sum[5704]:cur[64], unzip sum[0]:cur[0]
----------------------
INDIVIDUAL BUFFER POOL INFO
----------------------
---BUFFER POOL 0
Buffer pool size   49129
Free buffers       1024
Database pages     48105
Old database pages 17737
Modified db pages  685
Pending reads      0
Pending writes: LRU 0, flush list 0, single page 0, flush chunk 0
Pages made young 1213711330, not young 472972603
35.59 youngs/s, 1.94 non-youngs/s
Pages read 45497630, created 24747512, written 5754934
0.58 reads/s, 0.00 creates/s, 1.05 writes/s
Buffer pool hit rate 1000 / 1000, young-making rate 1 / 1000 not 0 / 1000
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 48105, unzip_LRU len: 0
I/O sum[713]:cur[8], unzip sum[0]:cur[0]
---BUFFER POOL 1
Buffer pool size   49133
Free buffers       1024
Database pages     48109
Old database pages 17738
Modified db pages  833
Pending reads      0
Pending writes: LRU 0, flush list 0, single page 0, flush chunk 0
Pages made young 1113352706, not young 535645643
43.60 youngs/s, 0.37 non-youngs/s
Pages read 42766514, created 24871797, written 5767778
0.47 reads/s, 0.21 creates/s, 1.05 writes/s
Buffer pool hit rate 1000 / 1000, young-making rate 1 / 1000 not 0 / 1000
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 48109, unzip_LRU len: 0
I/O sum[713]:cur[8], unzip sum[0]:cur[0]
---BUFFER POOL 2
Buffer pool size   49134
Free buffers       1024
Database pages     48110
Old database pages 17739
Modified db pages  486
Pending reads      0
Pending writes: LRU 0, flush list 0, single page 0, flush chunk 0
Pages made young 1080238229, not young 352911173
46.74 youngs/s, 1.41 non-youngs/s
Pages read 40137076, created 24649592, written 5516858
0.79 reads/s, 0.68 creates/s, 1.05 writes/s
Buffer pool hit rate 1000 / 1000, young-making rate 3 / 1000 not 0 / 1000
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 48110, unzip_LRU len: 0
I/O sum[713]:cur[8], unzip sum[0]:cur[0]
---BUFFER POOL 3
Buffer pool size   49134
Free buffers       1024
Database pages     48110
Old database pages 17739
Modified db pages  664
Pending reads      0
Pending writes: LRU 0, flush list 0, single page 0, flush chunk 0
Pages made young 1243655186, not young 369577573
44.07 youngs/s, 1.20 non-youngs/s
Pages read 43136118, created 24373460, written 5602715
0.42 reads/s, 0.00 creates/s, 1.05 writes/s
Buffer pool hit rate 1000 / 1000, young-making rate 1 / 1000 not 0 / 1000
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 48110, unzip_LRU len: 0
I/O sum[713]:cur[8], unzip sum[0]:cur[0]
---BUFFER POOL 4
Buffer pool size   49139
Free buffers       1024
Database pages     48115
Old database pages 17741
Modified db pages  657
Pending reads      0
Pending writes: LRU 0, flush list 0, single page 0, flush chunk 0
Pages made young 1163396515, not young 513836002
54.91 youngs/s, 0.26 non-youngs/s
Pages read 43117766, created 24596855, written 5722729
0.42 reads/s, 0.16 creates/s, 1.05 writes/s
Buffer pool hit rate 1000 / 1000, young-making rate 2 / 1000 not 0 / 1000
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 48115, unzip_LRU len: 0
I/O sum[713]:cur[8], unzip sum[0]:cur[0]
---BUFFER POOL 5
Buffer pool size   49143
Free buffers       1024
Database pages     48119
Old database pages 17742
Modified db pages  889
Pending reads      0
Pending writes: LRU 0, flush list 0, single page 0, flush chunk 0
Pages made young 1074513303, not young 525978036
126.46 youngs/s, 3.04 non-youngs/s
Pages read 42931702, created 24608780, written 5861539
0.42 reads/s, 0.00 creates/s, 1.05 writes/s
Buffer pool hit rate 1000 / 1000, young-making rate 6 / 1000 not 0 / 1000
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 48119, unzip_LRU len: 0
I/O sum[713]:cur[8], unzip sum[0]:cur[0]
---BUFFER POOL 6
Buffer pool size   49130
Free buffers       1024
Database pages     48106
Old database pages 17737
Modified db pages  876
Pending reads      0
Pending writes: LRU 0, flush list 0, single page 0, flush chunk 0
Pages made young 1253517491, not young 356599282
41.82 youngs/s, 1.57 non-youngs/s
Pages read 43890768, created 24612154, written 5881921
0.52 reads/s, 0.00 creates/s, 1.05 writes/s
Buffer pool hit rate 1000 / 1000, young-making rate 0 / 1000 not 0 / 1000
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 48106, unzip_LRU len: 0
I/O sum[713]:cur[8], unzip sum[0]:cur[0]
---BUFFER POOL 7
Buffer pool size   49133
Free buffers       1024
Database pages     48109
Old database pages 17738
Modified db pages  946
Pending reads      0
Pending writes: LRU 0, flush list 0, single page 0, flush chunk 0
Pages made young 1061861709, not young 331614044
36.64 youngs/s, 2.98 non-youngs/s
Pages read 41468213, created 24728698, written 5822616
0.52 reads/s, 0.00 creates/s, 1.05 writes/s
Buffer pool hit rate 1000 / 1000, young-making rate 1 / 1000 not 0 / 1000
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 48109, unzip_LRU len: 0
I/O sum[713]:cur[8], unzip sum[0]:cur[0]
--------------
ROW OPERATIONS
--------------
0 queries inside InnoDB, 0 queries in queue
0 read views open inside InnoDB
Process ID=95926, Main thread ID=140358298105600 , state=sleeping
Number of rows inserted 96915973412, updated 77543189, deleted 5125954, read 802055717850
200.73 inserts/s, 23.21 updates/s, 0.00 deletes/s, 171210.09 reads/s
Number of system rows inserted 25334914, updated 1089860, deleted 25334003, read 113221378
9.79 inserts/s, 0.32 updates/s, 10.53 deletes/s, 11.74 reads/s
----------------------------
END OF INNODB MONITOR OUTPUT
============================
```
## QPS
Questions = SHOW GLOBAL STATUS LIKE 'Questions';
Uptime = SHOW GLOBAL STATUS LIKE 'Uptime';
QPS=Questions/Uptime

## TPS
Com_commit = SHOW GLOBAL STATUS LIKE 'Com_commit';
Com_rollback = SHOW GLOBAL STATUS LIKE 'Com_rollback';
Uptime = SHOW GLOBAL STATUS LIKE 'Uptime';
TPS=(Com_commit + Com_rollback)/Uptime

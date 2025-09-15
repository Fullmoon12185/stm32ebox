[33m7ce1d0c[m[33m ([m[1;36mHEAD -> [m[1;32meboxV6[m[33m, [m[1;32meboxV7_quectel[m[33m)[m update sending mqtt to less consumption data
[33m55aadfa[m[33m ([m[1;31morigin/eboxV6[m[33m)[m version 6.2.6
[33m8ae3a02[m latest version uisng internal clock
[33m4a6a2a8[m update to .1.3
[33md50dd78[m version .0.5
[33m87c8cab[m this version is OK
[33m8e7e582[m update display LCD from uint32_t to uint64_t
[33m722d3e8[m add at+csq for reading 4g signal strength
[33m22a4d5f[m done version 6
[33m8b9a385[m Merge pull request #6 from Fullmoon12185/feature/power-meter
[33mbd92519[m[33m ([m[1;31morigin/feature/power-meter[m[33m)[m feat: power meter
[33m273e2fc[m done first version of new boxes 8 outlets
[33me27beac[m[33m ([m[1;32meboxV6_8outlets[m[33m)[m first commit for verion 6 done testing all except rs485 and wifi lan
[33m5f59a7d[m update for checking mqtt connected and subscribed ok
[33m7486c70[m[33m ([m[1;31morigin/eboxV2[m[33m, [m[1;32meboxV5_old[m[33m, [m[1;32meboxV2[m[33m)[m fix update firmware - power off before jumping to fota version
[33m9c4fd19[m add sms boot up and alive
[33m4781863[m add off pdu mode before sending a text message
[33mba6a94a[m add send sms message
[33mb07ec79[m check full charge with threshold 2/3 max current
[33m53c2ec3[m change the time for detecting full charge
[33me00e8ba[m ignore *.log
[33mafb5fd0[m add logData folder
[33me66b1ed[m add relay_broken message
[33m1ffc599[m distinguish between unplug and full charge
[33m556f39b[m add max current checking
[33m9200a61[m change address for firmware update, adjust minimum value for caculation
[33mc8c77fe[m fix read status after fota
[33m6316d37[m merge conflict
[33m54e6247[m change flash to RE chip
[33med4c1fe[m merge from feature-fota
[33ma68051c[m Merge branch 'eboxV2' of https://github.com/Fullmoon12185/stm32ebox into eboxV2
[33m36d5abd[m add coeff for 10A outlet
[33mde311b8[m[33m ([m[1;31morigin/eboxV2-feature-FOTA[m[33m)[m add feature update FOTA status after FOTA Process
[33m1140062[m change command on FE topic to 0x for fota feature
[33m83cf7d6[m add log buffer for print VERSION number
[33m1e250ad[m add topic for fota and jump to fota function
[33m77079de[m add app_version and app_flash
[33mab57c91[m testing for v5
[33m1f39528[m eboxv5 ok
[33m2ae5193[m not done yet
[33me01a023[m version OK before  change to uint64_t
[33mb32a967[m change max current of outlet 1 2 to 9.5A at XI
[33m1b0d78d[m add delay at the beginning of charging 20s
[33m1ca8e83[m change for box v3
[33m5f15190[m get rid of unplug
[33m08ef8d2[m new data for 10A
[33mf727c21[m Update code for version 4
[33m71c2a86[m XI_Box_Change_Box_Power_7W_Volt_235_Add_delay_5s_Min_300mA_7h44_29102021
[33ma4ed3fd[m change min current 600mA
[33me100045[m add scan i2c
[33me906cc1[m copy flash config file
[33m153a1de[m add longer time for sim4g stable
[33m21d8203[m start version 3
[33md6c6388[m add update total power consumption via CEbox_ topic
[33m0eef7d4[m fix overflow in power calculation
[33m4bcd007[m modify Amp computation
[33m6864fbc[m update new mqtt server
[33m31ec75a[m add timeout for network connection lost
[33m4ea2817[m modify the chargefull threshold
[33m97b93bd[m modify code to run blackbox V1
[33mc2a82ff[m ready for V3
[33m0e46870[m led order reverser
[33m6c8b1cd[m add test
[33m04fef7c[m change led order
[33m937747e[m nothing
[33me127a38[m fix server name
[33md1aad0f[m add testing
[33mc593f35[m fix problem and leave it to Stefan
[33m4c1c706[m not fix the new problem \r\n
[33m6e11ba8[m fix sim7600
[33m74b7312[m new push test with opamp
[33mcbd1966[m test adc
[33m62e485d[m modify app_fsm to publish STATUS topic as fast as possible
[33mb96925f[m test hardfault-handler
[33m6866224[m modify eeprom read/write
[33m6eb4915[m v2 ready for demo 290420
[33m9d155dc[m fix eeprom
[33mb487d61[m add watchdog
[33m081bd85[m eeprom testing
[33mfca0853[m ready for quatest 3 test
[33mf768587[m[33m ([m[1;31morigin/Test3G[m[33m)[m already for test on 3 April, 2020
[33m0f289e2[m done with timer O1
[33m3d6a572[m add timer update O(1)
[33mb6cb483[m add watchdog timer
[33m53e0083[m ready for demo on Saturday 21032020
[33mb8de056[m demo 1
[33m6560b97[m add power.c and led done
[33m8776018[m first demo that uses app to turn on/off each outlet
[33m3cf4d55[m change number of relay to all buffer in adc.c
[33m2a9ae4b[m can measure power consumption
[33m8d558f6[m first demo, using app to control the board
[33m0207bc5[m new uart receive and processing data for multiple relay controlling
[33m86d85d9[m mqtt done
[33m1d0510e[m test new board up to adc functions
[33md20a05e[m add app_led_display module
[33mcfd6b3d[m add code for sim3g.c
[33m73316b2[m add app_relay app_pcf8574 app_flash_database
[33mbceb5b0[m add gpio init code
[33me700d52[m add gpio init code add code for 25lc512 but need to test it before using
[33mf9f3e50[m add code for 25lc512 but need to test it before using
[33m9b2f798[m Merge branch 'master' of https://github.com/Fullmoon12185/stm32ebox
[33md12b3cf[m Merge branch 'master' of https://github.com/Fullmoon12185/stm32ebox
[33ma20e6cd[m add project file
[33m6753e3c[m add project file
[33m77b5f4c[m Merge branch 'master' of https://github.com/Fullmoon12185/stm32ebox
[33me2afc93[m Merge branch 'master' of https://github.com/Fullmoon12185/stm32ebox
[33m33973b1[m Merge branch 'master' of https://github.com/Fullmoon12185/stm32ebox
[33m1636590[m Merge branch 'master' of https://github.com/Fullmoon12185/stm32ebox
[33m425b1c8[m Initial commit
[33m535477f[m adding app_xxx.c app_xxx.h timer running OK
[33mcf364a9[m first commit for eBox

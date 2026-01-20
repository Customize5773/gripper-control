'CNC type robot controller. Four servos provide motions to the one cubic inch neodymium magnet which the gimbal mounted magnet in the robot follows.

high 3'serial output pin
pause 7000

'set to 0 position
    serout 3, t2400, ($80, $01, $04, 1, 35, 127) 'position s1 13-24-35 counter-clockwise
    serout 3, t2400, ($80, $01, $04, 0, 35, 127) 'position s0 c-clock
    pause 7000

'level magnet
    serout 3, t2400, ($80, $01, $04, 3, 23, 127) 'position mid
    pause 1000

'move forward long servo1
    serout 3, t2400, ($80, $01, $04, 1, 21, 127) 'position clockwise
    pause 1500
'grip down
    serout 3, t2400, ($80, $01, $04, 3, 26, 127) 'position down
    pause 2000
'close grip
    serout 3, t2400, ($80, $01, $04, 2, 25, 1) 'slow speed clock
    pause 50
serout 3, t2400, ($80, $01, $00, 2, 0, 127) 'stop servo 2 rotate
    pause 700
'move forward short
    serout 3, t2400, ($80, $01, $04, 1, 13, 127) 'position clock
    pause 1000
'grip up
    serout 3, t2400, ($80, $01, $04, 3, 23, 127) 'position midpoint
    pause 700
'turn right 90 degrees
    serout 3, t2400, ($80, $01, $04, 2, 25, 1) 'slow speed clock
    pause 470

serout 3, t2400, ($80, $01, $00, 2, 0, 127) 'stop servo 2 rotate
    pause 1000

'forward
    serout 3, t2400, ($80, $01, $04, 0, 13, 12) 'position s0
    pause 1500
'grip down
    serout 3, t2400, ($80, $01, $04, 3, 25, 12) 'position down
    pause 2000
'close grip
    serout 3, t2400, ($80, $01, $04, 2, 25, 1) 'slow speed c-clockwise
    pause 50
    serout 3, t2400, ($80, $01, $00, 2, 0, 127) 'stop servo 2 rotate
    pause 400
'back up
    serout 3, t2400, ($80, $01, $04, 0, 35, 127) 'position s0 c-clock
    pause 700
'grip up
    serout 3, t2400, ($80, $01, $04, 3, 22, 12) 'position mid
    pause 1000

pause 6000

'set to 0 position
    serout 3, t2400, ($80, $01, $04, 1, 35, 127) 'position s1 13-24-35 counter-clockwise
    serout 3, t2400, ($80, $01, $04, 0, 35, 127) 'position s0 c-clock

loop:
    goto loop
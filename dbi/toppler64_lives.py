#!/usr/bin/env python3
import frida
import sys

session = frida.attach('toppler64')
print('Attached to Toppler!')
script = session.create_script(r"""
'use strict';
let PTS_DIED = ptr(0x413ffb)
let LIVES = ptr(0x42b8f0)
Interceptor.attach(PTS_DIED, {
	onLeave: function(args){
		const lives = LIVES.readS8()
		if(lives<=0) console.log("It's game over, sorry!")
		else console.log("Ouch! You still have "+lives+" lives left.")
	}
})
""")
script.load()
sys.stdin.readline()
session.detach()

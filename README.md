# Blink a morse code!

An Arduino project for a simple morse code blinker.

Original project was built on an Arduino Uno.

![Circuit diagram for this project](diagram-morse-code-blinker.png)

## How do I start morse'ing?

Short answer is via the serial monitor.

Once you've uploaded the code to your board you'll need to set up the serial monitor's port and baud. So, for example I'd set my port to be `/dev/tty.usbserial-130 - `. The baud rate is `115200`. After see the serial monitor print out a friendly `Hi! What're we blinking?` you can type a message and send it as text and, so long as your circuit is setup correctly, you should be enjoying the teeny tiny flashes of communication.

If you want to do away with the serial output for each letter/phrase you can set `debugMode` to `false`.

## This ain't that great

Yep. I know.

I know I'm an asshole programmer spoiled by web development's boundless system resources, the wild west of dynamic typing, a laissez-faire attitude to performance and the shocked and indignant reaction to the question of maintainability.

If anyone's willing to make some comments or PRs on how to improve this lacklustre code, please do. I mean, lets be civil and nice about the whole ordeal, but I'm definitely open to improving this pile of monkeyed code.

## I wanna make some chages, cool with that?

Yeah, go for it. Play around and do what you want.

I'd suggest running the basic tests I've included. Just change `testMode` to `true` and so long as all the tests pass i'd say you're good to go.

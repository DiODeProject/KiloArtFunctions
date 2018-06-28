# Kilobot Art Functions
Main program can be found in Behaviour Generator. There are python code and a template code that can generate a kilobot code from a script

## Getting Started
This program requires python3 environment. You can download python environment here:
> https://www.python.org/downloads/

The format of the script is that:

######one line script:
> amount of time in ticks, tab, LED colour, tab, Kilobot Motion, tab, whether to send message

Example:
```
150	RED	STRAIGHT	MSG
100	BLACK	STOP	NO_MSG
```
This will result the behaviour that the kilobot moving straight with red light on while sending messages,
after 150 ticks (around 5 second), it executes next action, which is turning off the light and stop and not sending messages.
It will keep doing the last action forever as there is nothing telling it to change.
However, if the last action inclues sending messages, it will terminate sending message after the amount of time specified.

######special scipt using loops:

start with:
> REPEAT,tab,number of times repeating or some special condictions

end by:
> REPEAT,space,END

Example:
```
REPEAT	5
20	GREEN	LEFT	NO_MSG
20	BLUE	RIGHT	NO_MSG
REPEAT END
```
this will repeats the actions inside the loop for 5 times

```
REPEAT	MSG
20	GREEN	LEFT	NO_MSG
20	BLUE	RIGHT	NO_MSG
REPEAT END
```
this will repeats the actions util the kilobot receives a message. you can also put NO_MSG after REPEAT, it means that the kilobot will repeat until it does not receive message.

```
REPEAT	0
20	GREEN	LEFT	NO_MSG
20	BLUE	RIGHT	NO_MSG
REPEAT END
```
this will repeats the actions forever

please make sure that the loop is closed with `REPEAT END`, otherwise there will be syntax error when you try to compile the resulted code.

You can save the script as a txt file.

######Command to generate c file
```
python3 -t template.c -r [the name of the script file you wrote] -w [the file name of the output code]
```
this will generate the c file. After that you can follow the instruction of manipulating kilobots to test the output code.

You can find the instruction of kilobots here:
> https://www.kilobotics.com/documentation


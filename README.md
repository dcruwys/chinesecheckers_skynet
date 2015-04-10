# Chinese Checkers (Skynet)
A (hopefully) intelliegent AI that plays Chinese Checkers better than most humans.

Created by Dan.Cruwys@du.edu and Paul.Heinen@du.edu

The neat AgentTester.jar was created by William.Mitchell@du.edu.

Traditonal Game AI COMP3703
--------------
**Instructor:**
Dr. Nathan Sturtevant
sturtevant@cs.du.edu

**TA:**
Will Mitchell
William.Mitchell@du.edu

**Course Description:**
This course covers traditional work in the area of AI for games. Students in the course will learn about the history of AI and games, and the techniques used to create human-level AI for traditional games. Topics include approaches for games of perfect and imperfect information and games with two or more players.

How to Run
-----------
Inside the binaries folder, there are a few differant types of binaries. As of right now, there are only the linux binaries.

Each one represents a differant state of the AI in chronological order:

1. **heurstic.out**, a basic "choose the farthest postion" implentation. 
2. **minimax.out**, a greedy minimax function. You can read about minimax [here.](http://en.wikipedia.org/wiki/Minimax)
3. **ideepening.out**, a interative deepening of the minimax, given ~10s per move. At this point it is not effective.
4. **ChineseCheckersRandom**, chooses a random move.
5. **ChineseCheckersModerator**, contains the rules and decides the winner. This is not a player and only for GameMaster.jar.

There are also two jars in the root folder.

1. **AgentTester.jar**, runs a SINGLE binary agent to test it's moves.
2. **GameMaster.jar**, simulates a game given two player agent binaries and a moderator binary.

To run, select a jar, and then select the binaries for each of the top parameters. AgentTester runs one move at a time, GameMaster simulates automaticly.


You can find more detailed notes on the jars [here.](https://github.com/wtmitchell/tradgames)

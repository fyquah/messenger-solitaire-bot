# Messenger Solitaire Bot

A bot to play solitaire in messenger.

<a href="http://www.youtube.com/watch?feature=player_embedded&v=xFNd-foQYrs"
target="_blank"><img src="http://img.youtube.com/vi/xFNd-foQYrs/0.jpg" 
alt="Video Demo" width="480" height="360" border="10" /></a>

## Usage

Compiling: `make`

Running: `make run`

## Source Code Organization

There are a few components to it:

1. JNI interface with the `Robot` class to capture screenshots, control the
   mouse and keyboards.
2. Using template matching in OpenCV, new cards are identified. That is
   converted into a game state.
3. The bot "cheats" by going through all the cards in the draw pile in the
   beginning.
4. The game state is purely markovian, hence a function `move_t solve(state_t)`
   can decide the best move.
5. As soon as the solver encounters a fixed point (that is
   `execute(state, solve(state)) == state`), the program terminates.

I had some problems getting everything running with a headless JVM (i believe
it is some problems with X server or something along that line), hence, I
ran everything in a java proces, calling into JNI method on every frame.

## Projected FAQ

0. How easy is it to get it running on my own machine?

I am glad you are interested in this project! You will need to tune some
vision parameters in `test/vision.hpp` depending on your  machine, and where
you decide to position your window.  I wish to automate this process, at
some point.

*1. Why don't you machine learning this?*

Much wow.

*2. Why don't you treat this as a markovian decision problem and run some
   TD-learning algorithms on this?*

I didn't really understand reinforcement learning (I still don't), but from
what I can gather, this is a non-deterministic purely markovian decision
problem with perfect information. This gives it really nice properties:

- randomness of the game prevents it from stucking in local optima
- it has a finite discrete action space `7 + ((7 + 1) * 4) + 1 + 4`

so, I'd expect even the simplest greedy TD-learning algorithm like those
in TD-backgammon work.

there has been quite a bit of previous work in this -- a simple search 
in google scholars yield some interesting results. A fun experiment
to try "learning" is see if a policy to "remember" what it has seen
in the draw pile without explicitly adding it in the state.

*3. Why did you have to partially implement this in C++, rather than just use
Java on the whole stack?*

I don't want to java.

*4. The code looks like a mess. Wh....*

I agree.

*5. Why `number_t` rather than `rank_t`?*

I am not a native english speaker. I didn't know that it is called rank
until I shared the project with my friends.

## License

MIT

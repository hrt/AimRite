# Aimrite - Battlerite Aimbot
A simple hackathon like project based around the game http://store.steampowered.com/app/504370/Battlerite/

[![Alt text](https://img.youtube.com/vi/RPr5pDSZ7gE/0.jpg)](https://www.youtube.com/watch?v=RPr5pDSZ7gE)

https://www.youtube.com/watch?v=RPr5pDSZ7gE

# Build (Windows)
Change ```SCREEN_WIDTH```, ```SCREEN_HEIGHT```, ```WINDOW_WIDTH``` and ```WINDOW_HEIGHT``` definitions to your screen dimensions.

To build simple clone and open the solution in Visual Studio 2015 and build under Debug x86

It's recommended that you play on locked screen.

Change resolution in ```main.cpp``` and ```MouseManager.cpp```

Adjust the number 69 in these lines (around 284 in ```main.cpp``` https://github.com/ih1115/BattleriteAimbot/blob/master/main.cpp#L239)
```
vec->x = 1920 / 2 + dx * 69;
vec->y = 1080 / 2 - dy * 69;
```
Until the aimbot places the cross-hair directly on-top of the enemy


Note : if Battlerite has updated since the last commit then it's quite likely that you will have to update ```Offsets.hpp```.


# ~~Jade Scripts~~
~~Anti-gap closer -> R if near~~

~~Space if in range for stun away from enemy~~

~~Auto Ex Sniper -> if not close but still in range~~

~~Auto Ex Stealth -> if an ally is alive near you (or no alive allies are near you) and enemy is near OR if projectile is going to hit you~~

Out of boredom, I recently updated ```Offsets.hpp``` for aimbot only and removed all script code for now..
Obviously scripts are what makes this OP and they will come if I'm still bored

# cannon-clash
* This is a program made in Raylib C++. It is a small, fun game made in C++.
* The Player is a cannon with a rotatable turret that can shoot cannon balls.
* The Player can charge their attack by holding down the space bar.
* Once the Player has charged, with any amount of charge level, a cannon ball shoots.
* The higher the time taken to charge, the more the fire power.
* However, if the Player shoots to fast, with less charge, the turret overheats, and the player is unable to shoot for 6 seconds.
* The cannon balls have physics, where it will bounce with contact with the ground, with gravity.
* The cannon ball stored how much charge the Player had, when the cannon ball was shot.
* Spawning every round in pairs, there are cars with snipers on top that shoot bullets in a straight line, at the player. They are called SniperCars.
* The reload time of the SniperCar varies between 0.8–1.2 seconds.
* The SniperCars have unlimited ammo, and have 30 health points.
* SniperCars do damage randomly from 3–5HP to the Player, when the bullet hits the Player.
* SniperCars are the main enemy to the Player, and the Player has to try to shoot them all down.
* Each cannon ball does damage from 1–10HP to the SniperCar, when the cannon ball lands on the SniperCar, depending on the charge amount of the player during the cannon ball was shot.
* When a Player reduces a SniperCar's health to 0HP, the SniperCar dies, and a gold Coin spawns in the SniperCar's place.
* The Player can pick up these Coins to increase their Coins counter.
* When the Player has a minimum of 5 Coins, they can purchase a Drone, when they are in a state of emergency, which costs 5 Coins.
* When a Drone is purchased, it comes down from the sky, and starts shooting bullets at the SniperCars, with perfect aim.
* Drones can be bought again and again with no cooldown by the player.
* Drones cannot be harmed by SniperCars.
* The reload time of the Drone varies between 0.8–1.2 seconds.
* Drones do damage randomly from 3–5HP to the SniperCar, when the bullet hits the SniperCar.
* The Drones last only for 15 seconds, and despawn after the time period ends.
* While in the home screen, the Player can choose between the different game difficulties:
  - Peaceful Difficulty: In this game difficulty, the player does not lose any health. This game difficulty is good for users who want to practice their skills.
  - Easy Difficulty: In this game difficulty, the Player has a maximum health of 200HP. This game difficulty is good for casual users.
  - Normal Difficulty: In this game difficulty, the Player has a maximum health of 150HP. This game difficulty is good for users who got a hang of the game.
  - Hard Difficulty: In this game difficulty, the Player has a maximum health of 100. This game difficulty is good for users who are pros at the game.
* The controls of the game:
  - Move Left: Key A
  - Move Right: Key D
  - Charge/Shoot: Key Space
  - Move Turret Left: Key Q OR Left Mouse Button
  - Move Turret Right: Key E OR Right Mouse Button

# OptiSail

## About
In this project optimization of a sailing boat course is performed. When sailing the boat can only be driven with wind and there is a huge problem when you need to reach a distant point and the direction towards it is perfectly opposite to that of the wind. So how can we move the boat while moving against the wind? Well, there is a sailing technoque that leverages counterintuitive properties of a sail to move in a way that in sailing terms is refered to as *tacking*. 
When tacking boat is stirred to move roughly at a 45 degrees angle with the wind as shown on the figure below.

![tacking](/image.png)

Here we can see that a sail is surprisingly used as *wing*. A wing that is placed on its side and that works in quite the same way with the only difference that the lifting force it helps to produce is directed in the XY plane. And here we can use this fact to move against the wind. 

In this project a simple approach was taken to develop a mechanical  model of the sailing boat. Using the model we can reproduce the sails way under certain circumstanes. The parameters are wind direction and velocity, intial position and velocity of the boat and some sail parameters.
Using this model an optimization of the sail's route is performed. In the specific case when wind faces the boat optimal way is found to be the well known among sailors tacking route. Some examples are shown below.
The case of facing against wind, two points (minimum possible) are chosen for building the path:
 
 ![against2](/against2.png)
 
The case going against the wind, five points are allowed to be used:

 ![against5](/against5.png)
 
 Here we see that algorithm essentially "omitted" the unnecessary points and only used two.
 And here is the obvious optimal way for the case of going with the wind:
 
 ![towards](/towards.png)
